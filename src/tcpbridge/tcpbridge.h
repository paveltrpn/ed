#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "io/async.h"
#include "io/scheduler.h"
#include "io/tcpclient.h"
#include "io/tcpconnection.h"
#include "io/tcpserver.h"
#include "io/timer.h"

namespace tire {

struct Scheduler;

enum class FrameStatus {
    //
    OK,
    OVERSIZED,
    PARTIAL_EOF
};

struct FrameDecodeResult {
    FrameStatus _status{ FrameStatus::OK };
    std::vector<std::vector<std::byte>> _messages{};
};

// ======================================================================================
// ==================== TcpFrameCodec ===================================================
// ======================================================================================

// Encodes payloads with a 32-bit big-endian length prefix and decodes complete frames
// from a TCP byte stream, buffering partial input and rejecting oversized payloads.
struct TcpFrameCodec {
    explicit TcpFrameCodec( size_t maximumPayload = _defaultMaximumPayload );

    auto push( const std::byte* data, size_t size ) -> FrameDecodeResult;
    auto push( const std::vector<std::byte>& data ) -> FrameDecodeResult;
    auto endOfStream() const noexcept -> FrameStatus;
    auto reset() -> void;

    static auto encode( const std::vector<std::byte>& payload ) -> std::vector<std::byte>;

public:
    static constexpr size_t _defaultMaximumPayload{ 1024 * 1024 };
    static constexpr size_t _prefixSize{ 4 };

private:
    size_t _maximumPayload;
    std::vector<std::byte> _buffer{};
};

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
    explicit TcpBridge( size_t maximumPayload = TcpFrameCodec::_defaultMaximumPayload );
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
    size_t _maximumPayload{ TcpFrameCodec::_defaultMaximumPayload };
    TcpFrameCodec _codec{ _maximumPayload };

    // Protects session state shared by scheduler coroutines and external threads, including
    // connection handles, pending operations, codec state, and queued messages.
    std::mutex _sessionMutex{};

    std::shared_ptr<TcpListener> _listener{};
    std::shared_ptr<TcpConnection> _connection{};
    std::shared_ptr<TcpConnectAwaitableState> _connect{};
    std::shared_ptr<TimeoutAwaitableState> _retryTimer{};
    std::shared_ptr<TimeoutAwaitableState> _delayTimer{};

    // Messages decoded ahead of the last co_awaited read.
    std::deque<std::vector<std::byte>> _pendingMessages{};
};

}  // namespace tire
