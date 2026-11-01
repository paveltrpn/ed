#pragma once

#include <atomic>
#include <condition_variable>
#include <coroutine>
#include <memory>
#include <mutex>

#include <QObject>
#include <QString>

#include "io/async.h"

class QProcess;

namespace tire {

struct TcpBridge;

enum class RenderRequestState {
    //
    Idle,
    Starting,
    Connecting,
    WaitingForReply,
    Failed
};

// ======================================================================================
// ==================== RenderRequest ===================================================
// ======================================================================================

struct RenderRequest final : QObject {
    Q_OBJECT

public:
    explicit RenderRequest( QObject *parent = nullptr );
    ~RenderRequest() override;

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

signals:
    auto errorOccurred( const QString &message ) -> void;
    auto echoCompleted() -> void;

private:
    struct SessionEndAwaitable final {
        RenderRequest *_owner{ nullptr };

        [[nodiscard]] auto await_ready() const noexcept -> bool {
            //
            return false;
        }

        auto await_suspend( std::coroutine_handle<> coroutine ) noexcept -> void;

        auto await_resume() const noexcept -> void {
            //
        }
    };

    auto runSession() -> Task<void>;

    auto fail( const QString &message ) -> void;

    auto cleanupSession() -> void;
    auto endSession() -> void;

    auto handleHandshakeDone() -> void;
    auto handleSuccess() -> void;
    auto handleFailure( int status ) -> void;

private:
    RenderRequestState _state{ RenderRequestState::Idle };

    std::unique_ptr<TcpBridge> _bridge{};
    Task<void> _sessionTask{};
    std::unique_ptr<QProcess> _process{};

    bool _quitHandlerConnected{ false };

    std::atomic<bool> _sessionDone{ false };
    std::mutex _doneMutex{};
    std::condition_variable _doneCondition{};
};

}  // namespace tire
