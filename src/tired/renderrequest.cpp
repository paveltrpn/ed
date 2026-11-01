#include "tired/renderrequest.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <string_view>
#include <vector>

#include <QCoreApplication>
#include <QDir>
#include <QMetaObject>
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
}

RenderRequest::~RenderRequest() {
    stop();

    if ( !_sessionTask.empty() ) {
        std::unique_lock lock{ _doneMutex };
        _doneCondition.wait( lock, [this]() -> bool {
            //
            return _sessionDone;
        } );

        // Release the bridge, joining the event loop thread, before the session frame
        // is freed: the loop may still be unwinding through the frame when it signals
        // finish, and a freed frame must never be on a live stack.
        _bridge.reset();
    }

    _sessionTask = Task<void>{};
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
        _sessionDone = false;

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

auto RenderRequest::runSession() -> Task<void> {
    auto *bridge = _bridge.get();

    auto status = co_await bridge->connect( "127.0.0.1", kRenderServerPort, TcpBridgeRetryOptions{} );

    if ( status == 0 ) {
        status = co_await bridge->send( messageFromString( "handshake_from_ed" ) );
    }

    while ( status == 0 ) {
        const auto reply = co_await bridge->read();
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

        status = co_await bridge->send( messageFromString( "dummy data to rt" ) );
    }

    while ( status == 0 ) {
        const auto reply = co_await bridge->read();
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

    co_await SessionEndAwaitable{ this };

    co_return;
}

auto RenderRequest::SessionEndAwaitable::await_suspend( std::coroutine_handle<> ) noexcept -> void {
    // Never resumed. The GUI thread destroys the quiesced frame via endSession(),
    // while the destructor waits on the condition variable instead.
    QMetaObject::invokeMethod(
        _owner,
        [owner = _owner]() -> void {
            //
            owner->endSession();
        },
        Qt::QueuedConnection );

    {
        std::scoped_lock lock{ _owner->_doneMutex };
        _owner->_sessionDone = true;
    }

    _owner->_doneCondition.notify_all();
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
