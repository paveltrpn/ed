#pragma once

#include <fcntl.h>

#include <coroutine>
#include <memory>
#include <string>

#include <uv.h>

#include "io/awaitablestate.h"
#include "io/scheduler.h"

namespace tire {

// ======================================================================================
// ==================== FileOpenResult ==================================================
// ======================================================================================

struct FileOpenResult {
    int _status{ 0 };
};

// ======================================================================================
// ==================== FileOpenAwaitableState ==========================================
// ======================================================================================

struct FileOpenAwaitableState : AwaitableState<FileOpenAwaitableState, FileOpenResult> {
    FileOpenAwaitableState( Scheduler& scheduler, std::string path );

    auto cancel() -> void;

public:
    Scheduler* _scheduler{ nullptr };
    std::string _path{};
    uv_fs_t _request{};

    std::shared_ptr<FileOpenAwaitableState>* _lifetime{ nullptr };

    bool _started{ false };
};

// ======================================================================================
// ==================== FileOpenAwaitable ===============================================
// ======================================================================================

struct FileOpenAwaitable final {
    explicit FileOpenAwaitable( std::shared_ptr<FileOpenAwaitableState> state );

    [[nodiscard]] auto await_ready() const noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) noexcept -> void;
    [[nodiscard]] auto await_resume() const noexcept -> int;

private:
    std::shared_ptr<FileOpenAwaitableState> _state{};
};

}  // namespace tire
