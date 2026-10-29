#include "io/tcpclient.h"

#include <utility>

#include "io/scheduler.h"

namespace tire {

struct TcpConnectOperation {
    std::shared_ptr<TcpClient::State> _state{};
    std::shared_ptr<TcpClient::ConnectResult> _result{};
    std::shared_ptr<TcpConnection> _connection{};
    std::coroutine_handle<> _coroutine{};
    std::string _host{};
    uint16_t _port{ 0 };
    uv_connect_t _request{};
    bool _completed{ false };
};

struct TcpClient::State : std::enable_shared_from_this<TcpClient::State> {
    Scheduler *_scheduler{ nullptr };
    std::shared_ptr<TcpConnection> _connection{};
    std::shared_ptr<TcpConnectOperation> _operation{};
    bool _closed{ false };
};

namespace {

auto completeConnect( const std::shared_ptr<TcpConnectOperation> &operation, int status ) -> void {
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

auto failConnect( const std::shared_ptr<TcpConnectOperation> &operation, int status ) -> void {
    auto state = operation->_state;
    if ( state->_operation == operation ) {
        state->_operation.reset();
    }
    if ( state->_connection == operation->_connection ) {
        state->_connection.reset();
    }
    if ( operation->_connection != nullptr ) {
        operation->_connection->close();
    }
    operation->_result->_connection.reset();
    completeConnect( operation, status );
}

auto connectCallback( uv_connect_t *request, int status ) -> void {
    auto *lifetime = static_cast<std::shared_ptr<TcpConnectOperation> *>( request->data );
    auto operation = *lifetime;
    delete lifetime;
    request->data = nullptr;

    auto state = operation->_state;
    if ( state->_operation == operation ) {
        state->_operation.reset();
    }

    if ( status != 0 ) {
        failConnect( operation, status );
        return;
    }

    operation->_result->_connection = operation->_connection;
    completeConnect( operation, status );
}

auto startConnect( const std::shared_ptr<TcpConnectOperation> &operation, uv_loop_t *loop ) -> void {
    auto state = operation->_state;
    if ( state->_closed ) {
        completeConnect( operation, UV_ECANCELED );
        return;
    }
    if ( state->_operation != nullptr || state->_connection != nullptr ) {
        completeConnect( operation, UV_EALREADY );
        return;
    }

    state->_operation = operation;
    auto status = 0;
    auto connection = TcpConnection::create( *state->_scheduler, loop, status );
    if ( status != 0 ) {
        state->_operation.reset();
        completeConnect( operation, status );
        return;
    }

    operation->_connection = connection;
    state->_connection = connection;

    sockaddr_in address{};
    status = uv_ip4_addr( operation->_host.c_str(), static_cast<int>( operation->_port ), &address );
    if ( status != 0 ) {
        failConnect( operation, status );
        return;
    }

    operation->_request.data = new std::shared_ptr<TcpConnectOperation>{ operation };
    status = uv_tcp_connect( &operation->_request, connection->nativeHandle(),
                             reinterpret_cast<const sockaddr *>( &address ), connectCallback );
    if ( status != 0 ) {
        auto *lifetime = static_cast<std::shared_ptr<TcpConnectOperation> *>( operation->_request.data );
        delete lifetime;
        operation->_request.data = nullptr;
        failConnect( operation, status );
    }
}

}

TcpClient::TcpClient( Scheduler &scheduler )
    : _state{ std::make_shared<State>() } {
    _state->_scheduler = &scheduler;
}

auto TcpClient::ConnectAwaitable::await_ready() noexcept -> bool {
    return false;
}

auto TcpClient::ConnectAwaitable::await_suspend( std::coroutine_handle<> coroutine ) -> void {
    auto operation = std::make_shared<TcpConnectOperation>();
    operation->_state = _state;
    operation->_result = _result;
    operation->_coroutine = coroutine;
    operation->_host = std::move( _host );
    operation->_port = _port;
    _state->_scheduler->post( [operation = std::move( operation )]( uv_loop_t *loop ) mutable -> void {
        startConnect( operation, loop );
    } );
}

auto TcpClient::ConnectAwaitable::await_resume() noexcept -> ConnectResult {
    return std::move( *_result );
}

auto TcpClient::connect( std::string host, uint16_t port ) -> ConnectAwaitable {
    return ConnectAwaitable{ _state, std::move( host ), port };
}

auto TcpClient::close() -> void {
    _state->_scheduler->post( [state = _state]( uv_loop_t * ) -> void {
        if ( state->_closed ) {
            return;
        }

        state->_closed = true;
        auto operation = std::exchange( state->_operation, nullptr );
        auto connection = std::exchange( state->_connection, nullptr );
        if ( connection != nullptr ) {
            connection->close();
        }
        completeConnect( operation, UV_ECANCELED );
    } );
}

}
