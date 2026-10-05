#include <print>

#include <vsg/all.h>

#include <QQmlEngine>
#include <QQmlContext>

#include "ui.h"
#include "appearance.h"
#include "log/log.h"
#include "tired/tired.h"

namespace tire {

// ====================================================================
// ========== AppStateSettings ========================================
// ====================================================================

AppStateSettings::AppStateSettings( QObject* parent )
    : QObject{ parent }
    , _settings{ new QSettings{ this } } {};

auto AppStateSettings::write() -> void {
    _settings->beginGroup( "AppStateSettings" );

    _settings->setValue( "mainWindowRect", mainWindowRect() );

    _settings->setValue( "topPanelHeight", topPanelHeight() );
    _settings->setValue( "bottomPanelHeight", bottomPanelHeight() );
    _settings->setValue( "leftPanelWidth", leftPanelWidth() );
    _settings->setValue( "rightPanelWidth", rightPanelWidth() );

    _settings->setValue( "panelsGeometry", QVariant::fromValue( QList<float>{ topPanelHeight(), bottomPanelHeight(),
                                                                              leftPanelWidth(), rightPanelWidth() } ) );

    _settings->endGroup();

    _settings->sync();
}

auto AppStateSettings::restore() -> void {
    _settings->beginGroup( "AppStateSettings" );

    const auto mainWindowRect = _settings->value( "mainWindowRect", QVariant().toRect() );

    if ( mainWindowRect.isNull() ) {
        setMainWindowRect( { QPoint{ 200, 200 }, QSize{ 1900, 1800 } } );
    } else {
        setMainWindowRect( mainWindowRect.toRect() );
    }

    const auto& panelsGeometry = _settings->value( "panelsGeometry", QVariant() ).toList();

    if ( panelsGeometry.isEmpty() ) {
        // Set default size of panels.
        resetPanelsSize();
    } else {
        QList<float> floatList;
        floatList.reserve( 4 );

        std::transform( panelsGeometry.begin(), panelsGeometry.end(), std::back_inserter( floatList ),
                        []( const QVariant& v ) -> int {
                            //
                            return v.toFloat();
                        } );

        setTopPanelHeight( floatList[0] );
        setBottomPanelHeight( floatList[1] );
        setLeftPanelWidth( floatList[2] );
        setRightPanelWidth( floatList[3] );
    }

    _settings->endGroup();
}

void AppStateSettings::enlargeRightPanel( float factor ) {
    // const auto g = this->geometry();
    // const auto width = g.width();

    // const auto leftPanelWidth = static_cast<int>( static_cast<float>( width ) * _rowLayoutFactor );
    // const auto rightPanelWidth = static_cast<int>( static_cast<float>( width ) * factor );
    // _rowSplitter->setSizes( { leftPanelWidth, width - ( leftPanelWidth + rightPanelWidth ), rightPanelWidth } );
}

void AppStateSettings::enlargeLeftPanel( float factor ) {
    // const auto g = this->geometry();
    // const auto width = g.width();

    // const auto leftPanelWidth = static_cast<int>( static_cast<float>( width ) * factor );
    // const auto rightPanelWidth = static_cast<int>( static_cast<float>( width ) * _rowLayoutFactor );
    // _rowSplitter->setSizes( { leftPanelWidth, width - ( leftPanelWidth + rightPanelWidth ), rightPanelWidth } );
}

void AppStateSettings::resetPanelsSize() {
    const auto rect = this->mainWindowRect().size();
    const auto width = rect.width();
    const auto height = rect.height();

    setTopPanelHeight( static_cast<int>( static_cast<float>( height ) * _columnLayoutFactor ) );
    setBottomPanelHeight( static_cast<int>( static_cast<float>( height ) * _columnLayoutFactor ) );

    setLeftPanelWidth( static_cast<int>( static_cast<float>( width ) * _rowLayoutFactor ) );
    setRightPanelWidth( static_cast<int>( static_cast<float>( width ) * _rowLayoutFactor ) );
}

auto AppStateSettings::mainWindowRect() const -> QRect {
    return _mainWindowRect;
}

auto AppStateSettings::topPanelHeight() const -> float {
    return _topPanelHeight;
}

auto AppStateSettings::bottomPanelHeight() const -> float {
    return _bottomPanelHeight;
}

auto AppStateSettings::leftPanelWidth() const -> float {
    return _leftPanelWidth;
}

auto AppStateSettings::rightPanelWidth() const -> float {
    return _rightPanelWidth;
}

auto AppStateSettings::setMainWindowRect( const QRect& value ) -> void {
    _mainWindowRect = value;
    emit mainWindowRectChanged();
}

auto AppStateSettings::setTopPanelHeight( float value ) -> void {
    _topPanelHeight = value;
    emit topPanelHeightChanged();
}

auto AppStateSettings::setBottomPanelHeight( float value ) -> void {
    _bottomPanelHeight = value;
    emit bottomPanelHeightChanged();
}

auto AppStateSettings::setLeftPanelWidth( float value ) -> void {
    _rightPanelWidth = value;
    emit leftPanelWidthChanged();
}

auto AppStateSettings::setRightPanelWidth( float value ) -> void {
    _rightPanelWidth = value;
    emit rightPanelWidthChanged();
}

// ====================================================================
// ========== TiredUi =================================================
// ====================================================================

TiredUI::TiredUI( std::shared_ptr<VsgRender> render, QObject* parent )
    : _settings{ new AppStateSettings{ this } }
    , _engine{ engine() }
    , _context{ _engine->rootContext() }
    , _render{ render } {
    //
    qRegisterMetaType<tire::SceneObjectTypeEnum>( "SceneObjectTypeEnum" );
    qRegisterMetaType<tire::SceneObjectData>( "SceneObjectData" );
    qRegisterMetaType<tire::BoxObjectData>( "BoxObject" );
    qRegisterMetaType<tire::SphereObjectData>( "SphereObjectData" );
    qRegisterMetaType<tire::MeshObjectData>( "MeshData" );

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

    _context->setContextProperty( "appStateSettings", _settings );

    // Register UI style provider object.
    tire::Appearance::init();
    qmlRegisterSingletonInstance( "Tire", 1, 0, "Appearence", tire::Appearance::pointer() );

    qmlRegisterSingletonInstance( "Tire", 1, 0, "Tired", tire::Tired::pointer() );

    // VSG initialization.

    // Setup RenderItem update interval.
    _update.setInterval( 16 );

    // setColor( "#0c0c1c" );
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

                // Restore previuosely saved window geometry.
                _settings->restore();

                resize( _settings->mainWindowRect().size() );
                setPosition( _settings->mainWindowRect().topLeft() );

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
}

QVector2D TiredUI::mainWindowCenter() const {
    const auto g = this->geometry();
    return { static_cast<float>( g.center().x() ), static_cast<float>( g.center().y() ) };
}

void TiredUI::quitApplication() {
    QGuiApplication::quit();
}

void TiredUI::closeEvent( QCloseEvent* event ) {
    _settings->write();

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

}  // namespace tire
