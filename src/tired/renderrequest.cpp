#include "tired/renderrequest.h"

#include <cstring>
#include <exception>
#include <utility>

#include <QCoreApplication>
#include <QDir>
#include <QMetaObject>
#include <QProcess>

#include <uv.h>

#include "log/log.h"
#include "tcpbridge/tcpbridge.h"

namespace tire {

RenderRequest::RenderRequest( QObject *parent )
    : QObject{ parent } {
}

RenderRequest::~RenderRequest() {
    stop();
}

void RenderRequest::start() {
    if ( _state != State::Idle && _state != State::Failed ) {
        return;
    }

    _state = State::Starting;

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

        auto callbacks = TcpBridge::Callbacks{};

        callbacks._connected = [this]() -> void {
            QMetaObject::invokeMethod( this, [this]() -> void { handleConnected(); }, Qt::QueuedConnection );
        };

        callbacks._message = [this]( TcpBridge::Message message ) -> void {
            const auto payload = QByteArray{ reinterpret_cast<const char *>( message.data() ),
                                             static_cast<qsizetype>( message.size() ) };
            QMetaObject::invokeMethod(
                this, [this, payload]() -> void { handleMessage( payload ); }, Qt::QueuedConnection );
        };

        callbacks._disconnected = [this]() -> void {
            QMetaObject::invokeMethod( this, [this]() -> void { handleDisconnected(); }, Qt::QueuedConnection );
        };

        callbacks._error = [this]( TcpBridge::Error error ) -> void {
            QMetaObject::invokeMethod(
                this,
                [this, error = std::move( error )]() -> void { handleBridgeError( error._status, error._context ); },
                Qt::QueuedConnection );
        };

        _bridge->setCallbacks( std::move( callbacks ) );

        _process = std::make_unique<QProcess>();

        QObject::connect( _process.get(), &QProcess::started, this, [this]() -> void {
            if ( _state != State::Starting || _bridge == nullptr ) {
                return;
            }

            _state = State::Connecting;
            _bridge->connect( "127.0.0.1", 45454, TcpBridge::RetryOptions{} );
        } );

        QObject::connect( _process.get(), &QProcess::errorOccurred, this,
                          [this]( QProcess::ProcessError ) -> void {
                              if ( _process == nullptr ) {
                                  return;
                              }

                              fail( QStringLiteral( "rt process error: %1" ).arg( _process->errorString() ) );
                          } );

        QObject::connect( _process.get(), &QProcess::finished, this,
                          [this]( int exitCode, QProcess::ExitStatus ) -> void {
                              if ( _state == State::Idle || _state == State::Failed ) {
                                  return;
                              }
                              fail( QStringLiteral( "rt process exited unexpectedly with code %1" ).arg( exitCode ) );
                          } );

        auto executable = QDir{ QCoreApplication::applicationDirPath() }.filePath( QStringLiteral( "rt" ) );
#ifdef Q_OS_WIN
        executable += QStringLiteral( ".exe" );
#endif
        _process->setProgram( executable );
        _process->setArguments(
            { QStringLiteral( "--serve" ), QStringLiteral( "--port" ), QStringLiteral( "45454" ) } );
        _process->start();

    } catch ( const std::exception &exception ) {
        fail( QStringLiteral( "Failed to initialize render request: %1" ).arg( exception.what() ) );
    } catch ( ... ) {
        fail( QStringLiteral( "Failed to initialize render request with an unknown error" ) );
    }
}

void RenderRequest::stop() {
    _state = State::Idle;
    cleanupSession();
}

auto RenderRequest::fail( const QString &message ) -> void {
    if ( _state == State::Idle || _state == State::Failed ) {
        return;
    }

    _state = State::Failed;

    cleanupSession();

    log::error()( "Render request failed: {}", message.toStdString() );

    emit errorOccurred( message );
}

auto RenderRequest::cleanupSession() -> void {
    if ( _bridge != nullptr ) {
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

auto RenderRequest::handleConnected() -> void {
    if ( _state != State::Connecting || _bridge == nullptr ) {
        return;
    }

    _state = State::WaitingForEcho;
    const auto expected = QByteArray{ "hello from ed" };
    auto message = TcpBridge::Message( static_cast<size_t>( expected.size() ) );
    std::memcpy( message.data(), expected.constData(), static_cast<size_t>( expected.size() ) );
    _bridge->send( std::move( message ) );
}

auto RenderRequest::handleMessage( const QByteArray &payload ) -> void {
    if ( _state != State::WaitingForEcho ) {
        return;
    }

    if ( payload != QByteArray{ "hello from ed" } ) {
        fail( QStringLiteral( "Received malformed TCP echo" ) );
        return;
    }

    log::info()( "TCP echo verified: {}", payload.toStdString() );
    stop();
    emit echoCompleted();
}

auto RenderRequest::handleDisconnected() -> void {
    if ( _state == State::Idle || _state == State::Failed ) {
        return;
    }

    fail( QStringLiteral( "TCP connection closed unexpectedly" ) );
}

auto RenderRequest::handleBridgeError( int status, const std::string &context ) -> void {
    if ( _state == State::Idle || _state == State::Failed ) {
        return;
    }

    auto message = QStringLiteral( "TCP bridge error: %1" ).arg( QString::fromStdString( context ) );
    if ( status != 0 ) {
        message += QStringLiteral( " (%1)" ).arg( uv_strerror( status ) );
    }

    fail( message );
}

}  // namespace tire
