
#include <print>
#include <iostream>

#include <vsg/all.h>

#include <QQuickWindow>
#include <QVulkanInstance>
#include <QSGTexture>
#include <QSGSimpleTextureNode>

#include "renderitem.h"

namespace tire {

RenderItem::RenderItem( QQuickItem* parent )
    : QQuickItem{ parent }
    , _keyboardMap{ KeyboardMap::create() } {
    //
    setFlag( QQuickItem::ItemHasContents );
    connect( this, &QQuickItem::windowChanged, this, &RenderItem::handleWindowChanged );
}

auto RenderItem::setVsgRender( std::shared_ptr<VsgRender> render ) -> void {
    _render = render;
}

auto RenderItem::updatePaintNode( QSGNode* oldNode, UpdatePaintNodeData* ) -> QSGNode* {
    auto* node = static_cast<QSGSimpleTextureNode*>( oldNode );

    if ( !_vkImage ) {
        delete node;
        return nullptr;
    }

    if ( !node ) {
        node = new QSGSimpleTextureNode();
    }

    // Удаляем старую текстуру Qt Quick, если она была создана ранее.
    QSGTexture* oldTexture = node->texture();
    delete oldTexture;

    // Оборачиваем существующий VkImage. Qt НЕ берет на себя владение VkImage.
    QSGTexture* qsgTexture =
        QNativeInterface::QSGVulkanTexture::fromNative( _vkImage, _currentLayout, _window, _textureSize );

    if ( qsgTexture ) {
        node->setTexture( qsgTexture );
        node->setRect( 0, 0, _textureSize.width(), _textureSize.height() );
    }

    return node;
};

auto RenderItem::geometryChange( const QRectF& newGeometry, const QRectF& oldGeometry ) -> void {
    QQuickItem::geometryChange( newGeometry, oldGeometry );

    if ( !_render.get() ) {
        return;
    }

    if ( newGeometry.size() != oldGeometry.size() ) {
        _render->setNeedResize( true );
        _render->setExtent( { static_cast<uint32_t>( newGeometry.size().width() ),
                              static_cast<uint32_t>( newGeometry.size().height() ) } );
        // update();
    }
}

auto RenderItem::handleWindowChanged( QQuickWindow* window ) -> void {
    if ( window ) {
        connect( window, &QQuickWindow::beforeSynchronizing, this, &RenderItem::sync, Qt::DirectConnection );
        connect( window, &QQuickWindow::sceneGraphInvalidated, this, &RenderItem::cleanup, Qt::DirectConnection );
    }
}

auto RenderItem::cleanup() -> void {
}

auto RenderItem::updateWindow() -> void {
    if ( _initialized ) {
        auto frame = _render->renderNative();

        if ( frame.has_value() ) {
            auto imageData = frame.value();

            auto extent = VkExtent2D{};
            std::tie( _vkImage, extent, _currentLayout ) = imageData;
            _textureSize = QSize{ static_cast<int>( extent.width ), static_cast<int>( extent.height ) };

            // auto extent = _render->extent();
            // _cpuSideImage = QImage{ (uchar*)imageData->dataPointer(), static_cast<int>( extent.width ),
            //                         static_cast<int>( extent.height ), QImage::Format_RGBA8888 }
            //                     .copy();

            update();
        };
    }
}

auto RenderItem::sync() -> void {
    if ( !_render.get() ) {
        return;
    }

    if ( !_initialized ) {
        _window = window();
        if ( !_window ) {
            qDebug() << "bad qquickitem window...";
        }

        _renderInterface = _window->rendererInterface();
        if ( !_renderInterface ) {
            qDebug() << "RenderItem === bad qquickitem render interface...";
        }

        void* resource = _renderInterface->getResource( _window, QSGRendererInterface::DeviceResource );
        if ( resource ) {
            VkDevice vulkanDevice = *static_cast<VkDevice*>( resource );
        }

        _initialized = true;
    }
}

bool RenderItem::event( QEvent* e ) {
    // switch ( e->type() ) {
    //     case QEvent::PlatformSurface: {
    //         auto surfaceEvent = dynamic_cast<QPlatformSurfaceEvent*>( e );
    //         if ( surfaceEvent->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed ) {
    //             vsg::clock::time_point event_time = vsg::clock::now();
    //             _windowAdapter->bufferedEvents.push_back( vsg::CloseWindowEvent::create( _windowAdapter, event_time ) );

    //             cleanup();
    //         }
    //         break;
    //     }

    //     default:
    //         break;
    // }

    return QQuickItem::event( e );
}

void RenderItem::keyPressEvent( QKeyEvent* e ) {
    if ( !_render ) {
        return;
    }

    vsg::KeySymbol keySymbol, modifiedKeySymbol;
    vsg::KeyModifier keyModifier;

    if ( _keyboardMap->getKeySymbol( e, keySymbol, modifiedKeySymbol, keyModifier ) ) {
        vsg::clock::time_point event_time = vsg::clock::now();

        auto event = vsg::KeyPressEvent::create( nullptr, event_time, keySymbol, modifiedKeySymbol, keyModifier );
        _render->_bufferedEvents.push_back( event );
    }
}

void RenderItem::keyReleaseEvent( QKeyEvent* e ) {
    if ( !_render ) {
        return;
    }

    vsg::KeySymbol keySymbol, modifiedKeySymbol;
    vsg::KeyModifier keyModifier;

    if ( _keyboardMap->getKeySymbol( e, keySymbol, modifiedKeySymbol, keyModifier ) ) {
        vsg::clock::time_point event_time = vsg::clock::now();

        auto event = vsg::KeyReleaseEvent::create( nullptr, event_time, keySymbol, modifiedKeySymbol, keyModifier );
        _render->_bufferedEvents.push_back( event );
    }
}

void RenderItem::mouseMoveEvent( QMouseEvent* e ) {
    if ( !_render ) return;

    auto p = mapFromScene( e->position() );

    if ( !contains( p ) ) {
        e->ignore();
        return;
    }

    vsg::clock::time_point event_time = vsg::clock::now();

    auto [mask, button] = convertMouseButtons( e );
    auto [x, y] = p;  //convertMousePosition( e );

    auto event = vsg::MoveEvent::create( nullptr, event_time, x, y, mask );
    _render->_bufferedEvents.push_back( event );
}

void RenderItem::mousePressEvent( QMouseEvent* e ) {
    if ( !_render ) {
        return;
    }

    auto p = mapFromScene( e->position() );

    if ( !contains( p ) ) {
        e->ignore();
        return;
    }

    vsg::clock::time_point event_time = vsg::clock::now();

    auto [mask, button] = convertMouseButtons( e );
    auto [x, y] = p;  //convertMousePosition( e );

    auto event = vsg::ButtonPressEvent::create( nullptr, event_time, x, y, mask, button );
    _render->_bufferedEvents.push_back( event );
}

void RenderItem::mouseReleaseEvent( QMouseEvent* e ) {
    if ( !_render ) return;

    auto p = mapFromScene( e->position() );

    if ( !contains( p ) ) {
        e->ignore();
        return;
    }

    vsg::clock::time_point event_time = vsg::clock::now();

    auto [mask, button] = convertMouseButtons( e );
    auto [x, y] = p;  //convertMousePosition( e );

    auto event = vsg::ButtonReleaseEvent::create( nullptr, event_time, x, y, mask, button );
    _render->_bufferedEvents.push_back( event );
}

void RenderItem::wheelEvent( QWheelEvent* e ) {
    if ( !_render ) {
        return;
    }

    vsg::clock::time_point event_time = vsg::clock::now();

    auto event = vsg::ScrollWheelEvent::create(
        nullptr, event_time, e->angleDelta().y() < 0 ? vsg::vec3( 0.0f, -1.0f, 0.0f ) : vsg::vec3( 0.0f, 1.0f, 0.0f ) );
    _render->_bufferedEvents.push_back( event );
}

std::pair<vsg::ButtonMask, uint32_t> RenderItem::convertMouseButtons( QMouseEvent* e ) const {
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

std::pair<int32_t, int32_t> RenderItem::convertMousePosition( QMouseEvent* e ) const {
#if QT_VERSION_MAJOR == 6
    return { convert_coord( e->position().x() ), convert_coord( e->position().y() ) };
#else
    return { convert_coord( e->x() ), convert_coord( e->y() ) };
#endif
}

}  // namespace tire
