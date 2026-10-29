#pragma once

#include <coroutine>
#include <cstdint>
#include <memory>
#include <string>

#include "io/tcpconnection.h"

namespace tire {

struct Scheduler;

// ======================================================================================
// ==================== TcpServer =======================================================
// ======================================================================================

struct TcpServer final {
    struct ListenResult {
        int _status{ 0 };
    };

    struct AcceptResult {
        int _status{ 0 };
        std::shared_ptr<TcpConnection> _connection{};
    };

    struct State;

    struct ListenAwaitable {
        std::shared_ptr<State> _state{};
        std::string _host{};
        uint16_t _port{ 0 };
        std::shared_ptr<ListenResult> _result{ std::make_shared<ListenResult>() };

        auto await_ready() noexcept -> bool;
        auto await_suspend( std::coroutine_handle<> coroutine ) -> void;
        auto await_resume() noexcept -> ListenResult;
    };

    struct AcceptAwaitable {
        std::shared_ptr<State> _state{};
        std::shared_ptr<AcceptResult> _result{ std::make_shared<AcceptResult>() };

        auto await_ready() noexcept -> bool;
        auto await_suspend( std::coroutine_handle<> coroutine ) -> void;
        auto await_resume() noexcept -> AcceptResult;
    };

    explicit TcpServer( Scheduler &scheduler );

    auto listen( std::string host, uint16_t port ) -> ListenAwaitable;
    auto accept() -> AcceptAwaitable;
    auto close() -> void;

private:
    std::shared_ptr<State> _state{};
};

}
