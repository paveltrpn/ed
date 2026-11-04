#pragma once

#include <cstdint>
#include <coroutine>
#include <memory>
#include <utility>

#include <uv.h>

#include "io/scheduler.h"

namespace tire {

// ======================================================================================
// ==================== TimeoutAwaitableState ===========================================
// ======================================================================================

struct TimeoutAwaitableState : std::enable_shared_from_this<TimeoutAwaitableState> {
    TimeoutAwaitableState( Scheduler& scheduler, uint64_t timeout );

    auto finish( int status ) -> void;
    auto close( int status ) -> void;
    auto fire() -> void;
    auto cancel() -> void;

public:
    Scheduler* _scheduler{ nullptr };
    uv_timer_t _timer{};

    std::shared_ptr<TimeoutAwaitableState>* _lifetime{ nullptr };
    std::coroutine_handle<> _coroutine{};
    std::shared_ptr<int> _status{ std::make_shared<int>( 0 ) };

    uint64_t _timeout{ 0 };

    bool _initialized{ false };
    bool _closing{ false };
    bool _completed{ false };
};

// ======================================================================================
// ==================== TimeoutAwaitable ================================================
// ======================================================================================

struct TimeoutAwaitable final {
    explicit TimeoutAwaitable( std::shared_ptr<TimeoutAwaitableState> state );

    [[nodiscard]] auto await_ready() const noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) noexcept -> void;
    auto await_resume() const noexcept -> int;

private:
    std::shared_ptr<TimeoutAwaitableState> _state{};
};

}  // namespace tire
