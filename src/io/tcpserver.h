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
struct TcpListenerState;

struct TcpListenResult {
    int _status{ 0 };
};

struct TcpAcceptResult {
    int _status{ 0 };
    std::shared_ptr<TcpConnection> _connection{};
};

struct TcpListenAwaitable;
struct TcpAcceptAwaitable;

struct TcpListenerState {
    Scheduler *_scheduler{ nullptr };
    uv_tcp_t _handle{};
    std::shared_ptr<TcpListenerState> *_lifetime{ nullptr };
    TcpListenAwaitable *_listenOperation{ nullptr };
    TcpAcceptAwaitable *_acceptOperation{ nullptr };
    bool _initialized{ false };
    bool _closed{ false };
};

// ======================================================================================
// ==================== TcpListenAwaitable ==============================================
// ======================================================================================

struct TcpListenAwaitable final : AwaitableState<TcpListenResult> {
    TcpListenAwaitable( Scheduler &scheduler, std::string host, uint16_t port );

    TcpListenAwaitable( const TcpListenAwaitable &other ) = delete;
    TcpListenAwaitable( TcpListenAwaitable &&other ) = delete;

    auto operator=( const TcpListenAwaitable &other ) -> TcpListenAwaitable & = delete;
    auto operator=( TcpListenAwaitable &&other ) -> TcpListenAwaitable & = delete;

    auto await_ready() noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) -> void;
    auto await_resume() noexcept -> TcpListenResult;

    [[nodiscard]] auto listener() const -> std::shared_ptr<TcpListener>;

    Scheduler *_scheduler{ nullptr };
    std::shared_ptr<TcpListenerState> _server{};
    std::shared_ptr<TcpListener> _listener{};
    std::string _host{};
    uint16_t _port{ 0 };
};

// ======================================================================================
// ==================== TcpAcceptAwaitable ==============================================
// ======================================================================================

struct TcpAcceptAwaitable final : AwaitableState<TcpAcceptResult> {
    explicit TcpAcceptAwaitable( const std::shared_ptr<TcpListenerState> &state );

    TcpAcceptAwaitable( const TcpAcceptAwaitable &other ) = delete;
    TcpAcceptAwaitable( TcpAcceptAwaitable &&other ) = delete;

    auto operator=( const TcpAcceptAwaitable &other ) -> TcpAcceptAwaitable & = delete;
    auto operator=( TcpAcceptAwaitable &&other ) -> TcpAcceptAwaitable & = delete;

    auto await_ready() noexcept -> bool;
    auto await_suspend( std::coroutine_handle<> coroutine ) -> void;
    auto await_resume() noexcept -> TcpAcceptResult;

    std::shared_ptr<TcpListenerState> _server{};
};

}  // namespace tire
