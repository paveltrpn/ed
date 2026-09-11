// Portions of this code are derived from vsgQt, licensed under the MIT License.
// See: https:/https://github.com/vsg-dev/vsgQt/LICENSE

#if defined( WIN32 )
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined( __APPLE__ )
#define VK_USE_PLATFORM_MACOS_MVK
#else
#define VK_USE_PLATFORM_XCB_KHR
// #define VK_USE_PLATFORM_WAYLAND_KHR
// #define VK_USE_PLATFORM_XLIB_KHR
#endif

#if defined( VK_USE_PLATFORM_WAYLAND_KHR )
#include <wayland-client.h>
#include <wayland-server.h>
#endif

#include <vsg/all.h>

#include <QPlatformSurfaceEvent>
#include <QWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include <vulkan/vulkan.h>

#include "window.h"

namespace tire {

VsgWidget::VsgWidget( vsg::ref_ptr<Viewer> in_viewer, vsg::ref_ptr<vsg::WindowTraits> in_traits )
    : QWidget()
    , viewer( in_viewer )
    , keyboardMap( KeyboardMap::create() ) {
    if ( in_traits ) {
        traits = vsg::WindowTraits::create( *in_traits );
        setGeometry( traits->x, traits->y, traits->width, traits->height );
    } else {
        traits = vsg::WindowTraits::create();
        traits->x = x();
        traits->y = y();
        traits->width = width();
        traits->height = height();
    }
}

VsgWidget::~VsgWidget() {
    cleanup();
}

void VsgWidget::initializeWindow() {
    if ( windowAdapter ) {
        return;
    }

    if ( !traits ) {
        traits = vsg::WindowTraits::create();
    }

#if defined( VK_USE_PLATFORM_WIN32_KHR )
    traits->nativeWindow = reinterpret_cast<HWND>( winId() );
#elif defined( VK_USE_PLATFORM_XLIB_KHR )
    traits->nativeWindow = static_cast<::Window>( winId() );
#elif defined( VK_USE_PLATFORM_XCB_KHR )
    traits->nativeWindow = static_cast<xcb_window_t>( winId() );
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
    //traits->nativeWindow =  reinterpret_cast<wl_surface*>( winId() );
    //traits->nativeDisplay = wl_display;
#elif defined( VK_USE_PLATFORM_MACOS_MVK )
    traits->nativeWindow = winId();
#endif

    // traits->x = convert_coord( x() );
    // traits->y = convert_coord( y() );
    // traits->width = convert_coord( width() );
    // traits->height = convert_coord( height() );

    windowAdapter = vsg::Window::create( traits );
    _initialized = true;
}

void VsgWidget::cleanup() {
    // remove links to all the VSG related classes.
    if ( windowAdapter ) {
        // wait for all rendering to be completed before we start cleaning up resources.
        if ( viewer ) {
            viewer->deviceWaitIdle();
            viewer->removeWindow( windowAdapter );
        }

        windowAdapter->releaseWindow();
    }

    windowAdapter = {};
    viewer = {};
}

// void Window::exposeEvent( QExposeEvent* /*e*/ ) {
//     if ( !_initialized && isExposed() ) {
//         initializeWindow();
//     }

//     if ( viewer ) viewer->request();
// }

bool VsgWidget::event( QEvent* e ) {
    switch ( e->type() ) {
        case QEvent::PlatformSurface: {
            auto surfaceEvent = dynamic_cast<QPlatformSurfaceEvent*>( e );
            if ( surfaceEvent->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed ) {
                vsg::clock::time_point event_time = vsg::clock::now();
                windowAdapter->bufferedEvents.push_back( vsg::CloseWindowEvent::create( windowAdapter, event_time ) );

                cleanup();
            }
            break;
        }

        default:
            break;
    }

    return QWidget::event( e );
}

void VsgWidget::hideEvent( QHideEvent* /*e*/ ) {
}

void VsgWidget::resizeEvent( QResizeEvent* /*e*/ ) {
    if ( !windowAdapter ) return;

    vsg::clock::time_point event_time = vsg::clock::now();
    windowAdapter->bufferedEvents.push_back(
        vsg::ConfigureWindowEvent::create( windowAdapter, event_time, convert_coord( x() ), convert_coord( y() ),
                                           convert_coord( width() ), convert_coord( height() ) ) );

    windowAdapter->resize();

    if ( viewer ) viewer->request();
}

void VsgWidget::keyPressEvent( QKeyEvent* e ) {
    if ( !windowAdapter ) return;

    vsg::KeySymbol keySymbol, modifiedKeySymbol;
    vsg::KeyModifier keyModifier;

    if ( keyboardMap->getKeySymbol( e, keySymbol, modifiedKeySymbol, keyModifier ) ) {
        vsg::clock::time_point event_time = vsg::clock::now();
        windowAdapter->bufferedEvents.push_back(
            vsg::KeyPressEvent::create( windowAdapter, event_time, keySymbol, modifiedKeySymbol, keyModifier ) );
    }

    if ( viewer ) viewer->request();
}

void VsgWidget::keyReleaseEvent( QKeyEvent* e ) {
    if ( !windowAdapter ) return;

    vsg::KeySymbol keySymbol, modifiedKeySymbol;
    vsg::KeyModifier keyModifier;

    if ( keyboardMap->getKeySymbol( e, keySymbol, modifiedKeySymbol, keyModifier ) ) {
        vsg::clock::time_point event_time = vsg::clock::now();
        windowAdapter->bufferedEvents.push_back(
            vsg::KeyReleaseEvent::create( windowAdapter, event_time, keySymbol, modifiedKeySymbol, keyModifier ) );
    }

    if ( viewer ) viewer->request();
}

void VsgWidget::mouseMoveEvent( QMouseEvent* e ) {
    if ( !windowAdapter ) return;

    vsg::clock::time_point event_time = vsg::clock::now();

    auto [mask, button] = convertMouseButtons( e );
    auto [x, y] = convertMousePosition( e );

    windowAdapter->bufferedEvents.push_back( vsg::MoveEvent::create( windowAdapter, event_time, x, y, mask ) );

    if ( viewer ) viewer->request();
}

void VsgWidget::mousePressEvent( QMouseEvent* e ) {
    if ( !windowAdapter ) return;

    vsg::clock::time_point event_time = vsg::clock::now();

    auto [mask, button] = convertMouseButtons( e );
    auto [x, y] = convertMousePosition( e );

    windowAdapter->bufferedEvents.push_back(
        vsg::ButtonPressEvent::create( windowAdapter, event_time, x, y, mask, button ) );

    if ( viewer ) viewer->request();
}

void VsgWidget::mouseReleaseEvent( QMouseEvent* e ) {
    if ( !windowAdapter ) return;

    vsg::clock::time_point event_time = vsg::clock::now();

    auto [mask, button] = convertMouseButtons( e );
    auto [x, y] = convertMousePosition( e );

    windowAdapter->bufferedEvents.push_back(
        vsg::ButtonReleaseEvent::create( windowAdapter, event_time, x, y, mask, button ) );

    if ( viewer ) viewer->request();
}

void VsgWidget::wheelEvent( QWheelEvent* e ) {
    if ( !windowAdapter ) return;

    vsg::clock::time_point event_time = vsg::clock::now();
    windowAdapter->bufferedEvents.push_back( vsg::ScrollWheelEvent::create(
        windowAdapter, event_time,
        e->angleDelta().y() < 0 ? vsg::vec3( 0.0f, -1.0f, 0.0f ) : vsg::vec3( 0.0f, 1.0f, 0.0f ) ) );

    if ( viewer ) viewer->request();
}

std::pair<vsg::ButtonMask, uint32_t> VsgWidget::convertMouseButtons( QMouseEvent* e ) const {
    uint16_t mask{ 0 };
    uint32_t button = 0;

    if ( e->buttons() & Qt::LeftButton ) mask = mask | vsg::BUTTON_MASK_1;
    if ( e->buttons() & Qt::MiddleButton ) mask = mask | vsg::BUTTON_MASK_2;
    if ( e->buttons() & Qt::RightButton ) mask = mask | vsg::BUTTON_MASK_3;

    switch ( e->button() ) {
        case Qt::LeftButton:
            button = 1;
            break;
        case Qt::MiddleButton:
            button = 2;
            break;
        case Qt::RightButton:
            button = 3;
            break;
        default:
            break;
    }

    return { static_cast<vsg::ButtonMask>( mask ), button };
}

std::pair<int32_t, int32_t> VsgWidget::convertMousePosition( QMouseEvent* e ) const {
#if QT_VERSION_MAJOR == 6
    return { convert_coord( e->position().x() ), convert_coord( e->position().y() ) };
#else
    return { convert_coord( e->x() ), convert_coord( e->y() ) };
#endif
}

}  // namespace tire
