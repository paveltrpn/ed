#pragma once

#include <cstdint>
#include <coroutine>
#include <memory>
#include <string>
#include <utility>

#include "io/tcpconnection.h"

namespace tire {

struct Scheduler;
struct TcpListenerState;

struct TcpListenResult {
    int _status{ 0 };
};

struct TcpAcceptResult {
    int _status{ 0 };
    std::shared_ptr<TcpConnection> _connection{};
};

struct TcpAcceptAwaitableState {
    std::shared_ptr<TcpListenerState> _server{};
    std::shared_ptr<TcpAcceptResult> _result{ std::make_shared<TcpAcceptResult>() };
    std::coroutine_handle<> _coroutine{};
    bool _completed{ false };
};

struct TcpListenAwaitableState {
    Scheduler *_scheduler{ nullptr };
    std::shared_ptr<TcpListenerState> _server{};
    std::shared_ptr<TcpListener> _listener{};
    std::shared_ptr<TcpListenResult> _result{ std::make_shared<TcpListenResult>() };
    std::coroutine_handle<> _coroutine{};
    std::string _host{};
    uint16_t _port{ 0 };
    bool _completed{ false };
};

struct TcpListenerState {
    Scheduler *_scheduler{ nullptr };
    uv_tcp_t _handle{};
    std::shared_ptr<TcpListenerState> *_lifetime{ nullptr };
    std::shared_ptr<TcpListenAwaitableState> _listenOperation{};
    std::shared_ptr<TcpAcceptAwaitableState> _acceptOperation{};
    bool _initialized{ false };
    bool _closed{ false };
};

// ======================================================================================
// ==================== TcpListenAwaitable ==============================================
// ======================================================================================

struct TcpListenAwaitable final {
    TcpListenAwaitable( Scheduler &scheduler, std::string host, uint16_t port );

    auto await_ready() noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) -> void;
    auto await_resume() noexcept -> TcpListenResult;

    [[nodiscard]] auto listener() const -> std::shared_ptr<TcpListener>;

    std::shared_ptr<TcpListenAwaitableState> _state{};
};

// ======================================================================================
// ==================== TcpAcceptAwaitable ==============================================
// ======================================================================================

struct TcpAcceptAwaitable final {
    explicit TcpAcceptAwaitable( const std::shared_ptr<TcpListenerState> &state );

    auto await_ready() noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) -> void;
    auto await_resume() noexcept -> TcpAcceptResult;

    std::shared_ptr<TcpAcceptAwaitableState> _state{};
};

}  // namespace tire
