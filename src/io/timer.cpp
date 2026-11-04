#include "io/timer.h"

namespace tire {

TimeoutAwaitableState::TimeoutAwaitableState( Scheduler& scheduler, uint64_t timeout )
    : _scheduler{ &scheduler }
    , _timeout{ timeout } {
}

auto TimeoutAwaitableState::close( int status ) -> void {
    if ( _closing ) {
        return;
    }

    _closing = true;
    if ( _initialized && !uv_is_closing( reinterpret_cast<uv_handle_t*>( &_timer ) ) ) {
        uv_timer_stop( &_timer );
        uv_close( reinterpret_cast<uv_handle_t*>( &_timer ), []( uv_handle_t* handle ) -> void {
            auto* lifetime = static_cast<std::shared_ptr<TimeoutAwaitableState>*>( handle->data );
            auto state = *lifetime;
            delete lifetime;
            handle->data = nullptr;
            state->_initialized = false;
        } );
    }
    finish( status );
}

auto TimeoutAwaitableState::fire() -> void {
    if ( _closing || _completed ) {
        return;
    }

    _closing = true;
    if ( _initialized && !uv_is_closing( reinterpret_cast<uv_handle_t*>( &_timer ) ) ) {
        uv_close( reinterpret_cast<uv_handle_t*>( &_timer ), []( uv_handle_t* handle ) -> void {
            auto* lifetime = static_cast<std::shared_ptr<TimeoutAwaitableState>*>( handle->data );
            auto state = *lifetime;
            delete lifetime;
            handle->data = nullptr;
            state->_initialized = false;
        } );
    }

    finish( 0 );
}

auto TimeoutAwaitableState::cancel() -> void {
    _scheduler->schedule( [state = shared_from_this()]( uv_loop_t* ) -> void {
        //
        state->close( UV_ECANCELED );
    } );
}

TimeoutAwaitable::TimeoutAwaitable( std::shared_ptr<TimeoutAwaitableState> state )
    : _state{ std::move( state ) } {
}

auto TimeoutAwaitable::await_ready() const noexcept -> bool {
    //
    return false;
}

auto TimeoutAwaitable::await_suspend( std::coroutine_handle<> coroutine ) noexcept -> void {
    _state->_coroutine = coroutine;

    _state->_scheduler->schedule( [state = _state]( uv_loop_t* loop ) -> void {
        //
        if ( state->_closing ) {
            state->finish( UV_ECANCELED );
            return;
        }

        const auto initStatus = uv_timer_init( loop, &state->_timer );
        if ( initStatus != 0 ) {
            state->finish( initStatus );
            return;
        }

        state->_initialized = true;
        state->_lifetime = new std::shared_ptr<TimeoutAwaitableState>{ state };
        state->_timer.data = state->_lifetime;

        const auto startStatus = uv_timer_start(
            &state->_timer,
            []( uv_timer_t* timer ) -> void {
                //
                auto* lifetime = static_cast<std::shared_ptr<TimeoutAwaitableState>*>( timer->data );
                auto state = *lifetime;
                state->fire();
            },
            state->_timeout, 0 );
        if ( startStatus != 0 ) {
            state->close( startStatus );
        }
    } );
}

auto TimeoutAwaitable::await_resume() const noexcept -> int {
    //
    return _state->_result->_status;
}

auto Scheduler::timeout( uint64_t timeout ) -> TimeoutAwaitable {
    //
    return TimeoutAwaitable{ std::make_shared<TimeoutAwaitableState>( *this, timeout ) };
};

}  // namespace tire
