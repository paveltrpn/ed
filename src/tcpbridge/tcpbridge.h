#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "tcpbridge/framecodec.h"

#include "io/async.h"
#include "io/scheduler.h"
#include "io/tcpclient.h"
#include "io/tcpconnection.h"
#include "io/tcpserver.h"
#include "io/timer.h"

namespace tire {

struct Scheduler;

// ======================================================================================
// ==================== TcpBridgeRetryOptions ===========================================
// ======================================================================================

struct TcpBridgeRetryOptions {
    size_t _maximumAttempts{ 16 };
    uint64_t _delayMilliseconds{ 100 };
};

// ======================================================================================
// ==================== TcpBridgeMessage ================================================
// ======================================================================================

struct TcpBridgeMessage {
    int _status{ 0 };
    std::vector<std::byte> _payload{};
};

// ======================================================================================
// ==================== TcpBridge =======================================================
// ======================================================================================

struct TcpBridge final {
    explicit TcpBridge( size_t maximumPayload = FrameCodec::_defaultMaximumPayload );
    ~TcpBridge();

    TcpBridge( const TcpBridge& ) = delete;
    TcpBridge( TcpBridge&& ) = delete;
    auto operator=( const TcpBridge& ) -> TcpBridge& = delete;
    auto operator=( TcpBridge&& ) -> TcpBridge& = delete;

    // Listen on the endpoint and accept one incoming TCP connection.
    auto accept( std::string host, uint16_t port ) -> Task<int>;

    // Connect to a TCP endpoint, retrying until the attempt limit is exhausted.
    auto connect( std::string host, uint16_t port, TcpBridgeRetryOptions retry ) -> Task<int>;

    // Read one framed message.
    auto read() -> Task<TcpBridgeMessage>;

    // Send one framed message.
    auto send( std::vector<std::byte> message ) -> Task<int>;

    auto delay( uint64_t timeoutMilliseconds ) -> Task<int>;

    // Cancel all pending operations and close the session. Safe to call from any thread.
    auto close() -> void;

private:
    std::unique_ptr<Scheduler> _scheduler{};
    std::atomic<bool> _closing{ false };
    size_t _maximumPayload{ FrameCodec::_defaultMaximumPayload };
    FrameCodec _codec{ _maximumPayload };

    std::mutex _sessionMutex{};

    std::shared_ptr<TcpListener> _listener{};
    std::shared_ptr<TcpConnection> _connection{};
    std::shared_ptr<TcpConnectAwaitable> _connect{};
    std::shared_ptr<TimeoutAwaitable<Task<void>>> _retryTimer{};
    std::shared_ptr<TimeoutAwaitable<Task<void>>> _delayTimer{};

    // Messages decoded ahead of the last co_awaited read.
    std::deque<std::vector<std::byte>> _pendingMessages{};
};

}  // namespace tire
