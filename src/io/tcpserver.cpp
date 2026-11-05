#include "io/tcpserver.h"

#include <utility>

#include "io/scheduler.h"

namespace {

using ListenerState = tire::TcpListenerState;
using ListenAwaitable = tire::TcpListenAwaitable;
using AcceptAwaitable = tire::TcpAcceptAwaitable;

auto completeListen( ListenAwaitable *operation, int status ) -> void {
    if ( operation == nullptr ) {
        return;
    }

    auto *state = operation->_server.get();
    if ( state->_listenOperation == operation ) {
        state->_listenOperation = nullptr;
    }

    const auto coroutine = operation->complete( status );
    if ( coroutine ) {
        coroutine.resume();
    }
}

auto completeAccept( AcceptAwaitable *operation, int status ) -> void {
    if ( operation == nullptr ) {
        return;
    }

    auto *state = operation->_server.get();
    if ( state->_acceptOperation == operation ) {
        state->_acceptOperation = nullptr;
    }

    const auto coroutine = operation->complete( status );
    if ( coroutine ) {
        coroutine.resume();
    }
}

auto closeServerHandle( const std::shared_ptr<ListenerState> &state ) -> void {
    if ( !state->_initialized || uv_is_closing( reinterpret_cast<uv_handle_t *>( &state->_handle ) ) ) {
        return;
    }

    uv_close( reinterpret_cast<uv_handle_t *>( &state->_handle ), []( uv_handle_t *handle ) -> void {
        auto *lifetime = static_cast<std::shared_ptr<ListenerState> *>( handle->data );
        auto state = *lifetime;
        delete lifetime;
        handle->data = nullptr;
        state->_initialized = false;
    } );
}

auto closeServer( const std::shared_ptr<ListenerState> &state ) -> void {
    if ( state->_closed ) {
        return;
    }

    state->_closed = true;
    closeServerHandle( state );
    auto *listenOperation = std::exchange( state->_listenOperation, nullptr );
    auto *acceptOperation = std::exchange( state->_acceptOperation, nullptr );

    std::coroutine_handle<> listenCoroutine{};
    if ( listenOperation != nullptr ) {
        listenCoroutine = listenOperation->complete( UV_ECANCELED );
    }

    std::coroutine_handle<> acceptCoroutine{};
    if ( acceptOperation != nullptr ) {
        acceptCoroutine = acceptOperation->complete( UV_ECANCELED );
    }

    if ( listenCoroutine ) {
        listenCoroutine.resume();
    }
    if ( acceptCoroutine ) {
        acceptCoroutine.resume();
    }
}

auto connectionCallback( uv_stream_t *stream, int status ) -> void {
    auto *lifetime = static_cast<std::shared_ptr<ListenerState> *>( stream->data );
    auto state = *lifetime;
    auto operation = state->_acceptOperation;

    if ( status != 0 ) {
        if ( operation != nullptr ) {
            completeAccept( operation, status );
        }
        return;
    }

    auto createStatus = 0;
    auto connection = state->_scheduler->initTcp( createStatus );
    if ( createStatus != 0 ) {
        if ( operation != nullptr ) {
            completeAccept( operation, createStatus );
        }
        return;
    }

    const auto acceptStatus = uv_accept( stream, reinterpret_cast<uv_stream_t *>( connection->nativeHandle() ) );
    if ( operation == nullptr || operation->_completed || acceptStatus != 0 ) {
        state->_scheduler->close( connection );
    }

    if ( operation == nullptr || operation->_completed ) {
        return;
    }

    if ( acceptStatus != 0 ) {
        completeAccept( operation, acceptStatus );
        return;
    }

    operation->_result._connection = connection;
    completeAccept( operation, 0 );
}

auto startListen( ListenAwaitable *operation, uv_loop_t *loop ) -> void {
    auto state = operation->_server;

    if ( state->_closed ) {
        completeListen( operation, UV_ECANCELED );
        return;
    }

    if ( state->_initialized || state->_listenOperation != nullptr ) {
        completeListen( operation, UV_EALREADY );
        return;
    }

    state->_listenOperation = operation;
    auto status = uv_tcp_init( loop, &state->_handle );

    if ( status != 0 ) {
        completeListen( operation, status );
        return;
    }

    state->_initialized = true;
    state->_lifetime = new std::shared_ptr<ListenerState>{ state };
    state->_handle.data = state->_lifetime;

    sockaddr_in address{};
    status = uv_ip4_addr( operation->_host.c_str(), static_cast<int>( operation->_port ), &address );

    if ( status == 0 ) {
        status = uv_tcp_bind( &state->_handle, reinterpret_cast<const sockaddr *>( &address ), 0 );
    }

    if ( status == 0 ) {
        status = uv_listen( reinterpret_cast<uv_stream_t *>( &state->_handle ), 1, connectionCallback );
    }

    if ( status != 0 ) {
        state->_closed = true;
        closeServerHandle( state );
        completeListen( operation, status );

        return;
    }

    completeListen( operation, 0 );
}

auto startAccept( AcceptAwaitable *operation ) -> void {
    auto state = operation->_server;

    if ( state->_closed || !state->_initialized ) {
        completeAccept( operation, UV_ECANCELED );
        return;
    }

    if ( state->_acceptOperation != nullptr ) {
        completeAccept( operation, UV_EALREADY );
        return;
    }

    state->_acceptOperation = operation;
}

}  // namespace

namespace tire {

TcpListener::TcpListener( std::shared_ptr<TcpListenerState> state )
    : _state{ std::move( state ) } {
}

auto TcpListener::nativeHandle() const -> uv_tcp_t * {
    return &_state->_handle;
}

TcpListenAwaitable::TcpListenAwaitable( Scheduler &scheduler, std::string host, uint16_t port )
    : _scheduler{ &scheduler }
    , _server{ std::make_shared<ListenerState>() }
    , _host{ std::move( host ) }
    , _port{ port } {
    _server->_scheduler = &scheduler;
    _listener = std::make_shared<TcpListener>( _server );
}

auto TcpListenAwaitable::await_ready() noexcept -> bool {
    return false;
}

auto TcpListenAwaitable::await_suspend( std::coroutine_handle<> coroutine ) -> void {
    _coroutine = coroutine;

    _scheduler->schedule( [this]( uv_loop_t *loop ) -> void {
        //
        startListen( this, loop );
    } );
}

auto TcpListenAwaitable::await_resume() noexcept -> TcpListenResult {
    return _result;
}

auto TcpListenAwaitable::listener() const -> std::shared_ptr<TcpListener> {
    return _listener;
}

TcpAcceptAwaitable::TcpAcceptAwaitable( const std::shared_ptr<TcpListenerState> &state )
    : _server{ state } {
}

auto TcpAcceptAwaitable::await_ready() noexcept -> bool {
    return false;
}

auto TcpAcceptAwaitable::await_suspend( std::coroutine_handle<> coroutine ) -> void {
    _coroutine = coroutine;

    _server->_scheduler->schedule( [this]( uv_loop_t * ) -> void {
        //
        startAccept( this );
    } );
}

auto TcpAcceptAwaitable::await_resume() noexcept -> TcpAcceptResult {
    return std::move( _result );
}

auto Scheduler::listen( std::string host, uint16_t port ) -> TcpListenAwaitable {
    //
    return TcpListenAwaitable{ *this, std::move( host ), port };
}

auto Scheduler::accept( const std::shared_ptr<TcpListener> &listener ) -> TcpAcceptAwaitable {
    //
    return TcpAcceptAwaitable{ listener->_state };
}

auto Scheduler::close( const std::shared_ptr<TcpListener> &listener ) -> void {
    listener->_state->_scheduler->schedule( [state = listener->_state]( uv_loop_t * ) -> void {
        //
        closeServer( state );
    } );
}

}  // namespace tire
