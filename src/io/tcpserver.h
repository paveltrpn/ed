#pragma once

#include <cstdint>
#include <coroutine>
#include <memory>
#include <string>
#include <utility>

#include "io/tcpconnection.h"

namespace tire {

struct Scheduler;

struct TcpListenResult {
    int _status{ 0 };
};

struct TcpAcceptResult {
    int _status{ 0 };
    std::shared_ptr<TcpConnection> _connection{};
};

// ======================================================================================
// ==================== TcpListenAwaitable ==============================================
// ======================================================================================

struct TcpListenAwaitable final {
    struct State;

    TcpListenAwaitable( Scheduler &scheduler, std::string host, uint16_t port );

    auto await_ready() noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) -> void;
    auto await_resume() noexcept -> TcpListenResult;

    [[nodiscard]] auto listener() const -> std::shared_ptr<TcpListener>;

    std::shared_ptr<State> _state{};
};

// ======================================================================================
// ==================== TcpAcceptAwaitable ==============================================
// ======================================================================================

struct TcpAcceptAwaitable final {
    struct State;

    explicit TcpAcceptAwaitable( const std::shared_ptr<TcpListener::State> &state );

    auto await_ready() noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) -> void;
    auto await_resume() noexcept -> TcpAcceptResult;

    std::shared_ptr<State> _state{};
};

}  // namespace tire
