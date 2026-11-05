#pragma once

#include <cstdint>
#include <coroutine>

#include <uv.h>

#include "io/awaitablestate.h"
#include "io/scheduler.h"

namespace tire {

// ======================================================================================
// ==================== TimeoutResult ===================================================
// ======================================================================================

struct TimeoutResult {
    int _status{ 0 };
};

// ======================================================================================
// ==================== TimeoutAwaitable ================================================
// ======================================================================================

struct TimeoutAwaitable final : AwaitableState<TimeoutResult> {
    TimeoutAwaitable( Scheduler& scheduler, uint64_t timeout );

    TimeoutAwaitable( const TimeoutAwaitable& other ) = delete;
    TimeoutAwaitable( TimeoutAwaitable&& other ) = delete;

    auto operator=( const TimeoutAwaitable& other ) -> TimeoutAwaitable& = delete;
    auto operator=( TimeoutAwaitable&& other ) -> TimeoutAwaitable& = delete;

    [[nodiscard]] auto await_ready() const noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) noexcept -> void;
    auto await_resume() const noexcept -> int;

private:
    auto close( int status ) -> void;

private:
    Scheduler* _scheduler{ nullptr };
    uv_timer_t _timer{};
    uint64_t _timeout{ 0 };

    bool _initialized{ false };
    bool _closing{ false };
};

}  // namespace tire
