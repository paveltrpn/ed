#pragma once

#include <cstdint>
#include <coroutine>
#include <memory>
#include <utility>

#include <uv.h>

#include "io/async.h"
#include "io/scheduler.h"

namespace tire {

template <typename T>
struct TimeoutAwaitable final {
    struct State {
        Scheduler *_scheduler{ nullptr };
        uv_timer_t _timer{};
        std::shared_ptr<State> *_lifetime{ nullptr };
        std::coroutine_handle<> _coroutine{};
        std::shared_ptr<int> _status{ std::make_shared<int>( 0 ) };
        uint64_t _timeout{ 0 };
        bool _initialized{ false };
        bool _closing{ false };
        bool _completed{ false };

        auto finish( int status ) -> void {
            if ( _completed ) {
                return;
            }

            _completed = true;
            *_status = status;
            const auto coroutine = std::exchange( _coroutine, nullptr );
            if ( coroutine ) {
                coroutine.resume();
            }
        }

        auto close( int status ) -> void {
            if ( _closing ) {
                return;
            }

            _closing = true;
            if ( _initialized && !uv_is_closing( reinterpret_cast<uv_handle_t *>( &_timer ) ) ) {
                uv_timer_stop( &_timer );
                uv_close( reinterpret_cast<uv_handle_t *>( &_timer ), []( uv_handle_t *handle ) -> void {
                    auto *lifetime = static_cast<std::shared_ptr<State> *>( handle->data );
                    auto state = *lifetime;
                    delete lifetime;
                    handle->data = nullptr;
                    state->_initialized = false;
                } );
            }
            finish( status );
        }

        auto fire() -> void {
            if ( _closing || _completed ) {
                return;
            }

            _closing = true;
            if ( _initialized && !uv_is_closing( reinterpret_cast<uv_handle_t *>( &_timer ) ) ) {
                uv_close( reinterpret_cast<uv_handle_t *>( &_timer ), []( uv_handle_t *handle ) -> void {
                    auto *lifetime = static_cast<std::shared_ptr<State> *>( handle->data );
                    auto state = *lifetime;
                    delete lifetime;
                    handle->data = nullptr;
                    state->_initialized = false;
                } );
            }
            finish( 0 );
        }
    };

    TimeoutAwaitable( Scheduler &context, uint64_t timeout )
        : _state{ std::make_shared<State>() } {
        _state->_scheduler = &context;
        _state->_timeout = timeout;
    }

    [[nodiscard]] auto await_ready() const noexcept -> bool {
        return false;
    }

    auto await_suspend( std::coroutine_handle<> coroutine ) noexcept -> void {
        _state->_coroutine = coroutine;
        _state->_scheduler->post( [state = _state]( uv_loop_t *loop ) -> void {
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
            state->_lifetime = new std::shared_ptr<State>{ state };
            state->_timer.data = state->_lifetime;
            const auto startStatus = uv_timer_start(
                &state->_timer,
                []( uv_timer_t *timer ) -> void {
                    auto *lifetime = static_cast<std::shared_ptr<State> *>( timer->data );
                    auto state = *lifetime;
                    state->fire();
                },
                state->_timeout, 0 );
            if ( startStatus != 0 ) {
                state->close( startStatus );
            }
        } );
    }

    auto await_resume() const noexcept -> int {
        return *_state->_status;
    }

    auto cancel() -> void {
        _state->_scheduler->post( [state = _state]( uv_loop_t * ) -> void { state->close( UV_ECANCELED ); } );
    }

    std::shared_ptr<State> _state{};
};

}  // namespace tire
