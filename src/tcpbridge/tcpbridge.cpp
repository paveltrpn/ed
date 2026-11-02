#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>

#include <uv.h>

#include "tcpbridge/tcpbridge.h"

namespace tire {

TcpBridge::TcpBridge( size_t maximumPayload )
    : _scheduler{ std::make_unique<Scheduler>() }
    , _maximumPayload{ maximumPayload } {
}

TcpBridge::~TcpBridge() {
    //
    close();
}

auto TcpBridge::accept( std::string host, uint16_t port ) -> Task<int> {
    auto listenAwaitable = std::make_shared<TcpListenAwaitable>( *_scheduler, std::move( host ), port );

    auto status = 0;
    {
        std::scoped_lock lock{ _sessionMutex };

        if ( _closing ) {
            status = UV_ECANCELED;
        } else {
            _listener = listenAwaitable->listener();
        }
    }

    if ( status != 0 ) {
        co_return status;
    }

    const auto listenResult = co_await * listenAwaitable;

    if ( listenResult._status != 0 ) {
        co_return listenResult._status;
    }

    auto listener = std::shared_ptr<TcpListener>{};
    {
        std::scoped_lock lock{ _sessionMutex };

        if ( !_closing ) {
            listener = _listener;
        }
    }

    if ( listener == nullptr ) {
        co_return UV_ECANCELED;
    }

    const auto acceptResult = co_await _scheduler->accept( listener );

    if ( acceptResult._status != 0 ) {
        co_return acceptResult._status;
    }

    {
        std::scoped_lock lock{ _sessionMutex };

        if ( _closing ) {
            status = UV_ECANCELED;
        } else {
            _connection = acceptResult._connection;
        }
    }

    if ( status != 0 ) {
        _scheduler->close( acceptResult._connection );
        co_return status;
    }

    co_return 0;
}

auto TcpBridge::connect( std::string host, uint16_t port, TcpBridgeRetryOptions retry ) -> Task<int> {
    if ( retry._maximumAttempts == 0 ) {
        co_return UV_EINVAL;
    }

    auto lastStatus = static_cast<int>( UV_ECANCELED );

    for ( size_t attempt = 0; attempt < retry._maximumAttempts; ++attempt ) {
        auto connect = std::make_shared<TcpConnectAwaitable>( *_scheduler, host, port );

        {
            std::scoped_lock lock{ _sessionMutex };

            if ( _closing ) {
                lastStatus = UV_ECANCELED;
                break;
            }

            _connect = connect;
        }

        const auto connectResult = co_await * connect;

        {
            std::scoped_lock lock{ _sessionMutex };

            if ( _connect == connect ) {
                _connect.reset();
            }
        }

        lastStatus = connectResult._status;

        if ( lastStatus == 0 ) {
            {
                std::scoped_lock lock{ _sessionMutex };

                if ( _closing ) {
                    lastStatus = UV_ECANCELED;
                } else {
                    _connection = connectResult._connection;
                }
            }

            if ( lastStatus != 0 ) {
                _scheduler->close( connectResult._connection );
            }

            co_return lastStatus;
        }

        if ( lastStatus == UV_ECANCELED || _closing ) {
            break;
        }

        if ( attempt + 1 < retry._maximumAttempts ) {
            auto timer = std::make_shared<TimeoutAwaitable<Task<void>>>( *_scheduler, retry._delayMilliseconds );
            {
                std::scoped_lock lock{ _sessionMutex };

                if ( _closing ) {
                    lastStatus = UV_ECANCELED;
                    break;
                }

                _retryTimer = timer;
            }

            const auto timerStatus = co_await * timer;

            {
                std::scoped_lock lock{ _sessionMutex };

                if ( _retryTimer == timer ) {
                    _retryTimer.reset();
                }
            }

            if ( timerStatus != 0 ) {
                lastStatus = timerStatus;
                break;
            }
        }
    }

    co_return lastStatus;
}

auto TcpBridge::read() -> Task<TcpBridgeMessage> {
    while ( true ) {
        auto connection = std::shared_ptr<TcpConnection>{};
        auto message = std::vector<std::byte>{};
        auto hasMessage = false;
        auto status = 0;

        {
            std::scoped_lock lock{ _sessionMutex };

            if ( !_pendingMessages.empty() ) {
                message = std::move( _pendingMessages.front() );
                _pendingMessages.pop_front();
                hasMessage = true;
            } else if ( _closing ) {
                status = UV_ECANCELED;
            } else {
                connection = _connection;
            }
        }

        if ( hasMessage ) {
            co_return TcpBridgeMessage{ 0, std::move( message ) };
        }

        if ( status != 0 ) {
            co_return TcpBridgeMessage{ status, std::vector<std::byte>{} };
        }

        if ( connection == nullptr ) {
            co_return TcpBridgeMessage{ UV_ENOTCONN, std::vector<std::byte>{} };
        }

        const auto readResult = co_await _scheduler->read( connection );

        if ( readResult._status != 0 ) {
            if ( readResult._status != UV_EOF ) {
                co_return TcpBridgeMessage{ readResult._status, std::vector<std::byte>{} };
            }

            auto endStatus = UV_EOF;
            {
                std::scoped_lock lock{ _sessionMutex };

                if ( _codec.endOfStream() == FrameStatus::PartialEof ) {
                    endStatus = UV_EPROTO;
                }
            }

            co_return TcpBridgeMessage{ endStatus, std::vector<std::byte>{} };
        }

        {
            std::scoped_lock lock{ _sessionMutex };

            const auto decoded = _codec.push( readResult._bytes );

            if ( decoded._status == FrameStatus::Oversized ) {
                status = UV_EPROTO;
            } else {
                for ( auto &decodedMessage : decoded._messages ) {
                    _pendingMessages.push_back( std::move( decodedMessage ) );
                }

                if ( !_pendingMessages.empty() ) {
                    message = std::move( _pendingMessages.front() );
                    _pendingMessages.pop_front();
                    hasMessage = true;
                }
            }
        }

        if ( status != 0 ) {
            co_return TcpBridgeMessage{ status, std::vector<std::byte>{} };
        }

        if ( hasMessage ) {
            co_return TcpBridgeMessage{ 0, std::move( message ) };
        }
    }
}

auto TcpBridge::send( std::vector<std::byte> message ) -> Task<int> {
    auto connection = std::shared_ptr<TcpConnection>{};
    auto status = 0;

    {
        std::scoped_lock lock{ _sessionMutex };

        if ( _closing ) {
            status = UV_ECANCELED;
        } else {
            connection = _connection;
        }
    }

    if ( status != 0 ) {
        co_return status;
    }

    if ( connection == nullptr ) {
        co_return UV_ENOTCONN;
    }

    if ( message.size() > _maximumPayload || message.size() > std::numeric_limits<uint32_t>::max() ) {
        co_return UV_EMSGSIZE;
    }

    auto framed = FrameCodec::encode( message );
    const auto writeResult = co_await _scheduler->write( connection, std::move( framed ) );

    co_return writeResult._status;
}

auto TcpBridge::delay( uint64_t timeoutMilliseconds ) -> Task<int> {
    auto timer = std::make_shared<TimeoutAwaitable<Task<void>>>( *_scheduler, timeoutMilliseconds );
    auto status = 0;

    {
        std::scoped_lock lock{ _sessionMutex };

        if ( _closing ) {
            status = UV_ECANCELED;
        } else if ( _delayTimer != nullptr ) {
            status = UV_EBUSY;
        } else {
            _delayTimer = timer;
        }
    }

    if ( status != 0 ) {
        co_return status;
    }

    status = co_await * timer;

    {
        std::scoped_lock lock{ _sessionMutex };

        if ( _delayTimer == timer ) {
            _delayTimer.reset();
        }
    }

    co_return status;
}

auto TcpBridge::close() -> void {
    _closing = true;

    auto listener = std::shared_ptr<TcpListener>{};
    auto connection = std::shared_ptr<TcpConnection>{};
    auto connect = std::shared_ptr<TcpConnectAwaitable>{};
    auto retryTimer = std::shared_ptr<TimeoutAwaitable<Task<void>>>{};
    auto delayTimer = std::shared_ptr<TimeoutAwaitable<Task<void>>>{};

    {
        std::scoped_lock lock{ _sessionMutex };

        listener = _listener;
        connection = _connection;
        connect = _connect;
        retryTimer = _retryTimer;
        delayTimer = _delayTimer;
    }

    if ( retryTimer != nullptr ) {
        retryTimer->cancel();
    }

    if ( delayTimer != nullptr ) {
        delayTimer->cancel();
    }

    if ( connect != nullptr ) {
        connect->cancel();
    }

    if ( listener != nullptr ) {
        _scheduler->close( listener );
    }

    if ( connection != nullptr ) {
        _scheduler->close( connection );
    }
}

}  // namespace tire
