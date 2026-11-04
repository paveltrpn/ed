#pragma once

#include <cstdint>
#include <coroutine>
#include <memory>
#include <string>
#include <utility>

#include "io/awaitablestate.h"
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

struct TcpConnectAwaitableState : AwaitableState<TcpConnectAwaitableState, TcpConnectResult> {
    TcpConnectAwaitableState( Scheduler &scheduler, std::string host, uint16_t port );

    auto cancel() -> void;

public:
    Scheduler *_scheduler{ nullptr };
    std::shared_ptr<TcpConnection> _connection{};
    std::string _host{};
    uint16_t _port{ 0 };
    uv_connect_t _request{};
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
