
#include <vsg/all.h>

#include <QVulkanInstance>
#include <QQuickGraphicsDevice>

#include "ui/ui.h"
#include "config/config.h"
#include <tired/tired.h>

QDir workPath() {
    auto wp = QDir{ QDir::currentPath() };
    wp.cdUp();
    return wp;
};

auto main( int argc, char* argv[] ) -> int {
    qputenv( "QSG_RENDER_LOOP", "basic" );

    tire::Config::init( "assets/config.json" );

    tire::Tired::init();

    // Force use vulkan as backend renderer API.
    QQuickWindow::setGraphicsApi( QSGRendererInterface::Vulkan );

    const QGuiApplication app( argc, argv );

    auto vsgRender = tire::Tired::pointer()->vsgRender();
    QVulkanInstance* vulkanInstance = new QVulkanInstance{};

    vulkanInstance->setVkInstance( vsgRender->instance() );

    if ( !vulkanInstance->create() ) {
        qFatal( "QVulkanInstance::create failed" );
    }

    QQuickGraphicsDevice device = QQuickGraphicsDevice::fromDeviceObjects(
        vsgRender->physicalDevice(), vsgRender->logicalDevice(), vsgRender->queueFamily() );

    auto tiredUI = new tire::TiredUI{ vsgRender, nullptr };

    tiredUI->setVulkanInstance( vulkanInstance );

    tiredUI->setGraphicsDevice( device );

    tiredUI->setResizeMode( QQuickView::SizeRootObjectToView );
    tiredUI->resize( 1920, 1800 );
    tiredUI->setSource( QUrl( workPath().path() + QDir::separator() + "src/ui/qml/main.qml" ) );
    tiredUI->setPosition( 300, 300 );

    tiredUI->show();

    app.exec();
}
