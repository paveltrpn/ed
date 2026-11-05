#pragma once

#include <coroutine>
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
// ==================== FileWatchAwaitable ==============================================
// ======================================================================================

struct FileWatchAwaitable final : AwaitableState<FileWatchResult> {
    FileWatchAwaitable( Scheduler& scheduler, std::string path );

    FileWatchAwaitable( const FileWatchAwaitable& other ) = delete;
    FileWatchAwaitable( FileWatchAwaitable&& other ) = delete;

    auto operator=( const FileWatchAwaitable& other ) -> FileWatchAwaitable& = delete;
    auto operator=( FileWatchAwaitable&& other ) -> FileWatchAwaitable& = delete;

    [[nodiscard]] auto await_ready() const noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) noexcept -> void;
    [[nodiscard]] auto await_resume() const noexcept -> FileWatchResult;

private:
    static auto eventCallback( uv_fs_event_t* handle, const char* filename, int events, int status ) -> void;
    auto close( int status ) -> void;

private:
    Scheduler* _scheduler{ nullptr };
    std::string _path{};
    uv_fs_event_t _watcher{};

    bool _initialized{ false };
    bool _closing{ false };
};

}  // namespace tire
