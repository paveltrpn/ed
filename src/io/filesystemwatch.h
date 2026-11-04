#pragma once

#include <coroutine>
#include <memory>
#include <string>

#include <uv.h>

#include "io/awaitablestate.h"
#include "io/scheduler.h"

namespace tire {

// ======================================================================================
// ==================== FileWatchResult =================================================
// ======================================================================================

struct FileWatchResult {
    int _status{ 0 };
    int _events{ 0 };
};

// ======================================================================================
// ==================== FileWatchAwaitableState =========================================
// ======================================================================================

struct FileWatchAwaitableState : AwaitableState<FileWatchAwaitableState, FileWatchResult> {
    FileWatchAwaitableState( Scheduler& scheduler, std::string path );

    auto close( int status ) -> void;
    auto cancel() -> void;

public:
    Scheduler* _scheduler{ nullptr };
    std::string _path{};
    uv_fs_event_t _watcher{};

    std::shared_ptr<FileWatchAwaitableState>* _lifetime{ nullptr };

    bool _initialized{ false };
    bool _closing{ false };
};

// ======================================================================================
// ==================== FileWatchAwaitable ==============================================
// ======================================================================================

struct FileWatchAwaitable final {
    explicit FileWatchAwaitable( std::shared_ptr<FileWatchAwaitableState> state );

    [[nodiscard]] auto await_ready() const noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) noexcept -> void;
    [[nodiscard]] auto await_resume() const noexcept -> FileWatchResult;

private:
    std::shared_ptr<FileWatchAwaitableState> _state{};
};

}  // namespace tire
