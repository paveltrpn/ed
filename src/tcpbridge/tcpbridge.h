#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <optional>

#include "tcpbridge/framecodec.h"

#include "io/async.h"
#include "io/scheduler.h"
#include "io/tcpclient.h"
#include "io/tcpserver.h"
#include "io/timer.h"

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

    struct State {
        explicit State( Scheduler *scheduler, size_t maximumPayload )
            : _scheduler{ scheduler }
            , _maximumPayload{ maximumPayload } {}

        Scheduler *_scheduler{ nullptr };
        Callbacks _callbacks{};
        size_t _maximumPayload{ FrameCodec::_defaultMaximumPayload };

        std::atomic<bool> _started{ false };
        std::atomic<bool> _closeRequested{ false };
        std::atomic<bool> _closing{ false };

        bool _connected{ false };
        bool _errorReported{ false };

        std::shared_ptr<TcpClient> _client{};
        std::shared_ptr<TcpServer> _server{};
        std::shared_ptr<TcpConnection> _connection{};

        std::optional<TimeoutAwaitable<Task<void>>> _retryTimer{};
        std::deque<Message> _outbound{};
        std::mutex _outboundMutex{};

        std::coroutine_handle<> _drainCoroutine{};

        bool _drainWaiting{ false };

        Task<void> _clientTask{};
        Task<void> _serverTask{};
        Task<void> _drainTask{};

        std::mutex _workerMutex{};
        std::condition_variable _workerCondition{};

        size_t _workerCount{ 0 };
    };

private:
    std::unique_ptr<Scheduler> _scheduler{};
    std::shared_ptr<State> _state{};
};

}  // namespace tire
