#include "tired/renderrequest.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <string_view>
#include <vector>

#include <QCoreApplication>
#include <QDir>
#include <QLinearGradient>
#include <QMetaObject>
#include <QPainter>
#include <QProcess>

#include <uv.h>

#include "log/log.h"
#include "tcpbridge/tcpbridge.h"

namespace {

constexpr uint16_t kRenderServerPort{ 45454 };

auto messageFromString( std::string_view text ) -> std::vector<std::byte> {
    auto message = std::vector<std::byte>{};
    message.resize( static_cast<size_t>( text.size() ) );
    std::memcpy( message.data(), text.data(), static_cast<size_t>( text.size() ) );
    return message;
}

auto payloadIs( const std::vector<std::byte> &payload, std::string_view text ) -> bool {
    if ( payload.size() != text.size() ) {
        return false;
    }

    for ( size_t index = 0; index < payload.size(); ++index ) {
        if ( payload[index] != static_cast<std::byte>( text[index] ) ) {
            return false;
        }
    }

    return true;
}

}  // namespace

namespace tire {

RenderRequest::RenderRequest( QObject *parent )
    : QObject{ parent } {
    _renderedImage = QImage{ _width, _height, QImage::Format_RGB32 };

    auto gradient = QLinearGradient{ QPointF{ 0.0, 0.0 }, QPointF{ static_cast<qreal>( _width ), 0.0 } };
    gradient.setColorAt( 0.0, Qt::blue );
    gradient.setColorAt( 1.0, Qt::gray );

    auto painter = QPainter{ &_renderedImage };
    painter.fillRect( _renderedImage.rect(), gradient );
}

RenderRequest::~RenderRequest() {
    // NOTE: deliberately no cross-thread wait for a session still running here.
    // The session coroutine resumes on the TcpBridge uv loop thread and uses `this`
    // and its members, so destroying the object while a session is in flight is
    // undefined behavior: the frame (~Task) and the members would be freed while the
    // loop thread is still on them. The condition-variable wait that used to guard
    // this was removed: it never runs in the current app (Tired is a leaked
    // singleton, the destructor is unreachable, and the real shutdown path is
    // aboutToQuit -> stop()), and it could block forever when the coroutine left
    // through the exception path, because Task<void> stores the exception in the
    // promise without signaling completion.
    //
    // Contract: a RenderRequest must outlive every session it has started. If
    // mid-session destruction is ever required, restore a completion signal that
    // covers the exception path too (e.g. completion notification from the
    // Task<void> promise / final_suspend), and tear the bridge down only after
    // joining the loop thread.
    stop();
}

void RenderRequest::start() {
    if ( _state != RenderRequestState::Idle && _state != RenderRequestState::Failed ) {
        return;
    }

    if ( !_sessionTask.empty() ) {
        // A previous session is still being wound down on the event loop thread.
        return;
    }

    _state = RenderRequestState::Starting;

    auto *application = QCoreApplication::instance();
    if ( application == nullptr ) {
        fail( QStringLiteral( "Cannot start a render request before QGuiApplication is constructed" ) );
        return;
    }

    if ( !_quitHandlerConnected ) {
        QObject::connect( application, &QCoreApplication::aboutToQuit, this, &RenderRequest::stop );
        _quitHandlerConnected = true;
    }

    try {
        _bridge = std::make_unique<TcpBridge>();

        _process = std::make_unique<QProcess>();

        QObject::connect( _process.get(), &QProcess::started, this, [this]() -> void {
            //
            if ( _state != RenderRequestState::Starting || _bridge == nullptr ) {
                return;
            }

            _state = RenderRequestState::Connecting;
            _sessionTask = runSession();
        } );

        QObject::connect( _process.get(), &QProcess::errorOccurred, this, [this]( QProcess::ProcessError ) -> void {
            //
            if ( _process == nullptr ) {
                return;
            }

            fail( QStringLiteral( "rt process error: %1" ).arg( _process->errorString() ) );
        } );

        QObject::connect( _process.get(), &QProcess::finished, this,
                          [this]( int exitCode, QProcess::ExitStatus ) -> void {
                              //
                              if ( _state == RenderRequestState::Idle || _state == RenderRequestState::Failed ) {
                                  return;
                              }
                              fail( QStringLiteral( "rt process exited unexpectedly with code %1" ).arg( exitCode ) );
                          } );

        auto executable = QDir{ QCoreApplication::applicationDirPath() }.filePath( QStringLiteral( "rt" ) );
#ifdef Q_OS_WIN
        executable += QStringLiteral( ".exe" );
#endif
        _process->setProgram( executable );
        _process->setArguments( { QStringLiteral( "--serve" ), QStringLiteral( "--port" ),
                                  QStringLiteral( "%1" ).arg( kRenderServerPort ) } );
        _process->start();

    } catch ( const std::exception &exception ) {
        fail( QStringLiteral( "Failed to initialize render request: %1" ).arg( exception.what() ) );
    } catch ( ... ) {
        fail( QStringLiteral( "Failed to initialize render request with an unknown error" ) );
    }
}

void RenderRequest::stop() {
    _state = RenderRequestState::Idle;
    cleanupSession();
}

auto RenderRequest::width() const -> int {
    return _width;
}

auto RenderRequest::height() const -> int {
    return _height;
}

auto RenderRequest::renderedImage() const -> QImage {
    return _renderedImage;
}

auto RenderRequest::setWidth( int value ) -> void {
    if ( value < 1 ) {
        return;
    }

    if ( _width == value ) {
        return;
    }

    _width = value;
    emit widthChanged();
}

auto RenderRequest::setHeight( int value ) -> void {
    if ( value < 1 ) {
        return;
    }

    if ( _height == value ) {
        return;
    }

    _height = value;
    emit heightChanged();
}

auto RenderRequest::setRenderedImage( const QImage &image ) -> void {
    if ( _renderedImage == image ) {
        return;
    }

    _renderedImage = image;
    emit renderedImageChanged();
}

auto RenderRequest::runSession() -> Task<void> {
    auto status = co_await _bridge->connect( "127.0.0.1", kRenderServerPort, TcpBridgeRetryOptions{} );

    if ( status == 0 ) {
        status = co_await _bridge->send( messageFromString( "handshake_from_ed" ) );
    }

    while ( status == 0 ) {
        const auto reply = co_await _bridge->read();
        status = reply._status;

        if ( status != 0 ) {
            break;
        }

        if ( payloadIs( reply._payload, "hello from rt" ) ) {
            break;
        }
    }

    if ( status == 0 ) {
        QMetaObject::invokeMethod(
            this,
            [this]() -> void {
                //
                handleHandshakeDone();
            },
            Qt::QueuedConnection );

        status = co_await _bridge->send( messageFromString( "dummy data to rt" ) );
    }

    while ( status == 0 ) {
        const auto reply = co_await _bridge->read();
        status = reply._status;

        if ( status != 0 ) {
            break;
        }

        if ( payloadIs( reply._payload, "return data to ed" ) ) {
            break;
        }
    }

    if ( status != 0 ) {
        const auto failure = status;
        QMetaObject::invokeMethod(
            this,
            [this, failure]() -> void {
                //
                handleFailure( failure );
            },
            Qt::QueuedConnection );
    } else {
        QMetaObject::invokeMethod(
            this,
            [this]() -> void {
                //
                handleSuccess();
            },
            Qt::QueuedConnection );
    }

    // NOTE: LLM suggest do it trough SessionEndAwaitable in await_suspend(),
    // not understand for what, just remove it.
    QMetaObject::invokeMethod(
        this,
        [owner = this]() -> void {
            //
            owner->endSession();
        },
        Qt::QueuedConnection );

    co_return;
}

auto RenderRequest::handleHandshakeDone() -> void {
    if ( _state != RenderRequestState::Connecting ) {
        return;
    }

    _state = RenderRequestState::WaitingForReply;

    log::info()( "handshake done!" );
}

auto RenderRequest::handleSuccess() -> void {
    if ( _state == RenderRequestState::Idle || _state == RenderRequestState::Failed ) {
        return;
    }

    log::info()( "TCP data transfer success!" );

    stop();

    emit echoCompleted();
}

auto RenderRequest::handleFailure( int status ) -> void {
    if ( _state == RenderRequestState::Idle || _state == RenderRequestState::Failed ) {
        return;
    }

    if ( status == UV_EOF ) {
        fail( QStringLiteral( "TCP connection closed unexpectedly" ) );
        return;
    }

    auto message = QStringLiteral( "TCP bridge error" );

    if ( status != UV_ECANCELED ) {
        message += QStringLiteral( ": %1" ).arg( QString::fromUtf8( uv_strerror( status ) ) );
    }

    fail( message );
}

auto RenderRequest::fail( const QString &message ) -> void {
    if ( _state == RenderRequestState::Idle || _state == RenderRequestState::Failed ) {
        return;
    }

    _state = RenderRequestState::Failed;

    cleanupSession();

    log::error()( "Render request failed: {}", message.toStdString() );

    emit errorOccurred( message );
}

auto RenderRequest::cleanupSession() -> void {
    if ( _bridge != nullptr ) {
        _bridge->close();

        // The bridge outlives the session coroutine while its awaits are being canceled.
        if ( _sessionTask.empty() ) {
            _bridge.reset();
        }
    }

    if ( _process == nullptr ) {
        return;
    }

    QObject::disconnect( _process.get(), nullptr, this, nullptr );

    if ( _process->state() != QProcess::NotRunning ) {
        if ( !_process->waitForFinished( 500 ) ) {
            _process->terminate();
        }

        if ( _process->state() != QProcess::NotRunning && !_process->waitForFinished( 500 ) ) {
            _process->kill();
        }

        if ( _process->state() != QProcess::NotRunning ) {
            _process->waitForFinished( 500 );
        }
    }

    _process.reset();
}

auto RenderRequest::endSession() -> void {
    _bridge.reset();
    _sessionTask = Task<void>{};
}

}  // namespace tire
