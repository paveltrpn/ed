#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "tcpbridge/framecodec.h"

namespace tire {

struct Scheduler;

// ======================================================================================
// ==================== TcpBridge =======================================================
// ======================================================================================

struct TcpBridge final {
    using Message = std::vector<std::byte>;

    struct RetryOptions {
        size_t _maximumAttempts{ 40 };
        uint64_t _delayMilliseconds{ 50 };
    };

    struct Error {
        int _status{ 0 };
        std::string _context{};
    };

    struct Callbacks {
        std::function<void()> _connected{};
        std::function<void( Message )> _message{};
        std::function<void()> _disconnected{};
        std::function<void( Error )> _error{};
    };

    explicit TcpBridge( size_t maximumPayload = FrameCodec::_defaultMaximumPayload );
    ~TcpBridge();

    TcpBridge( const TcpBridge & ) = delete;
    TcpBridge( TcpBridge && ) = delete;
    auto operator=( const TcpBridge & ) -> TcpBridge & = delete;
    auto operator=( TcpBridge && ) -> TcpBridge & = delete;

    auto setCallbacks( Callbacks callbacks ) -> void;
    auto listen( std::string host, uint16_t port ) -> void;
    auto connect( std::string host, uint16_t port, RetryOptions retry ) -> void;
    auto send( Message message ) -> void;
    auto close() -> void;

    struct State;

private:
    std::unique_ptr<Scheduler> _scheduler{};
    std::shared_ptr<State> _state{};
};

}
