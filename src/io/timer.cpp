#include "io/timer.h"

namespace tire {

TimeoutAwaitable::TimeoutAwaitable( Scheduler& scheduler, uint64_t timeout )
    : _scheduler{ &scheduler }
    , _timeout{ timeout } {
}

auto TimeoutAwaitable::close( int status ) -> void {
    if ( _closing || _completed ) {
        return;
    }

    _closing = true;
    _result._status = status;
    uv_timer_stop( &_timer );
    uv_close( reinterpret_cast<uv_handle_t*>( &_timer ), []( uv_handle_t* handle ) -> void {
        auto* awaitable = static_cast<TimeoutAwaitable*>( handle->data );
        handle->data = nullptr;
        awaitable->_initialized = false;

        const auto coroutine = awaitable->complete( awaitable->_result._status );
        if ( coroutine ) {
            coroutine.resume();
        }
    } );
}

auto TimeoutAwaitable::await_ready() const noexcept -> bool {
    //
    return false;
}

auto TimeoutAwaitable::await_suspend( std::coroutine_handle<> coroutine ) noexcept -> void {
    _coroutine = coroutine;

    _scheduler->schedule( [this]( uv_loop_t* loop ) -> void {
        //
        const auto initStatus = uv_timer_init( loop, &_timer );
        if ( initStatus != 0 ) {
            const auto coroutine = complete( initStatus );
            if ( coroutine ) {
                coroutine.resume();
            }
            return;
        }

        _initialized = true;
        _timer.data = this;

        const auto startStatus = uv_timer_start(
            &_timer,
            []( uv_timer_t* timer ) -> void {
                //
                auto* awaitable = static_cast<TimeoutAwaitable*>( timer->data );
                awaitable->close( 0 );
            },
            _timeout, 0 );
        if ( startStatus != 0 ) {
            close( startStatus );
        }
    } );
}

auto TimeoutAwaitable::await_resume() const noexcept -> int {
    //
    return _result._status;
}

auto Scheduler::timeout( uint64_t timeout ) -> TimeoutAwaitable {
    //
    return TimeoutAwaitable{ *this, timeout };
};

}  // namespace tire
