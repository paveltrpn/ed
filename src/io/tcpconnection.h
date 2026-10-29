#pragma once

#include <coroutine>
#include <cstddef>
#include <memory>
#include <vector>

#include <uv.h>

namespace tire {

struct Scheduler;

// ======================================================================================
// ==================== TcpConnection ===================================================
// ======================================================================================

struct TcpConnection final {
    using Bytes = std::vector<std::byte>;

    struct ReadResult {
        int _status{ 0 };
        Bytes _bytes{};
    };

    struct WriteResult {
        int _status{ 0 };
    };

    struct State;

    struct ReadAwaitable {
        std::shared_ptr<State> _state{};
        std::shared_ptr<ReadResult> _result{ std::make_shared<ReadResult>() };

        auto await_ready() noexcept -> bool;
        auto await_suspend( std::coroutine_handle<> coroutine ) -> void;
        auto await_resume() noexcept -> ReadResult;
    };

    struct WriteAwaitable {
        std::shared_ptr<State> _state{};
        Bytes _bytes{};
        std::shared_ptr<WriteResult> _result{ std::make_shared<WriteResult>() };

        auto await_ready() noexcept -> bool;
        auto await_suspend( std::coroutine_handle<> coroutine ) -> void;
        auto await_resume() noexcept -> WriteResult;
    };

    explicit TcpConnection( std::shared_ptr<State> state );

    static auto create( Scheduler &scheduler, uv_loop_t *loop, int &status ) -> std::shared_ptr<TcpConnection>;

    auto read() -> ReadAwaitable;
    auto write( Bytes bytes ) -> WriteAwaitable;
    auto close() -> void;

    [[nodiscard]] auto nativeHandle() const -> uv_tcp_t *;

private:
    std::shared_ptr<State> _state{};
};

}
