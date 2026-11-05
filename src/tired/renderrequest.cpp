#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <format>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <QCoreApplication>
#include <QMetaObject>
#include <QProcess>
#include <QDir>

#include <uv.h>

#include "tired/renderrequest.h"

#include "log/log.h"
#include "tcpbridge/renderprotocol.h"
#include "tcpbridge/tcpbridge.h"
#include "tired/subgraph/scene.h"

namespace {

constexpr uint16_t kRenderServerPort{ 45454 };
constexpr uint64_t kRenderPollDelayMilliseconds{ 200 };

enum class RenderSessionState {
    CONNECTING,
    SENDING_HANDSHAKE,
    RECEIVING_HANDSHAKE,
    SENDING_REQUEST,
    SENDING_POLL,
    RECEIVING_IMAGE,
    DELAYING_POLL,
    COMPLETE,
    FAILED,
    TRANSPORT_FAILED
};

}  // namespace

namespace tire {

RenderRequest::RenderRequest( vsg::ref_ptr<vsg::Camera> camera, SceneUIProxy *scene, QObject *parent )
    : QObject{ parent }
    , _camera{ std::move( camera ) }
    , _scene{ scene } {
    _renderedImage = QImage{ _width, _height, QImage::Format_RGB32 };
    _renderedImage.fill( Qt::black );
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
    if ( _requestRenderState != RenderRequestState::IDLE && _requestRenderState != RenderRequestState::FAILED ) {
        return;
    }

    if ( !_sessionTask.empty() ) {
        // A previous session is still being wound down on the event loop thread.
        return;
    }

    setRequestRenderState( RenderRequestState::STARTING );

    auto *application = QCoreApplication::instance();

    if ( !_quitHandlerConnected ) {
        QObject::connect( application, &QCoreApplication::aboutToQuit, this, &RenderRequest::stop );
        _quitHandlerConnected = true;
    }

    try {
        const auto lookAt = _camera->viewMatrix.cast<vsg::LookAt>();
        const auto perspective = _camera->projectionMatrix.cast<vsg::Perspective>();

        log::info()( "Begin render." );

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
            log::info()( "Remote render process started." );

            if ( _requestRenderState != RenderRequestState::STARTING || _bridge == nullptr ) {
                return;
            }

            setRequestRenderState( RenderRequestState::CONNECTING );

            _sessionTask = runSession( std::move( _pendingSceneObj ), _pendingCamera );
        } );

        QObject::connect( _process.get(), &QProcess::errorOccurred, this, [this]( QProcess::ProcessError ) -> void {
            //
            if ( _process == nullptr ) {
                return;
            }

            fail( std::format( "rt process error: {}", _process->errorString().toStdString() ) );
        } );

        QObject::connect( _process.get(), &QProcess::finished, this,
                          [this]( int exitCode, QProcess::ExitStatus ) -> void {
                              //
                              log::info()( "Remote render process finished." );

                              if ( _requestRenderState == RenderRequestState::IDLE ||
                                   _requestRenderState == RenderRequestState::FAILED ) {
                                  return;
                              }

                              fail( std::format( "rt process exited unexpectedly with code {}", exitCode ) );
                          } );

        auto executable = QDir{ QCoreApplication::applicationDirPath() }.filePath( QStringLiteral( "rt" ) );

        _process->setProgram( executable );
        _process->setArguments( { QStringLiteral( "--serve" ), QStringLiteral( "--port" ),
                                  QStringLiteral( "%1" ).arg( kRenderServerPort ) } );
        _process->start();

    } catch ( const std::exception &exception ) {
        fail( std::format( "Failed to initialize render request: {}", exception.what() ) );
    }
}

void RenderRequest::stop() {
    setRequestRenderState( RenderRequestState::IDLE );
    cleanupSession();
}

bool RenderRequest::requestPending() const {
    return _requestRenderState != RenderRequestState::IDLE && _requestRenderState != RenderRequestState::FAILED;
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

    auto state{ RenderSessionState::CONNECTING };

    auto failureMessage = std::string{};
    auto failureStatus{ 0 };

    const auto pollMessage{ ChatMessage{ RenderMessageType::POLL, std::string{} }.encode() };

    while ( state != RenderSessionState::COMPLETE && state != RenderSessionState::FAILED &&
            state != RenderSessionState::TRANSPORT_FAILED ) {
        switch ( state ) {
            case RenderSessionState::CONNECTING: {
                const auto status =
                    co_await _bridge->connect( "127.0.0.1", kRenderServerPort, TcpBridgeRetryOptions{} );
                if ( status != 0 ) {
                    failureStatus = status;
                    state = RenderSessionState::TRANSPORT_FAILED;
                } else {
                    state = RenderSessionState::SENDING_HANDSHAKE;
                }

                break;
            }

            case RenderSessionState::SENDING_HANDSHAKE: {
                auto handshakeMessage = ChatMessage{ "handshake_from_ed" }.encode();
                const auto status = co_await _bridge->send( std::move( handshakeMessage ) );
                if ( status != 0 ) {
                    failureStatus = status;
                    state = RenderSessionState::TRANSPORT_FAILED;
                } else {
                    state = RenderSessionState::RECEIVING_HANDSHAKE;
                }

                break;
            }

            case RenderSessionState::RECEIVING_HANDSHAKE: {
                const auto reply = co_await _bridge->read();
                if ( reply._status != 0 ) {
                    failureStatus = reply._status;
                    state = RenderSessionState::TRANSPORT_FAILED;
                    break;
                }

                try {
                    const auto message{ RenderMessage::create( reply._payload ) };

                    if ( message->type() == RenderMessageType::ERROR ) {
                        const auto *errorMessage = static_cast<ChatMessage *>( message.get() );
                        const auto error = errorMessage->message();
                        failureMessage = std::format( "RT render error: {}", error );
                        state = RenderSessionState::FAILED;
                    } else {
                        if ( message->type() != RenderMessageType::CHAT ) {
                            throw std::runtime_error{ "Unexpected message received during the RT handshake" };
                        }

                        const auto *handshake = static_cast<ChatMessage *>( message.get() );
                        if ( handshake->message() != "hello_from_rt" ) {
                            throw std::runtime_error{ "Unexpected RT handshake response" };
                        }

                        QMetaObject::invokeMethod(
                            this,
                            [this]() -> void {
                                //
                                handleHandshakeDone();
                            },
                            Qt::QueuedConnection );

                        log::info()( "Handshake done, send render request." );

                        state = RenderSessionState::SENDING_REQUEST;
                    }
                } catch ( const std::exception &exception ) {
                    failureMessage = std::format( "Invalid RT handshake response: {}", exception.what() );
                    state = RenderSessionState::FAILED;
                }

                break;
            }

            case RenderSessionState::SENDING_REQUEST: {
                auto requestMessage =
                    RequestRenderMessage{ renderWidth, renderHeight, std::move( sceneObj ), camera }.encode();
                const auto status = co_await _bridge->send( std::move( requestMessage ) );
                if ( status != 0 ) {
                    failureStatus = status;
                    state = RenderSessionState::TRANSPORT_FAILED;
                } else {
                    log::info()( "Request send done, poll images..." );

                    state = RenderSessionState::SENDING_POLL;
                }

                break;
            }

            case RenderSessionState::SENDING_POLL: {
                const auto status = co_await _bridge->send( pollMessage );
                if ( status != 0 ) {
                    failureStatus = status;
                    state = RenderSessionState::TRANSPORT_FAILED;
                } else {
                    state = RenderSessionState::RECEIVING_IMAGE;
                }

                break;
            }

            case RenderSessionState::RECEIVING_IMAGE: {
                const auto reply = co_await _bridge->read();
                if ( reply._status != 0 ) {
                    failureStatus = reply._status;
                    state = RenderSessionState::TRANSPORT_FAILED;
                    break;
                }

                try {
                    const auto message{ RenderMessage::create( reply._payload ) };

                    switch ( message->type() ) {
                        case RenderMessageType::ERROR: {
                            const auto *errorMessage = static_cast<ChatMessage *>( message.get() );
                            const auto error = errorMessage->message();
                            failureMessage = std::format( "RT render error: {}", error );
                            state = RenderSessionState::FAILED;

                            break;
                        }

                        case RenderMessageType::IMAGE: {
                            const auto *imageMessage = static_cast<ImageMessage *>( message.get() );

                            if ( imageMessage->width() != renderWidth || imageMessage->height() != renderHeight ) {
                                throw std::runtime_error{ "Render response dimensions do not match the request" };
                            }

                            const auto pixels{ imageMessage->pixels() };

                            auto image =
                                QImage{ static_cast<int>( imageMessage->width() ),
                                        static_cast<int>( imageMessage->height() ), QImage::Format_Grayscale8 };

                            if ( image.isNull() ) {
                                throw std::runtime_error{ "Cannot allocate the received render image" };
                            }

                            const auto rowSize = static_cast<size_t>( imageMessage->width() );

                            for ( size_t row = 0; row < imageMessage->height(); ++row ) {
                                std::memcpy( image.scanLine( static_cast<int>( row ) ), pixels.data() + row * rowSize,
                                             rowSize );
                            }

                            const auto isFinal{ imageMessage->final() };

                            QMetaObject::invokeMethod(
                                this,
                                [this, image = std::move( image )]() -> void {
                                    //
                                    handleImage( image );
                                },
                                Qt::QueuedConnection );

                            if ( isFinal ) {
                                log::info()( "Receive final image." );

                                state = RenderSessionState::COMPLETE;
                            } else {
                                state = RenderSessionState::DELAYING_POLL;
                            }

                            break;
                        }

                        default: {
                            failureMessage = "Unexpected message received from the RT render server";

                            state = RenderSessionState::FAILED;
                            break;
                        }
                    }
                } catch ( const std::exception &exception ) {
                    failureMessage = std::format( "Invalid RT render response: {}", exception.what() );

                    state = RenderSessionState::FAILED;
                }

                break;
            }

            case RenderSessionState::DELAYING_POLL: {
                const auto status = co_await _bridge->delay( kRenderPollDelayMilliseconds );
                if ( status != 0 ) {
                    failureStatus = status;
                    state = RenderSessionState::TRANSPORT_FAILED;
                } else {
                    state = RenderSessionState::SENDING_POLL;
                }

                break;
            }

            case RenderSessionState::COMPLETE:
            case RenderSessionState::FAILED:
            case RenderSessionState::TRANSPORT_FAILED: {
                break;
            }
        }
    }

    switch ( state ) {
        case RenderSessionState::COMPLETE: {
            log::info()( "Render complete." );

            QMetaObject::invokeMethod(
                this,
                [this]() -> void {
                    //
                    handleRenderComplete();
                },
                Qt::QueuedConnection );

            break;
        }

        case RenderSessionState::FAILED: {
            log::warning()( "Render failed due to RenderSessionState::FAILED." );

            QMetaObject::invokeMethod(
                this,
                [this, failureMessage]() -> void {
                    //
                    fail( failureMessage );
                },
                Qt::QueuedConnection );

            break;
        }

        case RenderSessionState::TRANSPORT_FAILED: {
            log::warning()( "Render failed due to RenderSessionState::TRANSPORT_FAILED." );

            QMetaObject::invokeMethod(
                this,
                [this, failureStatus]() -> void {
                    //
                    handleFailure( failureStatus );
                },
                Qt::QueuedConnection );

            break;
        }

        case RenderSessionState::CONNECTING:
        case RenderSessionState::SENDING_HANDSHAKE:
        case RenderSessionState::RECEIVING_HANDSHAKE:
        case RenderSessionState::SENDING_REQUEST:
        case RenderSessionState::SENDING_POLL:
        case RenderSessionState::RECEIVING_IMAGE:
        case RenderSessionState::DELAYING_POLL: {
            break;
        }
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
    if ( _requestRenderState != RenderRequestState::CONNECTING ) {
        return;
    }

    setRequestRenderState( RenderRequestState::WAITING_FOR_REPLY );
}

auto RenderRequest::handleImage( const QImage &image ) -> void {
    if ( _requestRenderState == RenderRequestState::IDLE || _requestRenderState == RenderRequestState::FAILED ) {
        return;
    }

    setRenderedImage( image );
}

auto RenderRequest::handleRenderComplete() -> void {
    if ( _requestRenderState == RenderRequestState::IDLE || _requestRenderState == RenderRequestState::FAILED ) {
        return;
    }

    stop();

    emit renderCompleted();
}

auto RenderRequest::handleFailure( int status ) -> void {
    if ( _requestRenderState == RenderRequestState::IDLE || _requestRenderState == RenderRequestState::FAILED ) {
        return;
    }

    if ( status == UV_EOF ) {
        fail( "TCP connection closed unexpectedly" );
        return;
    }

    auto message = std::string{ "TCP bridge error" };

    if ( status != UV_ECANCELED ) {
        message += std::format( ": {}", uv_strerror( status ) );
    }

    fail( message );
}

auto RenderRequest::setRequestRenderState( RenderRequestState state ) -> void {
    if ( _requestRenderState == state ) {
        return;
    }

    _requestRenderState = state;

    emit requestPendingChanged();
}

auto RenderRequest::fail( const std::string &message ) -> void {
    if ( _requestRenderState == RenderRequestState::IDLE || _requestRenderState == RenderRequestState::FAILED ) {
        return;
    }

    setRequestRenderState( RenderRequestState::FAILED );

    cleanupSession();

    log::error()( "Render request failed: {}", message );

    emit errorOccurred( message );
}

auto RenderRequest::cleanupSession() -> void {
    if ( _bridge != nullptr && _sessionTask.empty() ) {
        _bridge->close();
        _bridge.reset();
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
