#pragma once

#include <memory>
#include <string>

#include <QByteArray>
#include <QObject>
#include <QString>

class QProcess;

namespace tire {

struct TcpBridge;

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
    auto fail( const QString &message ) -> void;
    auto cleanupSession() -> void;
    auto handleConnected() -> void;
    auto handleMessage( const QByteArray &payload ) -> void;
    auto handleDisconnected() -> void;
    auto handleBridgeError( int status, const std::string &context ) -> void;

private:
    enum class State { Idle, Starting, Connecting, WaitingForEcho, Failed };

    State _state{ State::Idle };
    std::unique_ptr<TcpBridge> _bridge{};
    std::unique_ptr<QProcess> _process{};
    bool _quitHandlerConnected{ false };
};

}  // namespace tire
