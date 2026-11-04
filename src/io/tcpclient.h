#pragma once

#include <cstdint>
#include <coroutine>
#include <memory>
#include <string>
#include <utility>

#include "io/tcpconnection.h"

namespace tire {

struct Scheduler;

struct TcpConnectResult {
    int _status{ 0 };
    std::shared_ptr<TcpConnection> _connection{};
};

// ======================================================================================
// ==================== TcpConnectAwaitableState ========================================
// ======================================================================================

struct TcpConnectAwaitableState : std::enable_shared_from_this<TcpConnectAwaitableState> {
    TcpConnectAwaitableState( Scheduler &scheduler, std::string host, uint16_t port );

    auto cancel() -> void;

public:
    Scheduler *_scheduler{ nullptr };
    std::shared_ptr<TcpConnectResult> _result{ std::make_shared<TcpConnectResult>() };
    std::shared_ptr<TcpConnection> _connection{};
    std::coroutine_handle<> _coroutine{};
    std::string _host{};
    uint16_t _port{ 0 };
    uv_connect_t _request{};
    bool _completed{ false };
};

// ======================================================================================
// ==================== TcpConnectAwaitable =============================================
// ======================================================================================

struct TcpConnectAwaitable final {
    explicit TcpConnectAwaitable( std::shared_ptr<TcpConnectAwaitableState> state );

    auto await_ready() noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) -> void;
    auto await_resume() noexcept -> TcpConnectResult;

    std::shared_ptr<TcpConnectAwaitableState> _state{};
};

}  // namespace tire
