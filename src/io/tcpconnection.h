#pragma once

#include <coroutine>
#include <cstddef>
#include <memory>
#include <vector>

#include <uv.h>

#include "io/awaitablestate.h"

namespace tire {

struct Scheduler;
struct TcpConnectionState;
struct TcpListenerState;
struct TcpReadAwaitable;
struct TcpWriteAwaitable;

struct TcpReadResult {
    int _status{ 0 };
    std::vector<std::byte> _bytes{};
};

struct TcpWriteResult {
    int _status{ 0 };
};

struct TcpConnectionState : std::enable_shared_from_this<TcpConnectionState> {
    Scheduler* _scheduler{ nullptr };
    uv_tcp_t _handle{};
    std::shared_ptr<TcpConnectionState>* _lifetime{ nullptr };
    TcpReadAwaitable* _readOperation{ nullptr };
    TcpWriteAwaitable* _writeOperation{ nullptr };
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

struct TcpReadAwaitable final : AwaitableState<TcpReadResult> {
    explicit TcpReadAwaitable( std::shared_ptr<TcpConnectionState> state );

    TcpReadAwaitable( const TcpReadAwaitable &other ) = delete;
    TcpReadAwaitable( TcpReadAwaitable &&other ) = delete;

    auto operator=( const TcpReadAwaitable &other ) -> TcpReadAwaitable & = delete;
    auto operator=( TcpReadAwaitable &&other ) -> TcpReadAwaitable & = delete;

    auto await_ready() noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) -> void;
    auto await_resume() noexcept -> TcpReadResult;

    std::shared_ptr<TcpConnectionState> _state{};
    void *_allocation{ nullptr };
};

// ======================================================================================
// ==================== TcpWriteAwaitable ===============================================
// ======================================================================================

struct TcpWriteAwaitable final : AwaitableState<TcpWriteResult> {
    TcpWriteAwaitable( std::shared_ptr<TcpConnectionState> state, std::vector<std::byte> bytes );

    TcpWriteAwaitable( const TcpWriteAwaitable &other ) = delete;
    TcpWriteAwaitable( TcpWriteAwaitable &&other ) = delete;

    auto operator=( const TcpWriteAwaitable &other ) -> TcpWriteAwaitable & = delete;
    auto operator=( TcpWriteAwaitable &&other ) -> TcpWriteAwaitable & = delete;

    auto await_ready() noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) -> void;
    auto await_resume() noexcept -> TcpWriteResult;

    std::shared_ptr<TcpConnectionState> _state{};
    std::vector<std::byte> _bytes{};
    uv_write_t _request{};
};

}  // namespace tire
