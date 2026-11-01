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
// ==================== TcpConnectAwaitable =============================================
// ======================================================================================

struct TcpConnectAwaitable final {
    struct State;

    TcpConnectAwaitable( Scheduler &scheduler, std::string host, uint16_t port );

    auto await_ready() noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) -> void;
    auto await_resume() noexcept -> TcpConnectResult;

    auto cancel() -> void;

    std::shared_ptr<State> _state{};
};

}  // namespace tire
