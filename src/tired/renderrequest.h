#pragma once

#include <cstdint>
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
    enum class State { Idle, Starting, Connecting, WaitingForEcho, Ready, Failed };

    auto fail( const QString &message ) -> void;
    auto cleanupSession() -> void;
    auto handleConnected( uint64_t generation ) -> void;
    auto handleMessage( uint64_t generation, const QByteArray &payload ) -> void;
    auto handleDisconnected( uint64_t generation ) -> void;
    auto handleBridgeError( uint64_t generation, int status, const std::string &context ) -> void;

    State _state{ State::Idle };
    std::unique_ptr<TcpBridge> _bridge{};
    std::unique_ptr<QProcess> _process{};
    QByteArray _expectedEcho{};
    uint64_t _generation{ 0 };
    bool _quitHandlerConnected{ false };
};

}
