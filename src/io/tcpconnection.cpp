#include "io/tcpconnection.h"

#include <cstdlib>
#include <cstring>
#include <limits>
#include <utility>

#include "io/scheduler.h"

namespace tire {

struct TcpReadOperation {
    std::shared_ptr<TcpConnection::State> _state{};
    std::coroutine_handle<> _coroutine{};
    std::shared_ptr<TcpConnection::ReadResult> _result{};
    void *_allocation{ nullptr };
    bool _completed{ false };
};

struct TcpWriteOperation {
    std::shared_ptr<TcpConnection::State> _state{};
    std::coroutine_handle<> _coroutine{};
    TcpConnection::Bytes _bytes{};
    std::shared_ptr<TcpConnection::WriteResult> _result{};
    uv_write_t _request{};
    bool _completed{ false };
};

struct TcpConnection::State : std::enable_shared_from_this<TcpConnection::State> {
    Scheduler *_scheduler{ nullptr };
    uv_tcp_t _handle{};
    std::shared_ptr<State> *_lifetime{ nullptr };
    std::shared_ptr<TcpReadOperation> _readOperation{};
    std::shared_ptr<TcpWriteOperation> _writeOperation{};
    bool _initialized{ false };
    bool _closing{ false };
};

namespace {

using ConnectionState = TcpConnection::State;

auto completeRead( const std::shared_ptr<TcpReadOperation> &operation, int status ) -> void {
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

auto completeWrite( const std::shared_ptr<TcpWriteOperation> &operation, int status ) -> void {
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

auto closeConnection( const std::shared_ptr<ConnectionState> &state ) -> void {
    if ( state == nullptr || state->_closing ) {
        return;
    }

    state->_closing = true;

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

    auto readOperation = std::exchange( state->_readOperation, nullptr );
    auto writeOperation = std::exchange( state->_writeOperation, nullptr );
    completeRead( readOperation, UV_ECANCELED );
    completeWrite( writeOperation, UV_ECANCELED );
}

auto allocateReadBuffer( uv_handle_t *handle, size_t suggestedSize, uv_buf_t *buffer ) -> void {
    auto *lifetime = static_cast<std::shared_ptr<ConnectionState> *>( handle->data );
    auto state = *lifetime;
    const auto operation = state->_readOperation;

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
    auto operation = state->_readOperation;

    if ( nread > 0 && operation != nullptr && !operation->_completed && buffer != nullptr && buffer->base != nullptr ) {
        const auto size = static_cast<size_t>( nread );
        operation->_result->_bytes.resize( size );
        std::memcpy( operation->_result->_bytes.data(), buffer->base, size );
        operation->_result->_status = 0;
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

    if ( nread < 0 ) {
        operation->_result->_bytes.clear();
        operation->_result->_status = static_cast<int>( nread );
    }

    uv_read_stop( stream );
    state->_readOperation.reset();
    const auto status = operation->_result->_status;
    completeRead( operation, status );
}

auto writeCallback( uv_write_t *request, int status ) -> void {
    auto *lifetime = static_cast<std::shared_ptr<TcpWriteOperation> *>( request->data );
    auto operation = *lifetime;
    delete lifetime;
    request->data = nullptr;

    auto state = operation->_state;
    if ( state->_writeOperation == operation ) {
        state->_writeOperation.reset();
    }
    completeWrite( operation, status );
}

auto startRead( const std::shared_ptr<ConnectionState> &state,
                const std::shared_ptr<TcpReadOperation> &operation ) -> void {
    if ( state->_closing || !state->_initialized ) {
        completeRead( operation, UV_ECANCELED );
        return;
    }
    if ( state->_readOperation != nullptr ) {
        completeRead( operation, UV_EALREADY );
        return;
    }

    state->_readOperation = operation;
    const auto status = uv_read_start( reinterpret_cast<uv_stream_t *>( &state->_handle ), allocateReadBuffer, readCallback );
    if ( status != 0 ) {
        state->_readOperation.reset();
        completeRead( operation, status );
    }
}

auto startWrite( const std::shared_ptr<ConnectionState> &state,
                 const std::shared_ptr<TcpWriteOperation> &operation ) -> void {
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
    operation->_request.data = new std::shared_ptr<TcpWriteOperation>{ operation };
    auto buffer = uv_buf_init( reinterpret_cast<char *>( operation->_bytes.data() ),
                               static_cast<unsigned int>( operation->_bytes.size() ) );
    const auto status = uv_write( &operation->_request, reinterpret_cast<uv_stream_t *>( &state->_handle ), &buffer, 1,
                                  writeCallback );
    if ( status != 0 ) {
        auto *lifetime = static_cast<std::shared_ptr<TcpWriteOperation> *>( operation->_request.data );
        delete lifetime;
        operation->_request.data = nullptr;
        state->_writeOperation.reset();
        completeWrite( operation, status );
    }
}

}

TcpConnection::TcpConnection( std::shared_ptr<State> state )
    : _state{ std::move( state ) } {}

auto TcpConnection::create( Scheduler &scheduler, uv_loop_t *loop, int &status ) -> std::shared_ptr<TcpConnection> {
    auto state = std::make_shared<State>();
    state->_scheduler = &scheduler;
    status = uv_tcp_init( loop, &state->_handle );
    if ( status != 0 ) {
        return {};
    }

    state->_initialized = true;
    state->_lifetime = new std::shared_ptr<State>{ state };
    state->_handle.data = state->_lifetime;
    return std::make_shared<TcpConnection>( std::move( state ) );
}

auto TcpConnection::ReadAwaitable::await_ready() noexcept -> bool {
    return false;
}

auto TcpConnection::ReadAwaitable::await_suspend( std::coroutine_handle<> coroutine ) -> void {
    auto operation = std::make_shared<TcpReadOperation>();
    operation->_state = _state;
    operation->_coroutine = coroutine;
    operation->_result = _result;
    _state->_scheduler->post( [state = _state, operation = std::move( operation )]( uv_loop_t * ) mutable -> void {
        startRead( state, operation );
    } );
}

auto TcpConnection::ReadAwaitable::await_resume() noexcept -> ReadResult {
    return std::move( *_result );
}

auto TcpConnection::WriteAwaitable::await_ready() noexcept -> bool {
    return false;
}

auto TcpConnection::WriteAwaitable::await_suspend( std::coroutine_handle<> coroutine ) -> void {
    auto operation = std::make_shared<TcpWriteOperation>();
    operation->_state = _state;
    operation->_coroutine = coroutine;
    operation->_result = _result;
    operation->_bytes = std::move( _bytes );
    _state->_scheduler->post( [state = _state, operation = std::move( operation )]( uv_loop_t * ) mutable -> void {
        startWrite( state, operation );
    } );
}

auto TcpConnection::WriteAwaitable::await_resume() noexcept -> WriteResult {
    return *_result;
}

auto TcpConnection::read() -> ReadAwaitable {
    return ReadAwaitable{ _state };
}

auto TcpConnection::write( Bytes bytes ) -> WriteAwaitable {
    return WriteAwaitable{ _state, std::move( bytes ) };
}

auto TcpConnection::close() -> void {
    _state->_scheduler->post( [state = _state]( uv_loop_t * ) -> void { closeConnection( state ); } );
}

auto TcpConnection::nativeHandle() const -> uv_tcp_t * {
    return &_state->_handle;
}

}
