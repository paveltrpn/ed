
#pragma once

#include <memory>

#include <QImage>
#include <QSGRendererInterface>
#include <QQuickItem>
#include <QWindow>

#include <vsg/all.h>

#include "keyboardmap.h"
#include "../tired/vsgrender.h"

namespace tire {

// ======================================================================================
// ==================== RenderItem ======================================================
// ======================================================================================

// QQuickItem component, responsible for render scene.
// Drawed at forground of main window. Spawn in main.qml
struct RenderItem : public QQuickItem {
    Q_OBJECT
    QML_ELEMENT

public:
    RenderItem( QQuickItem* parent = nullptr );

    auto setVsgRender( std::shared_ptr<VsgRender> render ) -> void;

public slots:
    // Call update to undelying window. Redraw item.
    auto updateWindow() -> void;
    // Render will be created and qt handles will be
    // acquired at first call of this signal.
    auto sync() -> void;
    auto cleanup() -> void;

protected:
    auto updatePaintNode( QSGNode* node, UpdatePaintNodeData* ) -> QSGNode* override;
    auto geometryChange( const QRectF& newGeometry, const QRectF& oldGeometry ) -> void override;

public:
    bool event( QEvent* e ) override;

    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;
    void mouseMoveEvent( QMouseEvent* ) override;
    void mousePressEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;
    void wheelEvent( QWheelEvent* ) override;

    /// convert Qt's window coordinate into Vulkan/VSG ones by scaling by the devicePixelRatio()
    template <typename T>
    int32_t convert_coord( T c ) const {
        return static_cast<int32_t>( std::round( static_cast<qreal>( c ) * 1.0f /*devicePixelRatio()*/ ) );
    }

    std::pair<vsg::ButtonMask, uint32_t> convertMouseButtons( QMouseEvent* e ) const;
    std::pair<int32_t, int32_t> convertMousePosition( QMouseEvent* e ) const;

private:
    auto handleWindowChanged( QQuickWindow* win ) -> void;

private:
    bool _initialized{ false };

    vsg::ref_ptr<KeyboardMap> _keyboardMap;

    // Cached window and render interface that this item
    // assined to. Is this pointers valid through all
    // window lifetime?
    QQuickWindow* _window{};

    // The ownership of the  pointer returned from render interface
    // is never transferred to the caller.
    QSGRendererInterface* _renderInterface{};

    QImage _cpuSideImage{};

    std::shared_ptr<VsgRender> _render{};

    VkImage _vkImage{ VK_NULL_HANDLE };
    QSize _textureSize{};
    VkImageLayout _currentLayout{ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
};

}  // namespace tire
