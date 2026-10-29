#pragma once

#include <coroutine>
#include <cstdint>
#include <memory>
#include <string>

#include "io/tcpconnection.h"

namespace tire {

struct Scheduler;

// ======================================================================================
// ==================== TcpClient =======================================================
// ======================================================================================

struct TcpClient final {
    struct ConnectResult {
        int _status{ 0 };
        std::shared_ptr<TcpConnection> _connection{};
    };

    struct State;

    struct ConnectAwaitable {
        std::shared_ptr<State> _state{};
        std::string _host{};
        uint16_t _port{ 0 };
        std::shared_ptr<ConnectResult> _result{ std::make_shared<ConnectResult>() };

        auto await_ready() noexcept -> bool;
        auto await_suspend( std::coroutine_handle<> coroutine ) -> void;
        auto await_resume() noexcept -> ConnectResult;
    };

    explicit TcpClient( Scheduler &scheduler );

    auto connect( std::string host, uint16_t port ) -> ConnectAwaitable;
    auto close() -> void;

private:
    std::shared_ptr<State> _state{};
};

}
