// Portions of this code are derived from vsgQt, licensed under the MIT License.
// See: https:/https://github.com/vsg-dev/vsgQt/LICENSE

#if defined( WIN32 )
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined( __APPLE__ )
#define VK_USE_PLATFORM_MACOS_MVK
#else
#define VK_USE_PLATFORM_XCB_KHR
// #define VK_USE_PLATFORM_XLIB_KHR
#endif

#include <vsg/all.h>

#include <QPlatformSurfaceEvent>
#include <QWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include <vulkan/vulkan.h>

#include "viewer.h"

namespace tire {

Viewer::Viewer( int msecTimerInterval ) {
    // set the default timer as 8ms.
    if ( msecTimerInterval > 0 ) setInterval( msecTimerInterval );
}

bool Viewer::pollEvents( bool discardPreviousEvents ) {
    if ( discardPreviousEvents ) _events.clear();
    for ( auto& window : _windows ) {
        _events.splice( _events.end(), window->bufferedEvents );
        window->bufferedEvents.clear();
    }

    return !_events.empty();
}

void Viewer::request() {
    ++requests;
}

void Viewer::render( double simulationTime ) {
    if ( !continuousUpdate && requests.load() == 0 ) {
        //vsg::info("render() no render : requests = ", requests.load());
        return;
    }

    if ( advanceToNextFrame( simulationTime ) ) {
        handleEvents();
        update();
        recordAndSubmit();
        present();
    } else {
        if ( status->cancel() ) {
            QCoreApplication::quit();
        }
    }

    requests = 0;
}

void Viewer::setInterval( int msecTimerInterval ) {
    timer.setInterval( msecTimerInterval );
    timer.connect( &timer, &QTimer::timeout, [&]() { render(); } );
    timer.start();
}

}  // namespace tired
