#include <vsg/all.h>

#include <QHBoxLayout>
#include <QSplitter>
#include <QWidget>
#include <QApplication>

#include "ui.h"
#include "log/log.h"
#include "config/config.h"

namespace tire {

// ====================================================================
// ========== TiredUi =================================================
// ====================================================================

TiredUI::TiredUI( QObject* parent )
    : _tired{ new tire::Tired{ this } }
    , _settings{ new QSettings{ this } }
    , _engine{ new QQmlEngine{ this } }
    , _context{ _engine->rootContext() }
    , _topPanel{ new QQuickWidget{ _engine, this } }
    , _leftPanel{ new QQuickWidget{ _engine, this } }
    , _bottomPanel{ new QQuickWidget{ _engine, this } }
    , _rightPanel{ new QQuickWidget{ _engine, this } }
    , _theme{ new Appearance{ this } } {
    //
    Config::init( "assets/config.json" );

    const auto [windowWidth, windowHeight] = readSettings();

    _tired->registerTypes();

    _engine->addImageProvider( "TiredImageProvider", new TiredImageProvider{} );

    auto windowTraits = vsg::WindowTraits::create();
    windowTraits->windowTitle = "ed";
    windowTraits->vulkanVersion = VK_MAKE_API_VERSION( 0, 1, 4, 0 );
    // windowTraits->fullscreen = true;

    qmlRegisterSingletonInstance( "Tire", 1, 0, "Appearence", _theme );
    qmlRegisterSingletonInstance( "Tire", 1, 0, "Tired", _tired );

    // Use this object for main window position and size (in particular).
    qmlRegisterSingletonInstance( "Tire", 1, 0, "MainWindow", this );

    // Remove native decoration.
    setWindowFlags( Qt::FramelessWindowHint );

    // Set transparent main window to use qml defined application
    // specific decoration.
    setAttribute( Qt::WA_TranslucentBackground );

    // VSG initialization.
    _vsgWidget = new VsgWidget( _tired->viewer(), windowTraits );
    _vsgWidget->initializeWindow();

    const auto clearColor = QColor{ _theme->getColor( "clear_color" ) };
    _vsgWidget->getWindowAdapter()->clearColor().set( clearColor.redF(), clearColor.greenF(), clearColor.blueF(),
                                                      1.0f );

    _tired->init( _vsgWidget, windowTraits->width, windowTraits->height );

    _topPanel->setSource( QUrl::fromLocalFile( "../src/ed/ui/qml/panels/TopPanel.qml" ) );
    _topPanel->setResizeMode( QQuickWidget::SizeRootObjectToView );

    _leftPanel->setSource( QUrl::fromLocalFile( "../src/ed/ui/qml/panels/LeftPanel.qml" ) );
    _leftPanel->setResizeMode( QQuickWidget::SizeRootObjectToView );

    _bottomPanel->setSource( QUrl::fromLocalFile( "../src/ed/ui/qml/panels/BottomPanel.qml" ) );
    _bottomPanel->setResizeMode( QQuickWidget::SizeRootObjectToView );

    _rightPanel->setSource( QUrl::fromLocalFile( "../src/ed/ui/qml/panels/RightPanel.qml" ) );
    _rightPanel->setResizeMode( QQuickWidget::SizeRootObjectToView );

    // Set qml QQuickWidgets conteiners trnsparent background color.
    _topPanel->setClearColor( Qt::transparent );
    _leftPanel->setClearColor( Qt::transparent );
    _bottomPanel->setClearColor( Qt::transparent );
    _rightPanel->setClearColor( Qt::transparent );

    const auto splitterBorderColor = _theme->getColor( "background" );
    const auto splitterHandleWidth = _theme->getUnit( "quarter" );

    auto centralWidget = new QWidget{ this };
    setCentralWidget( centralWidget );

    auto* mainColumnLayout = new QVBoxLayout{};
    mainColumnLayout->setContentsMargins( 0, 0, 0, 0 );
    centralWidget->setLayout( mainColumnLayout );

    auto* mainColumnSplitter = new QSplitter{ this };
    mainColumnSplitter->setOrientation( Qt::Vertical );
    mainColumnSplitter->setStyleSheet(
        QString{ "QSplitter::handle { background-color:  %1; }" }.arg( splitterBorderColor ) );
    mainColumnSplitter->setHandleWidth( splitterHandleWidth );

    auto* hLayout = new QHBoxLayout{};
    hLayout->setContentsMargins( 0, 0, 0, 0 );

    auto middleElementsWidget = new QWidget{ this };
    middleElementsWidget->setLayout( hLayout );

    mainColumnSplitter->addWidget( _topPanel );
    mainColumnSplitter->addWidget( middleElementsWidget );
    mainColumnSplitter->addWidget( _bottomPanel );

    const auto topPanelHeight = static_cast<int>( windowHeight * 0.06f );
    const auto bottomPanelHeight = static_cast<int>( windowHeight * 0.06f );
    mainColumnSplitter->setSizes(
        { topPanelHeight, windowHeight - ( topPanelHeight + bottomPanelHeight ), bottomPanelHeight } );

    mainColumnLayout->addWidget( mainColumnSplitter );

    auto* hSplitter = new QSplitter{ this };
    hSplitter->setOrientation( Qt::Horizontal );
    hSplitter->setStyleSheet( QString{ "QSplitter::handle { background-color:  %1; }" }.arg( splitterBorderColor ) );
    hSplitter->setHandleWidth( splitterHandleWidth );

    hSplitter->addWidget( _leftPanel );
    hSplitter->addWidget( _vsgWidget );
    hSplitter->addWidget( _rightPanel );

    const auto leftPanelWidth = static_cast<int>( windowWidth * 0.08f );
    const auto rightPanelWidth = static_cast<int>( windowWidth * 0.08f );
    hSplitter->setSizes( { leftPanelWidth, windowWidth - ( leftPanelWidth + rightPanelWidth ), rightPanelWidth } );

    hLayout->addWidget( hSplitter );

    this->show();
}

auto TiredUI::writeSettings() -> void {
    _settings->beginGroup( "MainWindow" );
    _settings->setValue( "geometry", saveGeometry() );
    _settings->endGroup();

    _settings->sync();
}

auto TiredUI::readSettings() -> std::pair<int, int> {
    _settings->beginGroup( "MainWindow" );

    std::pair<int, int> result{};

    const auto geometry = _settings->value( "geometry", QByteArray() ).toByteArray();
    if ( geometry.isEmpty() ) {
        setGeometry( 200, 200, 1024, 768 );
        result = std::make_pair( 1024, 768 );
    } else {
        restoreGeometry( geometry );
        const auto g = this->geometry();
        result = std::make_pair( g.width(), g.height() );
    }

    _settings->endGroup();

    return result;
}

void TiredUI::quitApplication() {
    QApplication::quit();
}

void TiredUI::closeEvent( QCloseEvent* event ) {
    writeSettings();

    log::info()( "close event handled!" );
}

void TiredUI::onGlobalMouseMove( const QPointF& pos ) {
    _tired->setGlobalMousePosX( pos.x() );
    _tired->setGlobalMousePosY( pos.y() );
}

void TiredUI::moveWindow() {
    this->windowHandle()->startSystemMove();
}

void TiredUI::resizeWindow( int edge ) {
    const auto e = static_cast<Qt::Edge>( edge );
    this->windowHandle()->startSystemResize( e );
}

}  // namespace tire