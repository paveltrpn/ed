#include <print>

#include <vsg/all.h>

#include <QQmlContext>
#include <QHBoxLayout>
#include <QSplitter>
#include <QWidget>
#include <QApplication>

#include "ui.h"
#include "log/log.h"
#include "tired/tired.h"
#include "config/config.h"

namespace tire {

// ====================================================================
// ========== TiredUi =================================================
// ====================================================================

TiredUI::TiredUI( std::shared_ptr<VsgRender> render, QObject* parent )
    : _settings{ new QSettings{ this } }
    , _engine{ new QQmlEngine{ this } }
    , _context{ _engine->rootContext() }
    , _render{ render } {
    registerTypes();

    // Register RenderItem qml type. We instatiate item of
    // of this type only once in main.qml
    qmlRegisterType<tire::RenderItem>( "Tire", 1, 0, "Render" );

    // Set empty window title displayed on native decoration.
    // setWindowTitle( " " );

    // Remove native decoration.
    // setWindowFlags( Qt::FramelessWindowHint );

    // Set transparent main window to use qml defined application
    // specific decoration.
    // setAttribute( Qt::WA_TranslucentBackground );

    _engine->addImageProvider( "TiredImageProvider", new TiredImageProvider{} );

    // Register UI style provider object.
    tire::Appearance::init();
    qmlRegisterSingletonInstance( "Tire", 1, 0, "Appearence", tire::Appearance::pointer() );

    _engine->addImageProvider( "TiredImageProvider", new TiredImageProvider{} );

    qmlRegisterSingletonInstance( "Tire", 1, 0, "Tired", tire::Tired::pointer() );

    // VSG initialization.

    // Setup RenderItem update interval.
    _update.setInterval( 16 );

    setColor( "#0c0c1c" );
    // setFlags( Qt::Window | Qt::FramelessWindowHint );

    // Schedule actions on main component loading.
    connect( this, &QQuickView::statusChanged, this, [this]( QQuickView::Status status ) {
        switch ( status ) {
            case QQuickView::Error: {
                break;
            }

            case QQuickView::Loading: {
                break;
            }

            case QQuickView::Ready: {
                // Get main renderer item handle from QML.
                // It istatiates only once in main.qml.
                _renderItemHandle = rootObject()->findChild<RenderItem*>();
                if ( !_renderItemHandle ) {
                    std::println( "can't acquire renderer handle!" );
                    std::terminate();
                }

                _renderItemHandle->setVsgRender( _render );

                _render->setExtent( { static_cast<uint32_t>( _renderItemHandle->width() ),
                                      static_cast<uint32_t>( _renderItemHandle->height() ) } );

                _render->setNeedResize( true );

                // Start update timer.
                _update.start();

                // Call updateWindow to redraw qml item.
                connect( &_update, &QTimer::timeout, _renderItemHandle, &RenderItem::updateWindow );

                std::println( "Render QML component ready." );
                break;
            }

            case QQuickView::Null: {
                break;
            }
        }
    } );

    connect( _engine, &QQmlEngine::quit, this, [this]() -> void {
        //
        QGuiApplication::quit();
    } );

    // Restore previuosely saved window geometry.
    // readSettings();
}

auto TiredUI::writeSettings() -> void {
    // _settings->beginGroup( "MainWindow" );
    // _settings->setValue( "geometry", saveGeometry() );
    // _settings->endGroup();

    // const auto& rowSizes = _rowSplitter->sizes();
    // const auto& colSizes = _columnSplitter->sizes();

    // _settings->beginGroup( "PanelsLayout" );
    // _settings->setValue( "rowSizes", QVariant::fromValue( rowSizes ) );
    // _settings->setValue( "columnSizes", QVariant::fromValue( colSizes ) );
    // _settings->endGroup();

    // _settings->sync();
}

auto TiredUI::readSettings() -> void {
    // _settings->beginGroup( "MainWindow" );

    // const auto geometry = _settings->value( "geometry", QByteArray() ).toByteArray();

    // if ( geometry.isEmpty() ) {
    //     setGeometry( 200, 200, 1024, 768 );
    // } else {
    //     restoreGeometry( geometry );
    // }

    // _settings->endGroup();

    // _settings->beginGroup( "PanelsLayout" );

    // const auto& rowSizes = _settings->value( "rowSizes", QVariant() ).toList();
    // const auto& colSizes = _settings->value( "columnSizes", QVariant() ).toList();

    // if ( rowSizes.isEmpty() || colSizes.isEmpty() ) {
    //     // Set default size of panels.
    //     resetPanelsSize();
    // } else {
    //     {
    //         QList<int> intList;
    //         intList.reserve( 3 );

    //         std::transform( rowSizes.begin(), rowSizes.end(), std::back_inserter( intList ),
    //                         []( const QVariant& v ) -> int {
    //                             //
    //                             return v.toInt();
    //                         } );

    //         _rowSplitter->setSizes( intList );
    //     }

    //     {
    //         QList<int> intList;
    //         intList.reserve( 3 );

    //         std::transform( colSizes.begin(), colSizes.end(), std::back_inserter( intList ),
    //                         []( const QVariant& v ) -> int {
    //                             //
    //                             return v.toInt();
    //                         } );

    //         _columnSplitter->setSizes( intList );
    //     }
    // }

    // _settings->endGroup();
}

QVector2D TiredUI::mainWindowCenter() const {
    const auto g = this->geometry();
    return { static_cast<float>( g.center().x() ), static_cast<float>( g.center().y() ) };
}

void TiredUI::quitApplication() {
    QApplication::quit();
}

void TiredUI::closeEvent( QCloseEvent* event ) {
    writeSettings();

    log::info()( "close event handled!" );
}

void TiredUI::moveWindow() {
    // this->windowHandle()->startSystemMove();
}

void TiredUI::resizeWindow( int edge ) {
    // const auto e = static_cast<Qt::Edge>( edge );
    // this->windowHandle()->startSystemResize( e );
}

void TiredUI::keyPressEvent( QKeyEvent* ev ) {
    _renderItemHandle->keyPressEvent( ev );
    QQuickView::keyPressEvent( ev );
}

void TiredUI::keyReleaseEvent( QKeyEvent* ev ) {
    _renderItemHandle->keyReleaseEvent( ev );
    QQuickView::keyReleaseEvent( ev );
}

void TiredUI::mouseMoveEvent( QMouseEvent* ev ) {
    _renderItemHandle->mouseMoveEvent( ev );
    QQuickView::mouseMoveEvent( ev );
}

void TiredUI::mousePressEvent( QMouseEvent* ev ) {
    _renderItemHandle->mousePressEvent( ev );
    QQuickView::mousePressEvent( ev );
}

void TiredUI::mouseReleaseEvent( QMouseEvent* ev ) {
    _renderItemHandle->mousePressEvent( ev );
    QQuickView::mouseReleaseEvent( ev );
}

void TiredUI::resizeEvent( QResizeEvent* ev ) {
    QQuickView::resizeEvent( ev );
}

void TiredUI::enlargeRightPanel( float factor ) {
    // const auto g = this->geometry();
    // const auto width = g.width();

    // const auto leftPanelWidth = static_cast<int>( static_cast<float>( width ) * _rowLayoutFactor );
    // const auto rightPanelWidth = static_cast<int>( static_cast<float>( width ) * factor );
    // _rowSplitter->setSizes( { leftPanelWidth, width - ( leftPanelWidth + rightPanelWidth ), rightPanelWidth } );
}

void TiredUI::enlargeLeftPanel( float factor ) {
    // const auto g = this->geometry();
    // const auto width = g.width();

    // const auto leftPanelWidth = static_cast<int>( static_cast<float>( width ) * factor );
    // const auto rightPanelWidth = static_cast<int>( static_cast<float>( width ) * _rowLayoutFactor );
    // _rowSplitter->setSizes( { leftPanelWidth, width - ( leftPanelWidth + rightPanelWidth ), rightPanelWidth } );
}

void TiredUI::resetPanelsSize() {
    // const auto g = this->geometry();
    // const auto width = g.width();
    // const auto height = g.height();

    // const auto topPanelHeight = static_cast<int>( static_cast<float>( height ) * _columnLayoutFactor );
    // const auto bottomPanelHeight = static_cast<int>( static_cast<float>( height ) * _columnLayoutFactor );
    // _columnSplitter->setSizes( { topPanelHeight, height - ( topPanelHeight + bottomPanelHeight ), bottomPanelHeight } );

    // const int leftPanelWidth = static_cast<int>( static_cast<float>( width ) * _rowLayoutFactor );
    // const int rightPanelWidth = static_cast<int>( static_cast<float>( width ) * _rowLayoutFactor );
    // _rowSplitter->setSizes( { leftPanelWidth, width - ( leftPanelWidth + rightPanelWidth ), rightPanelWidth } );
}

auto TiredUI::registerTypes() -> void {
    qRegisterMetaType<tire::SceneObjectTypeEnum>( "SceneObjectTypeEnum" );

    qRegisterMetaType<tire::SceneObjectData>( "SceneObjectData" );
    qRegisterMetaType<tire::BoxObjectData>( "BoxObject" );
    qRegisterMetaType<tire::SphereObjectData>( "SphereObjectData" );
    qRegisterMetaType<tire::MeshObjectData>( "MeshData" );
}

}  // namespace tire
