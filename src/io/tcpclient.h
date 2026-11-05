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
// ==================== TcpConnectAwaitable =============================================
// ======================================================================================

struct TcpConnectAwaitable final : AwaitableState<TcpConnectResult> {
    TcpConnectAwaitable( Scheduler &scheduler, std::string host, uint16_t port );

    TcpConnectAwaitable( const TcpConnectAwaitable &other ) = delete;
    TcpConnectAwaitable( TcpConnectAwaitable &&other ) = delete;

    auto operator=( const TcpConnectAwaitable &other ) -> TcpConnectAwaitable & = delete;
    auto operator=( TcpConnectAwaitable &&other ) -> TcpConnectAwaitable & = delete;

    auto await_ready() noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) -> void;
    auto await_resume() noexcept -> TcpConnectResult;

    Scheduler *_scheduler{ nullptr };
    std::shared_ptr<TcpConnection> _connection{};
    std::string _host{};
    uint16_t _port{ 0 };
    uv_connect_t _request{};
};

}  // namespace tire
