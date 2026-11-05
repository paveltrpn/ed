#pragma once

#include <fcntl.h>

#include <coroutine>
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
// ==================== FileOpenAwaitable ===============================================
// ======================================================================================

struct FileOpenAwaitable final : AwaitableState<FileOpenResult> {
    FileOpenAwaitable( Scheduler& scheduler, std::string path );

    FileOpenAwaitable( const FileOpenAwaitable& other ) = delete;
    FileOpenAwaitable( FileOpenAwaitable&& other ) = delete;

    auto operator=( const FileOpenAwaitable& other ) -> FileOpenAwaitable& = delete;
    auto operator=( FileOpenAwaitable&& other ) -> FileOpenAwaitable& = delete;

    [[nodiscard]] auto await_ready() const noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) noexcept -> void;
    [[nodiscard]] auto await_resume() const noexcept -> int;

private:
    static auto openCallback( uv_fs_t* request ) -> void;
    auto start( uv_loop_t* loop ) -> void;

private:
    Scheduler* _scheduler{ nullptr };
    std::string _path{};
    uv_fs_t _request{};
};

}  // namespace tire
