#pragma once

// Portions of this code are derived from vsgQt, licensed under the MIT License.
// See: https:/https://github.com/vsg-dev/vsgQt/LICENSE

#include <QWindow>

#include <vsg/app/Window.h>

#include "keyboard_map.h"
#include "viewer.h"

namespace tire {

class Window : public QWindow {
public:
    Window( QScreen* targetScreen = nullptr );
    Window( QWindow* parent );
    Window( vsg::ref_ptr<vsg::WindowTraits> in_traits, QScreen* targetScreen = nullptr );
    Window( vsg::ref_ptr<vsg::WindowTraits> in_traits, QWindow* parent );
    Window( vsg::ref_ptr<Viewer> in_viewer, vsg::ref_ptr<vsg::WindowTraits> in_traits,
            QScreen* targetScreen = nullptr );
    Window( vsg::ref_ptr<Viewer> in_viewer, vsg::ref_ptr<vsg::WindowTraits> in_traits, QWindow* parent );

    virtual ~Window();

    vsg::ref_ptr<vsg::WindowTraits> traits;
    vsg::ref_ptr<Viewer> viewer;

    vsg::ref_ptr<vsg::Window> windowAdapter;
    vsg::ref_ptr<KeyboardMap> keyboardMap;

    operator vsg::ref_ptr<vsg::Window>() { return windowAdapter; }

    /// Initialize the Vulkan integration using VulkanSceneGraph VkInstance/VkSurface support
    virtual void initializeWindow();

protected:
    void cleanup();

    bool event( QEvent* e ) override;

    void exposeEvent( QExposeEvent* ) override;
    void hideEvent( QHideEvent* ev ) override;

    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;
    void mouseMoveEvent( QMouseEvent* ) override;
    void mousePressEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;
    void resizeEvent( QResizeEvent* ) override;
    void wheelEvent( QWheelEvent* ) override;

    /// convert Qt's window coordinate into Vulkan/VSG ones by scaling by the devicePixelRatio()
    template <typename T>
    int32_t convert_coord( T c ) const {
        return static_cast<int32_t>( std::round( static_cast<qreal>( c ) * devicePixelRatio() ) );
    }

    std::pair<vsg::ButtonMask, uint32_t> convertMouseButtons( QMouseEvent* e ) const;
    std::pair<int32_t, int32_t> convertMousePosition( QMouseEvent* e ) const;

private:
    bool _initialized = false;
};

}  // namespace tired

EVSG_type_name( tire::Window );
