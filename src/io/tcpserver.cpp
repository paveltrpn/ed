#include "io/tcpserver.h"

#include <utility>

#include "io/scheduler.h"

namespace {

using ListenerState = tire::TcpListenerState;
using ListenState = tire::TcpListenAwaitableState;
using AcceptState = tire::TcpAcceptAwaitableState;

auto completeListen( const std::shared_ptr<ListenState> &operation, int status ) -> void {
    if ( operation == nullptr || operation->_completed ) {
        return;
    }

    operation->_completed = true;
    operation->_result->_status = status;
    const auto coroutine = std::exchange( operation->_coroutine, nullptr );
    if ( coroutine ) {
        coroutine.resume();
    }
}

auto completeAccept( const std::shared_ptr<AcceptState> &operation, int status ) -> void {
    if ( operation == nullptr || operation->_completed ) {
        return;
    }

    operation->_completed = true;
    operation->_result->_status = status;
    const auto coroutine = std::exchange( operation->_coroutine, nullptr );
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
    auto listenOperation = std::exchange( state->_listenOperation, nullptr );
    auto acceptOperation = std::exchange( state->_acceptOperation, nullptr );
    completeListen( listenOperation, UV_ECANCELED );
    completeAccept( acceptOperation, UV_ECANCELED );
}

auto connectionCallback( uv_stream_t *stream, int status ) -> void {
    auto *lifetime = static_cast<std::shared_ptr<ListenerState> *>( stream->data );
    auto state = *lifetime;
    auto operation = state->_acceptOperation;

    if ( status != 0 ) {
        if ( operation != nullptr ) {
            state->_acceptOperation.reset();
            completeAccept( operation, status );
        }
        return;
    }

    auto createStatus = 0;
    auto connection = state->_scheduler->initTcp( createStatus );
    if ( createStatus != 0 ) {
        if ( operation != nullptr ) {
            state->_acceptOperation.reset();
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

    state->_acceptOperation.reset();
    if ( acceptStatus != 0 ) {
        completeAccept( operation, acceptStatus );
        return;
    }

    operation->_result->_connection = connection;
    completeAccept( operation, 0 );
}

auto startListen( const std::shared_ptr<ListenState> &operation, uv_loop_t *loop ) -> void {
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
        state->_listenOperation.reset();
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

        state->_listenOperation.reset();
        completeListen( operation, status );

        return;
    }

    state->_listenOperation.reset();

    completeListen( operation, 0 );
}

auto startAccept( const std::shared_ptr<AcceptState> &operation ) -> void {
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
    : _state{ std::make_shared<TcpListenAwaitableState>() } {
    _state->_scheduler = &scheduler;
    _state->_server = std::make_shared<ListenerState>();
    _state->_server->_scheduler = &scheduler;
    _state->_listener = std::make_shared<TcpListener>( _state->_server );
    _state->_host = std::move( host );
    _state->_port = port;
}

auto TcpListenAwaitable::await_ready() noexcept -> bool {
    return false;
}

auto TcpListenAwaitable::await_suspend( std::coroutine_handle<> coroutine ) -> void {
    _state->_coroutine = coroutine;

    _state->_scheduler->schedule( [operation = _state]( uv_loop_t *loop ) -> void {
        //
        startListen( operation, loop );
    } );
}

auto TcpListenAwaitable::await_resume() noexcept -> TcpListenResult {
    return *_state->_result;
}

auto TcpListenAwaitable::listener() const -> std::shared_ptr<TcpListener> {
    return _state->_listener;
}

TcpAcceptAwaitable::TcpAcceptAwaitable( const std::shared_ptr<TcpListenerState> &state )
    : _state{ std::make_shared<TcpAcceptAwaitableState>() } {
    _state->_server = state;
}

auto TcpAcceptAwaitable::await_ready() noexcept -> bool {
    return false;
}

auto TcpAcceptAwaitable::await_suspend( std::coroutine_handle<> coroutine ) -> void {
    _state->_coroutine = coroutine;

    _state->_server->_scheduler->schedule( [operation = _state]( uv_loop_t * ) -> void {
        //
        startAccept( operation );
    } );
}

auto TcpAcceptAwaitable::await_resume() noexcept -> TcpAcceptResult {
    return std::move( *_state->_result );
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
