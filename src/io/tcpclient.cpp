#include "io/tcpclient.h"

#include <utility>

#include "io/scheduler.h"

namespace {

using ConnectState = tire::TcpConnectAwaitableState;

auto completeConnect( const std::shared_ptr<ConnectState> &state, int status ) -> void {
    if ( state->_completed ) {
        return;
    }

    state->_completed = true;
    state->_result->_status = status;
    const auto coroutine = std::exchange( state->_coroutine, nullptr );
    if ( coroutine ) {
        coroutine.resume();
    }
}

auto failConnect( const std::shared_ptr<ConnectState> &state, int status ) -> void {
    if ( state->_connection != nullptr ) {
        state->_scheduler->close( state->_connection );
        state->_connection.reset();
    }
    state->_result->_connection.reset();
    completeConnect( state, status );
}

auto connectCallback( uv_connect_t *request, int status ) -> void {
    auto *lifetime = static_cast<std::shared_ptr<ConnectState> *>( request->data );
    auto state = *lifetime;
    delete lifetime;
    request->data = nullptr;

    if ( status != 0 ) {
        failConnect( state, status );
        return;
    }

    state->_result->_connection = state->_connection;
    completeConnect( state, status );
}

auto cancelConnect( const std::shared_ptr<ConnectState> &state ) -> void {
    if ( state->_completed ) {
        return;
    }

    if ( state->_connection != nullptr ) {
        state->_scheduler->close( state->_connection );
        state->_connection.reset();
    }
    state->_result->_connection.reset();
    completeConnect( state, UV_ECANCELED );
}

auto startConnect( const std::shared_ptr<ConnectState> &state ) -> void {
    if ( state->_completed ) {
        return;
    }

    auto status = 0;
    auto connection = state->_scheduler->initTcp( status );
    if ( status != 0 ) {
        completeConnect( state, status );
        return;
    }

    state->_connection = connection;

    sockaddr_in address{};
    status = uv_ip4_addr( state->_host.c_str(), static_cast<int>( state->_port ), &address );

    if ( status != 0 ) {
        failConnect( state, status );
        return;
    }

    state->_request.data = new std::shared_ptr<ConnectState>{ state };
    status = uv_tcp_connect( &state->_request, connection->nativeHandle(),
                             reinterpret_cast<const sockaddr *>( &address ), connectCallback );
    if ( status != 0 ) {
        auto *lifetime = static_cast<std::shared_ptr<ConnectState> *>( state->_request.data );
        delete lifetime;
        state->_request.data = nullptr;
        failConnect( state, status );
    }
}

}  // namespace

namespace tire {

TcpConnectAwaitableState::TcpConnectAwaitableState( Scheduler &scheduler, std::string host, uint16_t port )
    : _scheduler{ &scheduler }
    , _host{ std::move( host ) }
    , _port{ port } {
}

auto TcpConnectAwaitableState::cancel() -> void {
    _scheduler->schedule( [state = shared_from_this()]( uv_loop_t * ) -> void {
        //
        cancelConnect( state );
    } );
}

TcpConnectAwaitable::TcpConnectAwaitable( std::shared_ptr<TcpConnectAwaitableState> state )
    : _state{ std::move( state ) } {
}

auto TcpConnectAwaitable::await_ready() noexcept -> bool {
    return false;
}

auto TcpConnectAwaitable::await_suspend( std::coroutine_handle<> coroutine ) -> void {
    _state->_coroutine = coroutine;

    _state->_scheduler->schedule( [state = _state]( uv_loop_t * ) -> void {
        //
        startConnect( state );
    } );
}

auto TcpConnectAwaitable::await_resume() noexcept -> TcpConnectResult {
    return std::move( *_state->_result );
}

auto Scheduler::connect( std::string host, uint16_t port ) -> TcpConnectAwaitable {
    //
    return TcpConnectAwaitable{ std::make_shared<TcpConnectAwaitableState>( *this, std::move( host ), port ) };
}

}  // namespace tire
