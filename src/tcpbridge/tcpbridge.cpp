#include "tcpbridge/tcpbridge.h"

#include <atomic>
#include <condition_variable>
#include <coroutine>
#include <deque>
#include <format>
#include <limits>
#include <mutex>
#include <optional>
#include <utility>

#include "io/async.h"
#include "io/scheduler.h"
#include "io/tcpclient.h"
#include "io/tcpserver.h"
#include "io/timer.h"

namespace tire {

struct TcpBridge::State {
    explicit State( Scheduler *scheduler, size_t maximumPayload )
        : _scheduler{ scheduler }
        , _maximumPayload{ maximumPayload } {}

    Scheduler *_scheduler{ nullptr };
    Callbacks _callbacks{};
    size_t _maximumPayload{ FrameCodec::_defaultMaximumPayload };
    std::atomic<bool> _started{ false };
    std::atomic<bool> _closeRequested{ false };
    std::atomic<bool> _closing{ false };
    bool _connected{ false };
    bool _errorReported{ false };
    std::shared_ptr<TcpClient> _client{};
    std::shared_ptr<TcpServer> _server{};
    std::shared_ptr<TcpConnection> _connection{};
    std::optional<TimeoutAwaitable<Task<void>>> _retryTimer{};
    std::deque<Message> _outbound{};
    std::mutex _outboundMutex{};
    std::coroutine_handle<> _drainCoroutine{};
    bool _drainWaiting{ false };
    Task<void> _clientTask{};
    Task<void> _serverTask{};
    Task<void> _drainTask{};
    std::mutex _workerMutex{};
    std::condition_variable _workerCondition{};
    size_t _workerCount{ 0 };
};

namespace {

using BridgeState = TcpBridge::State;

struct WorkerCompletion {
    std::shared_ptr<BridgeState> _state{};

    ~WorkerCompletion() {
        _state->_scheduler->post( [state = _state]( uv_loop_t * ) -> void {
            std::scoped_lock lock{ state->_workerMutex };
            if ( state->_workerCount > 0 ) {
                --state->_workerCount;
            }
            if ( state->_workerCount == 0 ) {
                state->_workerCondition.notify_all();
            }
        } );
    }
};

struct DrainAwaitable {
    std::shared_ptr<BridgeState> _state{};

    auto await_ready() const noexcept -> bool { return false; }

    auto await_suspend( std::coroutine_handle<> coroutine ) noexcept -> void {
        _state->_drainCoroutine = coroutine;
        _state->_drainWaiting = true;
    }

    auto await_resume() const noexcept -> void {}
};

auto resumeDrain( const std::shared_ptr<BridgeState> &state ) -> void {
    if ( !state->_drainWaiting ) {
        return;
    }

    state->_drainWaiting = false;
    const auto coroutine = std::exchange( state->_drainCoroutine, nullptr );
    if ( coroutine ) {
        coroutine.resume();
    }
}

auto reportError( const std::shared_ptr<BridgeState> &state, int status, std::string context ) -> void {
    if ( state->_errorReported ) {
        return;
    }

    state->_errorReported = true;
    if ( state->_callbacks._error ) {
        state->_callbacks._error( TcpBridge::Error{ status, std::move( context ) } );
    }
}

auto shutdown( const std::shared_ptr<BridgeState> &state ) -> void {
    if ( !state->_closing ) {
        state->_closing = true;
        if ( state->_retryTimer.has_value() ) {
            state->_retryTimer->cancel();
        }
        if ( state->_client != nullptr ) {
            state->_client->close();
        }
        if ( state->_server != nullptr ) {
            state->_server->close();
        }
        if ( state->_connection != nullptr ) {
            state->_connection->close();
        }
    }

    {
        std::scoped_lock lock{ state->_outboundMutex };
        state->_outbound.clear();
    }
    resumeDrain( state );
}

auto finishSession( const std::shared_ptr<BridgeState> &state ) -> void {
    const auto wasConnected = std::exchange( state->_connected, false );
    shutdown( state );
    if ( wasConnected && state->_callbacks._disconnected ) {
        state->_callbacks._disconnected();
    }
}

auto decodeChunk( const std::shared_ptr<BridgeState> &state, FrameCodec &codec, const TcpConnection::Bytes &bytes )
    -> bool {
    auto decoded = codec.push( bytes );
    for ( auto &message : decoded._messages ) {
        if ( state->_callbacks._message ) {
            state->_callbacks._message( std::move( message ) );
        }
    }

    if ( decoded._status == FrameStatus::Oversized ) {
        reportError( state, UV_EPROTO, "Received frame exceeds the configured payload limit" );
        shutdown( state );
        return false;
    }
    return true;
}

auto handleEof( const std::shared_ptr<BridgeState> &state, FrameCodec &codec ) -> void {
    if ( codec.endOfStream() == FrameStatus::PartialEof ) {
        reportError( state, UV_EPROTO, "Connection ended with a partial frame" );
    }
}

auto runDrain( std::shared_ptr<BridgeState> state ) -> Task<void> {
    const auto completion = WorkerCompletion{ state };

    while ( !state->_closing ) {
        auto message = TcpBridge::Message{};
        auto hasMessage = false;
        std::shared_ptr<TcpConnection> connection{};
        {
            std::scoped_lock lock{ state->_outboundMutex };
            if ( state->_connection != nullptr && !state->_outbound.empty() ) {
                message = std::move( state->_outbound.front() );
                state->_outbound.pop_front();
                connection = state->_connection;
                hasMessage = true;
            }
        }

        if ( !hasMessage ) {
            co_await DrainAwaitable{ state };
            continue;
        }

        auto framed = FrameCodec::encode( message );
        const auto result = co_await connection->write( std::move( framed ) );
        if ( result._status != 0 ) {
            if ( !state->_closing ) {
                reportError( state, result._status, "TCP write failed" );
                shutdown( state );
            }
            co_return;
        }
    }
}

auto runServer( std::shared_ptr<BridgeState> state, std::string host, uint16_t port ) -> Task<void> {
    const auto completion = WorkerCompletion{ state };
    if ( state->_closing ) {
        co_return;
    }

    auto server = std::make_shared<TcpServer>( *state->_scheduler );
    state->_server = server;

    const auto listenResult = co_await server->listen( host, port );
    if ( listenResult._status != 0 ) {
        if ( !state->_closeRequested.load() ) {
            reportError( state, listenResult._status, std::format( "Failed to listen on {}:{}", host, port ) );
        }
        shutdown( state );
        co_return;
    }
    if ( state->_closing ) {
        co_return;
    }

    auto acceptResult = co_await server->accept();
    server->close();
    if ( acceptResult._status != 0 ) {
        if ( !state->_closing ) {
            reportError( state, acceptResult._status, "Failed to accept TCP client" );
        }
        shutdown( state );
        co_return;
    }

    state->_connection = acceptResult._connection;
    state->_connected = true;
    if ( state->_callbacks._connected ) {
        state->_callbacks._connected();
    }
    resumeDrain( state );

    auto codec = FrameCodec{ state->_maximumPayload };
    while ( !state->_closing ) {
        const auto readResult = co_await state->_connection->read();
        if ( readResult._status == 0 ) {
            if ( !decodeChunk( state, codec, readResult._bytes ) ) {
                break;
            }
            continue;
        }
        if ( readResult._status == UV_EOF ) {
            handleEof( state, codec );
        } else if ( !state->_closing ) {
            reportError( state, readResult._status, "TCP read failed" );
        }
        break;
    }

    finishSession( state );
}

auto runClient( std::shared_ptr<BridgeState> state, std::string host, uint16_t port, TcpBridge::RetryOptions retry )
    -> Task<void> {
    const auto completion = WorkerCompletion{ state };
    if ( state->_closing ) {
        co_return;
    }

    if ( retry._maximumAttempts == 0 ) {
        reportError( state, UV_EINVAL, "TCP connect requires at least one attempt" );
        shutdown( state );
        co_return;
    }

    auto client = std::make_shared<TcpClient>( *state->_scheduler );
    state->_client = client;
    auto connectResult = TcpClient::ConnectResult{};
    connectResult._status = UV_ECANCELED;

    for ( size_t attempt = 0; attempt < retry._maximumAttempts; ++attempt ) {
        connectResult = co_await client->connect( host, port );
        if ( connectResult._status == 0 ) {
            break;
        }

        if ( state->_closing || state->_closeRequested.load() ) {
            co_return;
        }

        if ( attempt + 1 < retry._maximumAttempts ) {
            state->_retryTimer.emplace( *state->_scheduler, retry._delayMilliseconds );

            const auto timerStatus = co_await state->_retryTimer.value();

            state->_retryTimer.reset();

            if ( timerStatus != 0 ) {
                if ( !state->_closing ) {
                    reportError( state, timerStatus, "TCP connect retry timer failed" );
                    shutdown( state );
                }
                co_return;
            }
        }
    }

    if ( connectResult._status != 0 ) {
        if ( !state->_closing && !state->_closeRequested.load() ) {
            reportError(
                state, connectResult._status,
                std::format( "Failed to connect to {}:{} after {} attempts", host, port, retry._maximumAttempts ) );
        }

        shutdown( state );

        co_return;
    }

    state->_connection = connectResult._connection;
    state->_connected = true;
    if ( state->_callbacks._connected ) {
        state->_callbacks._connected();
    }

    resumeDrain( state );

    auto codec = FrameCodec{ state->_maximumPayload };

    while ( !state->_closing ) {
        const auto readResult = co_await state->_connection->read();

        if ( readResult._status == 0 ) {
            if ( !decodeChunk( state, codec, readResult._bytes ) ) {
                break;
            }
            continue;
        }

        if ( readResult._status == UV_EOF ) {
            handleEof( state, codec );
        } else if ( !state->_closing ) {
            reportError( state, readResult._status, "TCP read failed" );
        }

        break;
    }

    finishSession( state );
}

}  // namespace

TcpBridge::TcpBridge( size_t maximumPayload )
    : _scheduler{ std::make_unique<Scheduler>() }
    , _state{ std::make_shared<State>( _scheduler.get(), maximumPayload ) } {
}

TcpBridge::~TcpBridge() {
    close();
    _state->_clientTask = Task<void>{};
    _state->_serverTask = Task<void>{};
    _state->_drainTask = Task<void>{};
    _scheduler.reset();
    _state.reset();
}

auto TcpBridge::setCallbacks( Callbacks callbacks ) -> void {
    _state->_callbacks = std::move( callbacks );
}

auto TcpBridge::listen( std::string host, uint16_t port ) -> void {
    if ( _state->_started.exchange( true ) ) {
        _scheduler->post( [state = _state]( uv_loop_t * ) -> void {
            reportError( state, UV_EALREADY, "TCP bridge session was already started" );
        } );
        return;
    }

    {
        std::scoped_lock lock{ _state->_workerMutex };
        _state->_workerCount = 2;
    }

    _scheduler->post( [state = _state, host = std::move( host ), port]( uv_loop_t * ) mutable -> void {
        state->_serverTask = runServer( state, std::move( host ), port );
        state->_drainTask = runDrain( state );
    } );
}

auto TcpBridge::connect( std::string host, uint16_t port, RetryOptions retry ) -> void {
    if ( _state->_started.exchange( true ) ) {
        _scheduler->post( [state = _state]( uv_loop_t * ) -> void {
            reportError( state, UV_EALREADY, "TCP bridge session was already started" );
        } );
        return;
    }

    {
        std::scoped_lock lock{ _state->_workerMutex };
        _state->_workerCount = 2;
    }

    _scheduler->post( [state = _state, host = std::move( host ), port, retry]( uv_loop_t * ) mutable -> void {
        state->_clientTask = runClient( state, std::move( host ), port, retry );
        state->_drainTask = runDrain( state );
    } );
}

auto TcpBridge::send( Message message ) -> void {
    if ( _state->_closeRequested.load() ) {
        return;
    }

    if ( message.size() > _state->_maximumPayload || message.size() > std::numeric_limits<uint32_t>::max() ) {
        _scheduler->post( [state = _state]( uv_loop_t * ) -> void {
            reportError( state, UV_EMSGSIZE, "Outbound message exceeds the configured payload limit" );
            shutdown( state );
        } );
        return;
    }

    {
        std::scoped_lock lock{ _state->_outboundMutex };
        if ( _state->_closing ) {
            return;
        }
        _state->_outbound.push_back( std::move( message ) );
    }

    _scheduler->post( [state = _state]( uv_loop_t * ) -> void { resumeDrain( state ); } );
}

auto TcpBridge::close() -> void {
    if ( !_state->_closeRequested.exchange( true ) ) {
        _scheduler->post( [state = _state]( uv_loop_t * ) -> void { shutdown( state ); } );
    }

    if ( _scheduler->isLoopThread() ) {
        return;
    }

    std::unique_lock lock{ _state->_workerMutex };

    _state->_workerCondition.wait( lock, [state = _state]() -> bool { return state->_workerCount == 0; } );
}

}  // namespace tire
