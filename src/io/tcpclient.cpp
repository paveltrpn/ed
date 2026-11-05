#include "io/tcpclient.h"

#include <utility>

#include "io/scheduler.h"

namespace {

using ConnectAwaitable = tire::TcpConnectAwaitable;

auto completeConnect( ConnectAwaitable *operation, int status ) -> void {
    const auto coroutine = operation->complete( status );
    if ( coroutine ) {
        coroutine.resume();
    }
}

auto failConnect( ConnectAwaitable *operation, int status ) -> void {
    if ( operation->_connection != nullptr ) {
        operation->_scheduler->close( operation->_connection );
        operation->_connection.reset();
    }
    operation->_result._connection.reset();
    completeConnect( operation, status );
}

auto connectCallback( uv_connect_t *request, int status ) -> void {
    auto *operation = static_cast<ConnectAwaitable *>( request->data );
    request->data = nullptr;

    if ( status != 0 ) {
        failConnect( operation, status );
        return;
    }

    operation->_result._connection = std::move( operation->_connection );
    completeConnect( operation, status );
}

auto startConnect( ConnectAwaitable *operation ) -> void {
    auto status = 0;
    operation->_connection = operation->_scheduler->initTcp( status );
    if ( status != 0 ) {
        failConnect( operation, status );
        return;
    }

    sockaddr_in address{};
    status = uv_ip4_addr( operation->_host.c_str(), static_cast<int>( operation->_port ), &address );

    if ( status != 0 ) {
        failConnect( operation, status );
        return;
    }

    operation->_request.data = operation;
    status = uv_tcp_connect( &operation->_request, operation->_connection->nativeHandle(),
                              reinterpret_cast<const sockaddr *>( &address ), connectCallback );
    if ( status != 0 ) {
        operation->_request.data = nullptr;
        failConnect( operation, status );
    }
}

}  // namespace

namespace tire {

TcpConnectAwaitable::TcpConnectAwaitable( Scheduler &scheduler, std::string host, uint16_t port )
    : _scheduler{ &scheduler }
    , _host{ std::move( host ) }
    , _port{ port } {
}

auto TcpConnectAwaitable::await_ready() noexcept -> bool {
    return false;
}

auto TcpConnectAwaitable::await_suspend( std::coroutine_handle<> coroutine ) -> void {
    _coroutine = coroutine;

    _scheduler->schedule( [this]( uv_loop_t * ) -> void {
        //
        startConnect( this );
    } );
}

auto TcpConnectAwaitable::await_resume() noexcept -> TcpConnectResult {
    return std::move( _result );
}

auto Scheduler::connect( std::string host, uint16_t port ) -> TcpConnectAwaitable {
    //
    return TcpConnectAwaitable{ *this, std::move( host ), port };
}

}  // namespace tire
