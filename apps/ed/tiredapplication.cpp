
#include <QMouseEvent>
#include <QVulkanInstance>
#include <QQuickGraphicsDevice>

#include <vsg/all.h>

#include "tiredapplication.h"

QDir workPath() {
    auto wp = QDir{ QDir::currentPath() };
    wp.cdUp();
    return wp;
};

TiredApplication::TiredApplication( int &argc, char **argv )
    : QApplication( argc, argv ) {
    //
    auto vsgRender = std::make_shared<tire::VsgRender>( argc, argv );

    // Force use vulkan as backend renderer API.
    QQuickWindow::setGraphicsApi( QSGRendererInterface::Vulkan );

    const QGuiApplication app( argc, argv );

    QVulkanInstance *vulkanInstance = new QVulkanInstance{};
    vulkanInstance->setVkInstance( vsgRender->instance() );

    if ( !vulkanInstance->create() ) {
        qFatal( "QVulkanInstance::create failed" );
    }

    QQuickGraphicsDevice device = QQuickGraphicsDevice::fromDeviceObjects(
        vsgRender->physicalDevice(), vsgRender->logicalDevice(), vsgRender->queueFamily() );

    _tiredUI = new tire::TiredUI{ vsgRender, this };

    // tire::MainWindow w{ std::move( vsgRender ) };
    _tiredUI->setVulkanInstance( vulkanInstance );

    // Apply to the window. For QQuickRenderControl, do this before initialize().
    _tiredUI->setGraphicsDevice( device );

    _tiredUI->setResizeMode( QQuickView::SizeRootObjectToView );
    _tiredUI->resize( 1280, 1000 );
    _tiredUI->setSource( QUrl( workPath().path() + QDir::separator() + "qml/main.qml" ) );
    _tiredUI->setPosition( 300, 300 );

    _tiredUI->show();
}

bool TiredApplication::notify( QObject *receiver, QEvent *event ) {
    if ( event->type() == QEvent::MouseMove ) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );
    }

    return QApplication::notify( receiver, event );
}

bool TiredApplication::eventFilter( QObject *watched, QEvent *event ) {
    //
    return QApplication::eventFilter( watched, event );
}
