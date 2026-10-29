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

struct TcpBridgeRetryOptions {
    size_t _maximumAttempts{ 16 };
    uint64_t _delayMilliseconds{ 100 };
};

struct TcpBridgeError {
    int _status{ 0 };
    std::string _context{};
};

struct TcpBridgeCallbacks {
    std::function<void()> _connected{};
    std::function<void( std::vector<std::byte> )> _message{};
    std::function<void()> _disconnected{};
    std::function<void( TcpBridgeError )> _error{};
};

struct TcpBridgeState {
    explicit TcpBridgeState( Scheduler* scheduler, size_t maximumPayload )
        : _scheduler{ scheduler }
        , _maximumPayload{ maximumPayload } {}

    Scheduler* _scheduler{ nullptr };
    TcpBridgeCallbacks _callbacks{};
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

    // Message queue.
    std::deque<std::vector<std::byte>> _outbound{};

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

    auto setCallbacks( TcpBridgeCallbacks callbacks ) -> void;

    auto listen( std::string host, uint16_t port ) -> void;
    auto connect( std::string host, uint16_t port, TcpBridgeRetryOptions retry ) -> void;
    auto send( std::vector<std::byte> message ) -> void;
    auto close() -> void;

private:
    std::unique_ptr<Scheduler> _scheduler{};
    std::shared_ptr<TcpBridgeState> _state{};
};

// ======================================================================================
// ==================== WorkerCompletion ================================================
// ======================================================================================

struct WorkerCompletion {
    std::shared_ptr<TcpBridgeState> _state{};

    ~WorkerCompletion() {
        _state->_scheduler->post( [state = _state]( uv_loop_t* ) -> void {
            //
            std::scoped_lock lock{ state->_workerMutex };

            if ( state->_workerCount > 0 ) {
                --state->_workerCount;
            }

            if ( state->_workerCount == 0 ) {
                state->_workerCondition.notify_all();
            }
        } );
    }
};

// ======================================================================================
// ==================== DrainAwaitable ==================================================
// ======================================================================================

struct DrainAwaitable {
    std::shared_ptr<TcpBridgeState> _state{};

    auto await_ready() const noexcept -> bool {
        //
        return false;
    }

    auto await_suspend( std::coroutine_handle<> coroutine ) noexcept -> void {
        _state->_drainCoroutine = coroutine;
        _state->_drainWaiting = true;
    }

    auto await_resume() const noexcept -> void {
        //
    }
};

}  // namespace tire
