#pragma once

#include <cstdint>
#include <coroutine>
#include <memory>
#include <utility>

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
// ==================== TimeoutAwaitableState ===========================================
// ======================================================================================

struct TimeoutAwaitableState : AwaitableState<TimeoutAwaitableState, TimeoutResult> {
    TimeoutAwaitableState( Scheduler& scheduler, uint64_t timeout );

    auto close( int status ) -> void;
    auto fire() -> void;
    auto cancel() -> void;

public:
    Scheduler* _scheduler{ nullptr };
    uv_timer_t _timer{};

    std::shared_ptr<TimeoutAwaitableState>* _lifetime{ nullptr };

    uint64_t _timeout{ 0 };

    bool _initialized{ false };
    bool _closing{ false };
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
