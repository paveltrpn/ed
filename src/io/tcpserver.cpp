#include "io/tcpserver.h"

#include <utility>

#include "io/scheduler.h"

namespace tire {

struct TcpListenOperation {
    std::shared_ptr<TcpServer::State> _state{};
    std::shared_ptr<TcpServer::ListenResult> _result{};
    std::coroutine_handle<> _coroutine{};
    std::string _host{};
    uint16_t _port{ 0 };
    bool _completed{ false };
};

struct TcpAcceptOperation {
    std::shared_ptr<TcpServer::State> _state{};
    std::shared_ptr<TcpServer::AcceptResult> _result{};
    std::coroutine_handle<> _coroutine{};
    bool _completed{ false };
};

struct TcpServer::State : std::enable_shared_from_this<TcpServer::State> {
    Scheduler *_scheduler{ nullptr };
    uv_tcp_t _handle{};
    std::shared_ptr<State> *_lifetime{ nullptr };
    std::shared_ptr<TcpListenOperation> _listenOperation{};
    std::shared_ptr<TcpAcceptOperation> _acceptOperation{};
    bool _initialized{ false };
    bool _closed{ false };
};

namespace {

using ServerState = TcpServer::State;

auto completeListen( const std::shared_ptr<TcpListenOperation> &operation, int status ) -> void {
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

auto completeAccept( const std::shared_ptr<TcpAcceptOperation> &operation, int status ) -> void {
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

auto closeServerHandle( const std::shared_ptr<ServerState> &state ) -> void {
    if ( !state->_initialized || uv_is_closing( reinterpret_cast<uv_handle_t *>( &state->_handle ) ) ) {
        return;
    }

    uv_close( reinterpret_cast<uv_handle_t *>( &state->_handle ), []( uv_handle_t *handle ) -> void {
        auto *lifetime = static_cast<std::shared_ptr<ServerState> *>( handle->data );
        auto state = *lifetime;
        delete lifetime;
        handle->data = nullptr;
        state->_initialized = false;
    } );
}

auto closeServer( const std::shared_ptr<ServerState> &state ) -> void {
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
    auto *lifetime = static_cast<std::shared_ptr<ServerState> *>( stream->data );
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
    auto connection = TcpConnection::create( *state->_scheduler, stream->loop, createStatus );
    if ( createStatus != 0 ) {
        if ( operation != nullptr ) {
            state->_acceptOperation.reset();
            completeAccept( operation, createStatus );
        }
        return;
    }

    const auto acceptStatus = uv_accept( stream, reinterpret_cast<uv_stream_t *>( connection->nativeHandle() ) );
    if ( operation == nullptr || operation->_completed || acceptStatus != 0 ) {
        connection->close();
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

auto startListen( const std::shared_ptr<TcpListenOperation> &operation, uv_loop_t *loop ) -> void {
    auto state = operation->_state;
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
    state->_lifetime = new std::shared_ptr<ServerState>{ state };
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

auto startAccept( const std::shared_ptr<TcpAcceptOperation> &operation ) -> void {
    auto state = operation->_state;
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

}

TcpServer::TcpServer( Scheduler &scheduler )
    : _state{ std::make_shared<State>() } {
    _state->_scheduler = &scheduler;
}

auto TcpServer::ListenAwaitable::await_ready() noexcept -> bool {
    return false;
}

auto TcpServer::ListenAwaitable::await_suspend( std::coroutine_handle<> coroutine ) -> void {
    auto operation = std::make_shared<TcpListenOperation>();
    operation->_state = _state;
    operation->_result = _result;
    operation->_coroutine = coroutine;
    operation->_host = std::move( _host );
    operation->_port = _port;
    _state->_scheduler->post( [operation = std::move( operation )]( uv_loop_t *loop ) mutable -> void {
        startListen( operation, loop );
    } );
}

auto TcpServer::ListenAwaitable::await_resume() noexcept -> ListenResult {
    return *_result;
}

auto TcpServer::AcceptAwaitable::await_ready() noexcept -> bool {
    return false;
}

auto TcpServer::AcceptAwaitable::await_suspend( std::coroutine_handle<> coroutine ) -> void {
    auto operation = std::make_shared<TcpAcceptOperation>();
    operation->_state = _state;
    operation->_result = _result;
    operation->_coroutine = coroutine;
    _state->_scheduler->post( [operation = std::move( operation )]( uv_loop_t * ) mutable -> void {
        startAccept( operation );
    } );
}

auto TcpServer::AcceptAwaitable::await_resume() noexcept -> AcceptResult {
    return std::move( *_result );
}

auto TcpServer::listen( std::string host, uint16_t port ) -> ListenAwaitable {
    return ListenAwaitable{ _state, std::move( host ), port };
}

auto TcpServer::accept() -> AcceptAwaitable {
    return AcceptAwaitable{ _state };
}

auto TcpServer::close() -> void {
    _state->_scheduler->post( [state = _state]( uv_loop_t * ) -> void { closeServer( state ); } );
}

}
