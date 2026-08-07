#include <vsg/all.h>

#include <QHBoxLayout>
#include <QSplitter>
#include <QWidget>
#include <QApplication>

#include "ui.h"
#include "log/log.h"

namespace tire {

// ====================================================================
// ========== TiredUi =================================================
// ====================================================================

TiredUI::TiredUI( QObject *parent )
    : _tired{ std::make_unique<tire::Tired>() }
    , _settings{ new QSettings{ this } }
    , _engine{ new QQmlEngine{ this } }
    , _context{ _engine->rootContext() }
    , _topPanel{ new QQuickWidget{ _engine, this } }
    , _leftPanel{ new QQuickWidget{ _engine, this } }
    , _bottomPanel{ new QQuickWidget{ _engine, this } }
    , _rightPanel{ new QQuickWidget{ _engine, this } }
    , _theme{ new Appearance{ this } } {
    //

    readSettings();

    _tired->registerTypes();

    _engine->addImageProvider( "TiredImageProvider", new TiredImageProvider{} );

    auto windowTraits = vsg::WindowTraits::create();
    windowTraits->windowTitle = "ed";
    windowTraits->vulkanVersion = VK_MAKE_API_VERSION( 0, 1, 4, 0 );
    // windowTraits->clearColor = vsg::vec4{ 55.0f / 255.0f, 55.0f / 255.0f, 55.0f / 255.0f, 1.0f };
    // windowTraits->fullscreen = true;

    qmlRegisterSingletonInstance( "Tire", 1, 0, "Appearence", _theme );
    qmlRegisterSingletonInstance( "Tire", 1, 0, "Tired", _tired.get() );

    // Use this object for main window position and size (in particular).
    qmlRegisterSingletonInstance( "Tire", 1, 0, "MainWindow", this );

    // Remove native decoration.
    setWindowFlags( Qt::FramelessWindowHint );

    // Set transparent main window to use qml defined application
    // specific decoration.
    setAttribute( Qt::WA_TranslucentBackground );

    // VSG initialization.
    _vsgWindow = new Window( _tired->viewer(), windowTraits );
    _vsgWindow->setTitle( "title" );
    _vsgWindow->initializeWindow();

    _tired->init( _vsgWindow, windowTraits->width, windowTraits->height );

    // Qt widgets initialization.
    _vsgWidget = QWidget::createWindowContainer( _vsgWindow, this );

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

    const auto topPanelHeight = _theme->getGap( "top_panel_height" );
    const auto leftPanelWidth = _theme->getGap( "left_panel_width" );
    const auto rightPanelWidth = 56;
    const auto splitterBorderColor = _theme->getColor( "background" );
    const auto splitterHandleWidth = _theme->getGap( "quarter" );
    const auto clearColor = _theme->getColor( "clear_color" );

    auto centralWidget = new QWidget{ this };
    setCentralWidget( centralWidget );

    auto *mainColumnLayout = new QVBoxLayout{};
    mainColumnLayout->setContentsMargins( 0, 0, 0, 0 );
    centralWidget->setLayout( mainColumnLayout );

    auto *mainColumnSplitter = new QSplitter{ this };
    mainColumnSplitter->setOrientation( Qt::Vertical );
    mainColumnSplitter->setStyleSheet(
        QString{ "QSplitter::handle { background-color:  %1; }" }.arg( splitterBorderColor ) );
    mainColumnSplitter->setHandleWidth( splitterHandleWidth );

    auto *hLayout = new QHBoxLayout{};
    hLayout->setContentsMargins( 0, 0, 0, 0 );

    auto middleElementsWidget = new QWidget{ this };
    middleElementsWidget->setLayout( hLayout );

    mainColumnSplitter->addWidget( _topPanel );
    mainColumnSplitter->addWidget( middleElementsWidget );
    mainColumnSplitter->addWidget( _bottomPanel );
    mainColumnSplitter->setSizes( { topPanelHeight, 1080 - topPanelHeight * 2, topPanelHeight } );

    mainColumnLayout->addWidget( mainColumnSplitter );

    auto *hSplitter = new QSplitter{ this };
    hSplitter->setOrientation( Qt::Horizontal );
    hSplitter->setStyleSheet( QString{ "QSplitter::handle { background-color:  %1; }" }.arg( splitterBorderColor ) );
    hSplitter->setHandleWidth( splitterHandleWidth );

    hSplitter->addWidget( _leftPanel );
    hSplitter->addWidget( _vsgWidget );
    hSplitter->addWidget( _rightPanel );
    hSplitter->setSizes( { leftPanelWidth, 1920 - leftPanelWidth - rightPanelWidth, rightPanelWidth } );

    hLayout->addWidget( hSplitter );

    this->show();
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
        setGeometry( 200, 200, 400, 800 );
    } else {
        restoreGeometry( geometry );
    }

    _settings->endGroup();
}

void TiredUI::quitApplication() {
    QApplication::quit();
}

void TiredUI::closeEvent( QCloseEvent *event ) {
    writeSettings();

    log::info()( "close event handled!" );
}

void TiredUI::onGlobalMouseMove( const QPointF &pos ) {
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