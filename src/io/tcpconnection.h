#pragma once

#include <coroutine>
#include <cstddef>
#include <memory>
#include <vector>

#include <uv.h>

namespace tire {

struct Scheduler;
struct TcpConnectionState;
struct TcpListenerState;

struct TcpReadResult {
    int _status{ 0 };
    std::vector<std::byte> _bytes{};
};

struct TcpWriteResult {
    int _status{ 0 };
};

struct TcpReadOperation {
    std::shared_ptr<TcpConnectionState> _state{};
    std::coroutine_handle<> _coroutine{};
    std::shared_ptr<TcpReadResult> _result{};
    void* _allocation{ nullptr };
    bool _completed{ false };
};

struct TcpWriteOperation {
    std::shared_ptr<TcpConnectionState> _state{};
    std::coroutine_handle<> _coroutine{};
    std::vector<std::byte> _bytes{};
    std::shared_ptr<TcpWriteResult> _result{};
    uv_write_t _request{};
    bool _completed{ false };
};

struct TcpConnectionState : std::enable_shared_from_this<TcpConnectionState> {
    Scheduler* _scheduler{ nullptr };
    uv_tcp_t _handle{};
    std::shared_ptr<TcpConnectionState>* _lifetime{ nullptr };
    std::shared_ptr<TcpReadOperation> _readOperation{};
    std::shared_ptr<TcpWriteOperation> _writeOperation{};
    bool _initialized{ false };
    bool _closing{ false };
};

// ======================================================================================
// ==================== TcpConnection ===================================================
// ======================================================================================

struct TcpConnection final {
    friend struct Scheduler;

    explicit TcpConnection( std::shared_ptr<TcpConnectionState> state );

    [[nodiscard]] auto nativeHandle() const -> uv_tcp_t*;

private:
    std::shared_ptr<TcpConnectionState> _state{};
};

// ======================================================================================
// ==================== TcpListener =====================================================
// ======================================================================================

struct TcpListener final {
    friend struct Scheduler;

    explicit TcpListener( std::shared_ptr<TcpListenerState> state );

    [[nodiscard]] auto nativeHandle() const -> uv_tcp_t*;

private:
    std::shared_ptr<TcpListenerState> _state{};
};

// ======================================================================================
// ==================== TcpReadAwaitable ================================================
// ======================================================================================

struct TcpReadAwaitable final {
    std::shared_ptr<TcpConnectionState> _state{};
    std::shared_ptr<TcpReadResult> _result{ std::make_shared<TcpReadResult>() };

    auto await_ready() noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) -> void;
    auto await_resume() noexcept -> TcpReadResult;
};

// ======================================================================================
// ==================== TcpWriteAwaitable ===============================================
// ======================================================================================

struct TcpWriteAwaitable final {
    std::shared_ptr<TcpConnectionState> _state{};
    std::vector<std::byte> _bytes{};
    std::shared_ptr<TcpWriteResult> _result{ std::make_shared<TcpWriteResult>() };

    auto await_ready() noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) -> void;
    auto await_resume() noexcept -> TcpWriteResult;
};

}  // namespace tire
