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

TiredUI::TiredUI( QObject* parent )
    : _settings{ new QSettings{ this } }
    , _engine{ new QQmlEngine{ this } }
    , _context{ _engine->rootContext() }
    , _columnSplitter{ new QSplitter{ this } }
    , _rowSplitter{ new QSplitter{ this } }
    , _topPanel{ new QQuickWidget{ _engine, this } }
    , _leftPanel{ new QQuickWidget{ _engine, this } }
    , _bottomPanel{ new QQuickWidget{ _engine, this } }
    , _rightPanel{ new QQuickWidget{ _engine, this } } {
    registerTypes();

    // Set empty window title displayed on native decoration.
    setWindowTitle( " " );

    // Remove native decoration.
    // setWindowFlags( Qt::FramelessWindowHint );

    // Set transparent main window to use qml defined application
    // specific decoration.
    // setAttribute( Qt::WA_TranslucentBackground );

    // "By default, when you pass a QObject* via setContextProperty, the C++ side retains
    // ownership (the QML engine will not delete it)."
    //
    // Use this object for main window position and size (in particular).
    _context->setContextProperty( "mainWindow", this );

    _engine->addImageProvider( "TiredImageProvider", new TiredImageProvider{} );

    // Register UI style provider object.
    tire::Appearance::init();
    qmlRegisterSingletonInstance( "Tire", 1, 0, "Appearence", tire::Appearance::pointer() );

    // VSG initialization.
    auto windowTraits = vsg::WindowTraits::create();
    windowTraits->vulkanVersion = VK_MAKE_API_VERSION( 0, 1, 4, 0 );
    windowTraits->deviceExtensionNames.push_back( "VK_EXT_extended_dynamic_state3" );

    try {
        _vsgWidget = new VsgWidget( windowTraits );
        _vsgWidget->initializeWindow();

        const auto clearColor = QColor{ tire::Appearance::instance().getColor( "clear_color" ) };
        _vsgWidget->windowAdapter()->clearColor().set( clearColor.redF(), clearColor.greenF(), clearColor.blueF(),
                                                       1.0f );

        tire::Tired::init( _vsgWidget->windowAdapter(), _vsgWidget->viewer(), windowTraits->width,
                           windowTraits->height );

        connect( tire::Tired::pointer(), &tire::Tired::restorePanelsSize, this, [this]() {
            //
            this->resetPanelsSize();
        } );

    } catch ( vsg::Exception& e ) {
        log::fatal()( "{}", e.message );
    }

    qmlRegisterSingletonInstance( "Tire", 1, 0, "Tired", tire::Tired::pointer() );

    // Qt widgets initialization.
    _topPanel->setSource( QUrl::fromLocalFile( "../src/ui/qml/panels/TopPanel.qml" ) );
    _topPanel->setResizeMode( QQuickWidget::SizeRootObjectToView );

    _leftPanel->setSource( QUrl::fromLocalFile( "../src/ui/qml/panels/LeftPanel.qml" ) );
    _leftPanel->setResizeMode( QQuickWidget::SizeRootObjectToView );

    _bottomPanel->setSource( QUrl::fromLocalFile( "../src/ui/qml/panels/BottomPanel.qml" ) );
    _bottomPanel->setResizeMode( QQuickWidget::SizeRootObjectToView );

    _rightPanel->setSource( QUrl::fromLocalFile( "../src/ui/qml/panels/RightPanel.qml" ) );
    _rightPanel->setResizeMode( QQuickWidget::SizeRootObjectToView );

    // Set qml QQuickWidgets conteiners trnsparent background color.
    _topPanel->setClearColor( Qt::transparent );
    _leftPanel->setClearColor( Qt::transparent );
    _bottomPanel->setClearColor( Qt::transparent );
    _rightPanel->setClearColor( Qt::transparent );

    const auto splitterBorderColor = tire::Appearance::instance().getColor( "background" );

    auto centralWidget = new QWidget{ this };
    setCentralWidget( centralWidget );

    auto* mainColumnLayout = new QVBoxLayout{};
    mainColumnLayout->setContentsMargins( 0, 0, 0, 0 );
    centralWidget->setLayout( mainColumnLayout );

    _columnSplitter->setOrientation( Qt::Vertical );
    _columnSplitter->setStyleSheet(
        QString{ "QSplitter::handle { background-color:  %1; }" }.arg( splitterBorderColor ) );
    _columnSplitter->setHandleWidth( 2 );

    auto* hLayout = new QHBoxLayout{};
    hLayout->setContentsMargins( 0, 0, 0, 0 );

    auto middleElementsWidget = new QWidget{ this };
    middleElementsWidget->setLayout( hLayout );

    _columnSplitter->addWidget( _topPanel );
    _columnSplitter->addWidget( middleElementsWidget );
    _columnSplitter->addWidget( _bottomPanel );

    mainColumnLayout->addWidget( _columnSplitter );

    _rowSplitter->setOrientation( Qt::Horizontal );
    _rowSplitter->setStyleSheet( QString{ "QSplitter::handle { background-color:  %1; }" }.arg( splitterBorderColor ) );
    _rowSplitter->setHandleWidth( 2 );

    _rowSplitter->addWidget( _leftPanel );
    _rowSplitter->addWidget( _vsgWidget );
    _rowSplitter->addWidget( _rightPanel );

    // Restore previuosely saved window geometry.
    readSettings();

    // Set default size of panels.
    resetPanelsSize();

    hLayout->addWidget( _rowSplitter );
}

auto TiredUI::writeSettings() -> void {
    _settings->beginGroup( "MainWindow" );
    _settings->setValue( "geometry", saveGeometry() );
    _settings->endGroup();

    _settings->sync();
}

auto TiredUI::readSettings() -> void {
    _settings->beginGroup( "MainWindow" );

    const auto geometry = _settings->value( "geometry", QByteArray() ).toByteArray();
    if ( geometry.isEmpty() ) {
        setGeometry( 200, 200, 1024, 768 );
    } else {
        restoreGeometry( geometry );
    }

    _settings->endGroup();
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

void TiredUI::enlargeRightPanel( float factor ) {
    const auto g = this->geometry();
    const auto width = g.width();

    const auto leftPanelWidth = static_cast<int>( width * 0.11f );
    const auto rightPanelWidth = static_cast<int>( width * factor );
    _rowSplitter->setSizes( { leftPanelWidth, width - ( leftPanelWidth + rightPanelWidth ), rightPanelWidth } );
}

void TiredUI::enlargeLeftPanel( float factor ) {
    const auto g = this->geometry();
    const auto width = g.width();

    const auto leftPanelWidth = static_cast<int>( width * factor );
    const auto rightPanelWidth = static_cast<int>( width * 0.11f );
    _rowSplitter->setSizes( { leftPanelWidth, width - ( leftPanelWidth + rightPanelWidth ), rightPanelWidth } );
}

void TiredUI::resetPanelsSize() {
    const auto g = this->geometry();
    const auto width = g.width();
    const auto height = g.height();

    const auto topPanelHeight = static_cast<int>( height * 0.07f );
    const auto bottomPanelHeight = static_cast<int>( height * 0.07f );
    _columnSplitter->setSizes( { topPanelHeight, height - ( topPanelHeight + bottomPanelHeight ), bottomPanelHeight } );

    const auto leftPanelWidth = static_cast<int>( width * 0.11f );
    const auto rightPanelWidth = static_cast<int>( width * 0.11f );
    _rowSplitter->setSizes( { leftPanelWidth, width - ( leftPanelWidth + rightPanelWidth ), rightPanelWidth } );
}

auto TiredUI::registerTypes() -> void {
    qRegisterMetaType<tire::SceneObjectTypeEnum>( "SceneObjectTypeEnum" );

    qRegisterMetaType<tire::SceneObjectData>( "SceneObjectData" );
    qRegisterMetaType<tire::BoxObjectData>( "BoxObject" );
    qRegisterMetaType<tire::SphereObjectData>( "SphereObjectData" );
    qRegisterMetaType<tire::MeshObjectData>( "MeshData" );
}

}  // namespace tire