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
    friend struct Scheduler;

    struct State;

    explicit TcpConnection( std::shared_ptr<State> state );

    [[nodiscard]] auto nativeHandle() const -> uv_tcp_t *;

private:
    std::shared_ptr<State> _state{};
};

// ======================================================================================
// ==================== TcpListener =====================================================
// ======================================================================================

struct TcpListener final {
    friend struct Scheduler;

    struct State;

    explicit TcpListener( std::shared_ptr<State> state );

    [[nodiscard]] auto nativeHandle() const -> uv_tcp_t *;

private:
    std::shared_ptr<State> _state{};
};

struct TcpReadResult {
    int _status{ 0 };
    std::vector<std::byte> _bytes{};
};

struct TcpWriteResult {
    int _status{ 0 };
};

// ======================================================================================
// ==================== TcpReadAwaitable ================================================
// ======================================================================================

struct TcpReadAwaitable final {
    std::shared_ptr<TcpConnection::State> _state{};
    std::shared_ptr<TcpReadResult> _result{ std::make_shared<TcpReadResult>() };

    auto await_ready() noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) -> void;
    auto await_resume() noexcept -> TcpReadResult;
};

// ======================================================================================
// ==================== TcpWriteAwaitable ===============================================
// ======================================================================================

struct TcpWriteAwaitable final {
    std::shared_ptr<TcpConnection::State> _state{};
    std::vector<std::byte> _bytes{};
    std::shared_ptr<TcpWriteResult> _result{ std::make_shared<TcpWriteResult>() };

    auto await_ready() noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) -> void;
    auto await_resume() noexcept -> TcpWriteResult;
};

}
