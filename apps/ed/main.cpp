
#include <vsg/all.h>

#include <QVulkanInstance>
#include <QQuickGraphicsDevice>
#include <QDir>

#include "ui/ui.h"
#include "config/config.h"
#include "tired/tired.h"

auto main( int argc, char* argv[] ) -> int {
    qputenv( "QSG_RENDER_LOOP", "basic" );

    // Init global Cunfig singlton.
    tire::Config::init( "assets/config.json" );

    // Init global Tired (main object thatt holds a render) singlton.
    tire::Tired::init();

    // Force use vulkan as backend renderer API.
    QQuickWindow::setGraphicsApi( QSGRendererInterface::Vulkan );

    // Init Qt aplication.
    const QGuiApplication app( argc, argv );

    // Init Qt vulkan instance object.
    auto vulkanInstance = new QVulkanInstance{};

    // Borrow vulkan instance handle from vsg render.
    auto vsgRender = tire::Tired::pointer()->vsgRender();
    vulkanInstance->setVkInstance( vsgRender->instance() );

    // Necessary call to create actual qt vulkan ninstance object from
    // borrowed VkInstance handle.
    if ( !vulkanInstance->create() ) {
        qFatal( "QVulkanInstance::create failed" );
    }

    // Create qt vulkan logacal device object that will be used by
    // all qt qml machinery. This deveice must be created with support
    // of vk surface extancion and presentation (dispite to vsg render itself)
    // do a off screen work.
    auto device = QQuickGraphicsDevice::fromDeviceObjects( vsgRender->physicalDevice(), vsgRender->logicalDevice(),
                                                           vsgRender->queueFamily() );

    // Main window.
    auto tiredUI = tire::TiredUI{ vsgRender, nullptr };
    tiredUI.setVulkanInstance( vulkanInstance );
    tiredUI.setGraphicsDevice( device );
    tiredUI.setResizeMode( QQuickView::SizeRootObjectToView );
    tiredUI.resize( 1920, 1800 );

    auto wp = QDir{ QDir::currentPath() };
    wp.cdUp();

    tiredUI.setSource( QUrl( wp.path() + QDir::separator() + "src/ui/qml/Main.qml" ) );
    tiredUI.setPosition( 300, 300 );
    tiredUI.show();

    // Start.
    app.exec();
}
