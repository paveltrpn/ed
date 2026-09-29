
#include <print>

#include "inputhandler.h"
#include "scene_object/sceneobjectgraph.h"

namespace tire {

// ======================================================================================
// ==================== InputHandler ====================================================
// ======================================================================================

InputHandler::InputHandler( Scenegraph* scenegraph, vsg::observer_ptr<vsg::Camera> camera, QObject* parent )
    : QObject{ parent }
    , _handler{ new Handler{ scenegraph, camera } } {
}

auto InputHandler::handler() -> const vsg::ref_ptr<Handler> {
    return _handler;
}

// ======================================================================================
// ==================== Handler =========================================================
// ======================================================================================

Handler::Handler( Scenegraph* scenegraph, vsg::observer_ptr<vsg::Camera> camera )
    : vsg::Visitor{}
    , _scenegraph{ scenegraph }
    , _camera{ camera } {
}

void Handler::apply( vsg::KeyPressEvent& keyPress ) {
    if ( closeKey != vsg::KEY_Undefined && keyPress.keyBase == closeKey ) {
        close();
    }
}

void Handler::apply( vsg::KeyReleaseEvent& keyRelease ) {
}

void Handler::apply( vsg::FocusInEvent& focusIn ) {
}

void Handler::apply( vsg::FocusOutEvent& focusOut ) {
}

void Handler::apply( vsg::ButtonPressEvent& buttonPress ) {
    switch ( buttonPress.button ) {
        case 1: {
            onLMBPress( buttonPress );
            return;
        }

        case 2: {
            onMMBPress( buttonPress );
            return;
        }

        case 3: {
            onRMBPress( buttonPress );
            return;
        }
    }
}

void Handler::apply( vsg::ButtonReleaseEvent& buttonRelease ) {
    switch ( buttonRelease.button ) {
        case 1: {
            onLMBRelease( buttonRelease );
            return;
        }

        case 2: {
            onMMBRelease( buttonRelease );
            return;
        }

        case 3: {
            onRMBRelease( buttonRelease );
            return;
        }
    }
}

void Handler::apply( vsg::MoveEvent& moveEvent ) {
}

void Handler::apply( vsg::ScrollWheelEvent& scrollWheel ) {
}

void Handler::apply( vsg::TouchDownEvent& touchDown ) {
}

void Handler::apply( vsg::TouchUpEvent& touchUp ) {
}

void Handler::apply( vsg::TouchMoveEvent& touchMove ) {
}

void Handler::apply( vsg::FrameEvent& frame ) {
}

void Handler::apply( vsg::CloseWindowEvent& ) {
    close();
}

void Handler::apply( vsg::TerminateEvent& ) {
    close();
}

void Handler::close() {
    // take a ref_ptr<> of the observer_ptr<> to be able to safely access it
    vsg::ref_ptr<vsg::Viewer> viewer = _scenegraph->scenegraphViewer();
    if ( viewer ) {
        viewer->close();
    }
}

auto Handler::collectIntersections( int32_t x, int32_t y )
    -> std::vector<vsg::ref_ptr<vsg::LineSegmentIntersector::Intersection>> {
    //

    auto intersector = vsg::LineSegmentIntersector::create( *_camera.get(), x, y );

    // const auto beforeIntersection = vsg::clock::now();

    _scenegraph->root()->accept( *intersector );

    // const auto afterIntersection = vsg::clock::now();

    // Sort the intersections front to back.
    std::ranges::sort( intersector->intersections, []( auto& lhs, auto& rhs ) {
        //
        return lhs->ratio < rhs->ratio;
    } );

    return intersector->intersections;
}

void Handler::onLMBPress( vsg::PointerEvent& pointerEvent ) {
    const auto& intersections = collectIntersections( pointerEvent.x, pointerEvent.y );

    if ( intersections.empty() ) {
        _scenegraph->bounding()->setTransformMat( vsg::mat4{} );
        _scenegraph->scene()->setSelectedObjectUid( QUuid{}.toString() );
        return;
    }

    for ( auto& intersection : intersections ) {
        auto handled = bool{ false };

        for ( auto node : intersection->nodePath ) {
            if ( auto clickedDrawable = dynamic_cast<const SceneObjectGraph*>( node ) ) {
                auto owner = clickedDrawable->owner();
                _scenegraph->scene()->setSelectedObjectUid( owner->uid() );
                _scenegraph->bounding()->setOnObject( owner );

                handled = true;
            } else if ( auto clickedDrawable = dynamic_cast<const MoveDragger*>( node ) ) {
                std::println( " dragger axis: {}", static_cast<int>( clickedDrawable->axis() ) );

                _dragActive = true;

                handled = true;
            }
        }

        if ( handled ) {
            return;
        }
    }
}

void Handler::onMMBPress( vsg::PointerEvent& pointerEvent ) {
    std::println( " VSG middle button press" );
}

void Handler::onRMBPress( vsg::PointerEvent& pointerEvent ) {
    std::println( " VSG right button press" );
}

void Handler::onLMBRelease( vsg::PointerEvent& pointerEvent ) {
    std::println( " VSG left button release" );
}

void Handler::onMMBRelease( vsg::PointerEvent& pointerEvent ) {
    std::println( " VSG middle button release" );
}

void Handler::onRMBRelease( vsg::PointerEvent& pointerEvent ) {
    std::println( " VSG right button release" );
}

}  // namespace tire
