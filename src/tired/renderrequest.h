#pragma once

#include <coroutine>
#include <memory>

#include <QImage>
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

    Q_PROPERTY( int width READ width WRITE setWidth NOTIFY widthChanged FINAL )
    Q_PROPERTY( int height READ height WRITE setHeight NOTIFY heightChanged FINAL )
    Q_PROPERTY( QImage renderedImage READ renderedImage WRITE setRenderedImage NOTIFY renderedImageChanged FINAL )

public:
    explicit RenderRequest( QObject *parent = nullptr );
    ~RenderRequest() override;

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

    auto width() const -> int;
    auto height() const -> int;
    auto renderedImage() const -> QImage;

    auto setWidth( int value ) -> void;
    auto setHeight( int value ) -> void;
    auto setRenderedImage( const QImage &image ) -> void;

signals:
    auto widthChanged() -> void;
    auto heightChanged() -> void;
    auto renderedImageChanged() -> void;

    auto errorOccurred( const QString &message ) -> void;
    auto echoCompleted() -> void;

private:
    auto runSession() -> Task<void>;

    auto fail( const QString &message ) -> void;

    auto cleanupSession() -> void;
    auto endSession() -> void;

    auto handleHandshakeDone() -> void;
    auto handleSuccess() -> void;
    auto handleFailure( int status ) -> void;

private:
    RenderRequestState _state{ RenderRequestState::Idle };

    int _width{ 1024 };
    int _height{ 768 };
    QImage _renderedImage{};

    std::unique_ptr<TcpBridge> _bridge{};
    Task<void> _sessionTask{};
    std::unique_ptr<QProcess> _process{};

    bool _quitHandlerConnected{ false };
};

}  // namespace tire
