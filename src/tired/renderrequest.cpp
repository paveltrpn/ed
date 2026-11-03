#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <QCoreApplication>
#include <QDir>
#include <QLinearGradient>
#include <QMetaObject>
#include <QPainter>
#include <QProcess>

#include <uv.h>

#include "tired/renderrequest.h"

#include "log/log.h"
#include "tcpbridge/renderprotocol.h"
#include "tcpbridge/tcpbridge.h"
#include "tired/subgraph/scene.h"

namespace {

constexpr uint16_t kRenderServerPort{ 45454 };
constexpr uint64_t kRenderPollDelayMilliseconds{ 200 };
constexpr size_t kPayloadPreviewMaximumSize{ 512 };

auto payloadPreview( const std::vector<std::byte> &payload ) -> std::string {
    auto size = payload.size();
    if ( size > kPayloadPreviewMaximumSize ) {
        size = kPayloadPreviewMaximumSize;
    }

    auto preview = std::string{};
    preview.reserve( size );

    for ( size_t index = 0; index < size; ++index ) {
        const auto value = std::to_integer<unsigned char>( payload[index] );
        if ( value >= 0x20U && value < 0x7fU ) {
            preview += static_cast<char>( value );
        } else {
            preview += '.';
        }
    }

    return preview;
}

}  // namespace

namespace tire {

RenderRequest::RenderRequest( vsg::ref_ptr<vsg::Camera> camera, SceneUIProxy *scene, QObject *parent )
    : QObject{ parent }
    , _camera{ std::move( camera ) }
    , _scene{ scene } {
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

    if ( !_quitHandlerConnected ) {
        QObject::connect( application, &QCoreApplication::aboutToQuit, this, &RenderRequest::stop );
        _quitHandlerConnected = true;
    }

    try {
        const auto lookAt = _camera->viewMatrix.cast<vsg::LookAt>();
        const auto perspective = _camera->projectionMatrix.cast<vsg::Perspective>();

        log::info()( "RT render started" );

        auto [sceneObj, sceneInfo] = _scene->serializeObjectsToObj();

        log::info()( "OBJ serialization completed: {} meshes, {} vertices, {} triangles", sceneInfo._meshCount,
                     sceneInfo._vertexCount, sceneInfo._triangleCount );

        _pendingCamera = RenderCamera{ { lookAt->eye.x, lookAt->eye.y, lookAt->eye.z },
                                       { lookAt->center.x, lookAt->center.y, lookAt->center.z },
                                       { lookAt->up.x, lookAt->up.y, lookAt->up.z },
                                       perspective->fieldOfViewY,
                                       perspective->nearDistance,
                                       perspective->farDistance };

        _pendingSceneObj = std::move( sceneObj );

        _bridge = std::make_unique<TcpBridge>( kRenderProtocolMaximumPayload );

        _process = std::make_unique<QProcess>();
        _process->setWorkingDirectory( QCoreApplication::applicationDirPath() );

        QObject::connect( _process.get(), &QProcess::started, this, [this]() -> void {
            //
            if ( _state != RenderRequestState::Starting || _bridge == nullptr ) {
                return;
            }

            _state = RenderRequestState::Connecting;
            _sessionTask = runSession( std::move( _pendingSceneObj ), _pendingCamera );
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

    if ( !ImageMessage::sizeIsValid( static_cast<uint32_t>( value ), static_cast<uint32_t>( _height ) ) ) {
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

    if ( !ImageMessage::sizeIsValid( static_cast<uint32_t>( _width ), static_cast<uint32_t>( value ) ) ) {
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

auto RenderRequest::runSession( std::string sceneObj, RenderCamera camera ) -> Task<void> {
    const auto renderWidth = static_cast<uint32_t>( _width );
    const auto renderHeight = static_cast<uint32_t>( _height );

    auto failureMessage = QString{};

    auto handshakeMessage = std::vector<std::byte>{};
    auto requestMessage = std::vector<std::byte>{};
    auto pollMessage = std::vector<std::byte>{};

    try {
        handshakeMessage = ChatMessage{ "handshake_from_ed" }.encode();
        requestMessage = RequestRenderMessage{ renderWidth, renderHeight, std::move( sceneObj ), camera }.encode();
        pollMessage = ChatMessage{ RenderMessageType::POLL, std::string{} }.encode();
    } catch ( const std::exception &exception ) {
        failureMessage = QStringLiteral( "Cannot encode render request: %1" ).arg( exception.what() );
    } catch ( ... ) {
        failureMessage = QStringLiteral( "Cannot encode render request with an unknown error" );
    }

    auto status{ 0 };
    if ( failureMessage.isEmpty() ) {
        status = co_await _bridge->connect( "127.0.0.1", kRenderServerPort, TcpBridgeRetryOptions{} );
    }

    if ( status == 0 && failureMessage.isEmpty() ) {
        status = co_await _bridge->send( std::move( handshakeMessage ) );
    }

    auto handshakeComplete = false;
    if ( status == 0 && failureMessage.isEmpty() ) {
        const auto reply = co_await _bridge->read();
        status = reply._status;

        if ( status == 0 ) {
            try {
                const auto message{ RenderMessage::create( reply._payload ) };

                if ( message->type() == RenderMessageType::ERROR ) {
                    const auto *errorMessage = static_cast<ChatMessage *>( message.get() );
                    const auto error = errorMessage->message();
                    failureMessage =
                        QStringLiteral( "RT render error: %1" )
                            .arg( QString::fromUtf8( error.data(), static_cast<qsizetype>( error.size() ) ) );
                } else {
                    if ( message->type() != RenderMessageType::CHAT ) {
                        throw std::runtime_error{ "Unexpected message received during the RT handshake" };
                    }

                    const auto *handshake = static_cast<ChatMessage *>( message.get() );
                    if ( handshake->message() != "hello from rt" ) {
                        throw std::runtime_error{ "Unexpected RT handshake response" };
                    }

                    handshakeComplete = true;
                }
            } catch ( const std::exception &exception ) {
                log::error()( "RT handshake payload preview: {}", payloadPreview( reply._payload ) );
                failureMessage = QStringLiteral( "Invalid RT handshake response: %1" ).arg( exception.what() );
            } catch ( ... ) {
                failureMessage = QStringLiteral( "Invalid RT handshake response with an unknown error" );
            }
        }
    }

    if ( status == 0 && failureMessage.isEmpty() && handshakeComplete ) {
        QMetaObject::invokeMethod(
            this,
            [this]() -> void {
                //
                handleHandshakeDone();
            },
            Qt::QueuedConnection );

        status = co_await _bridge->send( std::move( requestMessage ) );
    }

    auto finalReceived = false;
    while ( status == 0 && failureMessage.isEmpty() && handshakeComplete && !finalReceived ) {
        status = co_await _bridge->send( pollMessage );

        if ( status != 0 ) {
            break;
        }

        const auto reply = co_await _bridge->read();
        status = reply._status;

        if ( status != 0 ) {
            break;
        }

        try {
            const auto message{ RenderMessage::create( reply._payload ) };

            if ( message->type() == RenderMessageType::ERROR ) {
                const auto *errorMessage = static_cast<ChatMessage *>( message.get() );
                const auto error = errorMessage->message();
                failureMessage = QStringLiteral( "RT render error: %1" )
                                     .arg( QString::fromUtf8( error.data(), static_cast<qsizetype>( error.size() ) ) );
            } else if ( message->type() == RenderMessageType::IMAGE ) {
                const auto *imageMessage = static_cast<ImageMessage *>( message.get() );

                if ( imageMessage->width() != renderWidth || imageMessage->height() != renderHeight ) {
                    throw std::runtime_error{ "Render response dimensions do not match the request" };
                }

                const auto pixels{ imageMessage->pixels() };

                auto image = QImage{ static_cast<int>( imageMessage->width() ),
                                     static_cast<int>( imageMessage->height() ), QImage::Format_Grayscale8 };

                if ( image.isNull() ) {
                    throw std::runtime_error{ "Cannot allocate the received render image" };
                }

                const auto rowSize = static_cast<size_t>( imageMessage->width() );

                for ( size_t row = 0; row < imageMessage->height(); ++row ) {
                    std::memcpy( image.scanLine( static_cast<int>( row ) ), pixels.data() + row * rowSize, rowSize );
                }

                finalReceived = imageMessage->final();

                QMetaObject::invokeMethod(
                    this,
                    [this, image = std::move( image )]() -> void {
                        //
                        handleImage( image );
                    },
                    Qt::QueuedConnection );
            } else {
                failureMessage = QStringLiteral( "Unexpected message received from the RT render server" );
            }
        } catch ( const std::exception &exception ) {
            log::error()( "RT response payload preview: {}", payloadPreview( reply._payload ) );
            failureMessage = QStringLiteral( "Invalid RT render response: %1" ).arg( exception.what() );
        } catch ( ... ) {
            failureMessage = QStringLiteral( "Invalid RT render response with an unknown error" );
        }

        if ( status == 0 && failureMessage.isEmpty() && !finalReceived ) {
            status = co_await _bridge->delay( kRenderPollDelayMilliseconds );
        }
    }

    if ( !failureMessage.isEmpty() ) {
        QMetaObject::invokeMethod(
            this,
            [this, failureMessage]() -> void {
                //
                fail( failureMessage );
            },
            Qt::QueuedConnection );
    } else if ( status != 0 ) {
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
                handleRenderComplete();
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

    log::info()( "Handshake done!" );
}

auto RenderRequest::handleImage( const QImage &image ) -> void {
    if ( _state == RenderRequestState::Idle || _state == RenderRequestState::Failed ) {
        return;
    }

    setRenderedImage( image );
}

auto RenderRequest::handleRenderComplete() -> void {
    if ( _state == RenderRequestState::Idle || _state == RenderRequestState::Failed ) {
        return;
    }

    log::info()( "RT render completed" );

    stop();

    emit renderCompleted();
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
