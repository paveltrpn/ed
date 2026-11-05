#include "io/tcpconnection.h"

#include <cstdlib>
#include <cstring>
#include <limits>
#include <utility>

#include "io/scheduler.h"

namespace tire {

namespace {

using ConnectionState = TcpConnectionState;

auto completeRead( TcpReadAwaitable *operation, int status ) -> void {
    if ( operation == nullptr ) {
        return;
    }

    if ( operation->_allocation != nullptr ) {
        std::free( operation->_allocation );
        operation->_allocation = nullptr;
    }

    if ( operation->_state->_readOperation == operation ) {
        operation->_state->_readOperation = nullptr;
    }

    const auto coroutine = operation->complete( status );
    if ( coroutine ) {
        coroutine.resume();
    }
}

auto completeWrite( TcpWriteAwaitable *operation, int status ) -> void {
    if ( operation == nullptr ) {
        return;
    }

    if ( operation->_state->_writeOperation == operation ) {
        operation->_state->_writeOperation = nullptr;
    }

    const auto coroutine = operation->complete( status );
    if ( coroutine ) {
        coroutine.resume();
    }
}

auto closeConnection( const std::shared_ptr<ConnectionState> &state ) -> void {
    if ( state == nullptr || state->_closing ) {
        return;
    }

    state->_closing = true;

    auto *readOperation = std::exchange( state->_readOperation, nullptr );
    state->_writeOperation = nullptr;

    if ( state->_initialized && !uv_is_closing( reinterpret_cast<uv_handle_t *>( &state->_handle ) ) ) {
        uv_read_stop( reinterpret_cast<uv_stream_t *>( &state->_handle ) );
        uv_close( reinterpret_cast<uv_handle_t *>( &state->_handle ), []( uv_handle_t *handle ) -> void {
            auto *lifetime = static_cast<std::shared_ptr<ConnectionState> *>( handle->data );
            auto state = *lifetime;
            delete lifetime;
            handle->data = nullptr;
            state->_initialized = false;
        } );
    }

    completeRead( readOperation, UV_ECANCELED );
}

auto allocateReadBuffer( uv_handle_t *handle, size_t suggestedSize, uv_buf_t *buffer ) -> void {
    auto *lifetime = static_cast<std::shared_ptr<ConnectionState> *>( handle->data );
    auto state = *lifetime;
    auto *operation = state->_readOperation;

    buffer->base = nullptr;
    buffer->len = 0;

    if ( operation == nullptr || operation->_completed || suggestedSize == 0 ) {
        return;
    }

    buffer->base = static_cast<char *>( std::malloc( suggestedSize ) );
    if ( buffer->base == nullptr ) {
        return;
    }

    buffer->len = suggestedSize;
    operation->_allocation = buffer->base;
}

auto readCallback( uv_stream_t *stream, ssize_t nread, const uv_buf_t *buffer ) -> void {
    auto *lifetime = static_cast<std::shared_ptr<ConnectionState> *>( stream->data );
    auto state = *lifetime;
    auto *operation = state->_readOperation;

    if ( nread > 0 && operation != nullptr && !operation->_completed && buffer != nullptr && buffer->base != nullptr ) {
        const auto size = static_cast<size_t>( nread );
        operation->_result._bytes.resize( size );
        std::memcpy( operation->_result._bytes.data(), buffer->base, size );
    }

    if ( buffer != nullptr && buffer->base != nullptr ) {
        std::free( buffer->base );
    }

    if ( operation != nullptr ) {
        operation->_allocation = nullptr;
    }

    if ( nread == 0 || operation == nullptr || operation->_completed ) {
        return;
    }

    auto status = 0;
    if ( nread < 0 ) {
        operation->_result._bytes.clear();
        status = static_cast<int>( nread );
    }

    uv_read_stop( stream );
    completeRead( operation, status );
}

auto writeCallback( uv_write_t *request, int status ) -> void {
    auto *operation = static_cast<TcpWriteAwaitable *>( request->data );
    request->data = nullptr;

    if ( operation->_state->_closing ) {
        status = UV_ECANCELED;
    }

    completeWrite( operation, status );
}

auto startRead( TcpReadAwaitable *operation ) -> void {
    const auto state = operation->_state;

    if ( state->_closing || !state->_initialized ) {
        completeRead( operation, UV_ECANCELED );
        return;
    }

    if ( state->_readOperation != nullptr ) {
        completeRead( operation, UV_EALREADY );
        return;
    }

    state->_readOperation = operation;
    const auto status =
        uv_read_start( reinterpret_cast<uv_stream_t *>( &state->_handle ), allocateReadBuffer, readCallback );

    if ( status != 0 ) {
        completeRead( operation, status );
    }
}

auto startWrite( TcpWriteAwaitable *operation ) -> void {
    const auto state = operation->_state;

    if ( state->_closing || !state->_initialized ) {
        completeWrite( operation, UV_ECANCELED );
        return;
    }
    if ( state->_writeOperation != nullptr ) {
        completeWrite( operation, UV_EALREADY );
        return;
    }

    if ( operation->_bytes.size() > std::numeric_limits<unsigned int>::max() ) {
        completeWrite( operation, UV_EINVAL );
        return;
    }

    state->_writeOperation = operation;
    operation->_request.data = operation;
    auto buffer = uv_buf_init( reinterpret_cast<char *>( operation->_bytes.data() ),
                               static_cast<unsigned int>( operation->_bytes.size() ) );
    const auto status =
        uv_write( &operation->_request, reinterpret_cast<uv_stream_t *>( &state->_handle ), &buffer, 1, writeCallback );
    if ( status != 0 ) {
        operation->_request.data = nullptr;
        completeWrite( operation, status );
    }
}

}  // namespace

TcpConnection::TcpConnection( std::shared_ptr<TcpConnectionState> state )
    : _state{ std::move( state ) } {
}

TcpReadAwaitable::TcpReadAwaitable( std::shared_ptr<TcpConnectionState> state )
    : _state{ std::move( state ) } {
}

TcpWriteAwaitable::TcpWriteAwaitable( std::shared_ptr<TcpConnectionState> state, std::vector<std::byte> bytes )
    : _state{ std::move( state ) }
    , _bytes{ std::move( bytes ) } {
}

auto Scheduler::initTcp( int &status ) -> std::shared_ptr<TcpConnection> {
    auto state = std::make_shared<TcpConnectionState>();
    state->_scheduler = this;
    status = uv_tcp_init( _loop, &state->_handle );

    if ( status != 0 ) {
        return {};
    }

    state->_initialized = true;
    state->_lifetime = new std::shared_ptr<TcpConnectionState>{ state };
    state->_handle.data = state->_lifetime;

    return std::make_shared<TcpConnection>( std::move( state ) );
}

auto TcpReadAwaitable::await_ready() noexcept -> bool {
    return false;
}

auto TcpReadAwaitable::await_suspend( std::coroutine_handle<> coroutine ) -> void {
    _coroutine = coroutine;

    _state->_scheduler->schedule( [this]( uv_loop_t * ) -> void {
        //
        startRead( this );
    } );
}

auto TcpReadAwaitable::await_resume() noexcept -> TcpReadResult {
    return std::move( _result );
}

auto TcpWriteAwaitable::await_ready() noexcept -> bool {
    return false;
}

auto TcpWriteAwaitable::await_suspend( std::coroutine_handle<> coroutine ) -> void {
    _coroutine = coroutine;

    _state->_scheduler->schedule( [this]( uv_loop_t * ) -> void {
        startWrite( this );
    } );
}

auto TcpWriteAwaitable::await_resume() noexcept -> TcpWriteResult {
    return _result;
}

auto Scheduler::read( const std::shared_ptr<TcpConnection> &connection ) -> TcpReadAwaitable {
    //
    return TcpReadAwaitable{ connection->_state };
}

auto Scheduler::write( const std::shared_ptr<TcpConnection> &connection, std::vector<std::byte> bytes )
    -> TcpWriteAwaitable {
    //
    return TcpWriteAwaitable{ connection->_state, std::move( bytes ) };
}

auto Scheduler::close( const std::shared_ptr<TcpConnection> &connection ) -> void {
    connection->_state->_scheduler->schedule( [state = connection->_state]( uv_loop_t * ) -> void {
        //
        closeConnection( state );
    } );
}

auto TcpConnection::nativeHandle() const -> uv_tcp_t * {
    return &_state->_handle;
}

}  // namespace tire
