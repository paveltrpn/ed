
#include <print>

#include "inputhandler.h"
#include "scene_object/sceneobjectgraph.h"

namespace tire {

// ======================================================================================
// ==================== InputHandler ====================================================
// ======================================================================================

InputHandler::InputHandler( Scenegraph* scenegraph, Manipulator* manipuilator, vsg::observer_ptr<vsg::Camera> camera,
                            QObject* parent )
    : QObject{ parent }
    , _handler{ new Handler{ scenegraph, manipuilator, camera } } {
}

auto InputHandler::handler() -> const vsg::ref_ptr<Handler> {
    return _handler;
}

// ======================================================================================
// ==================== Handler =========================================================
// ======================================================================================

Handler::Handler( Scenegraph* scenegraph, Manipulator* manipuilator, vsg::observer_ptr<vsg::Camera> camera )
    : vsg::Visitor{}
    , _scenegraph{ scenegraph }
    , _manipulator{ manipuilator }
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
    if ( _manipulator->dragActive() ) {
        const auto worldPoint = dragPlaneIntersection( moveEvent.x, moveEvent.y );
        _scenegraph->gizmo()->update( worldPoint - _dragStartWorld, _dragAxis );
    }
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

auto Handler::viewDirection() -> vsg::dvec3 {
    const auto camera = _camera.get();

    const auto eyeToWorld = vsg::inverse( camera->viewMatrix->transform() );
    return vsg::normalize( ( eyeToWorld * vsg::dvec4{ 0.0, 0.0, -1.0, 0.0 } ).xyz );
}

auto Handler::dragPlaneIntersection( int32_t x, int32_t y ) -> vsg::dvec3 {
    const auto camera = _camera.get();

    const auto viewport = camera->getViewport();
    const double ndcX = ( static_cast<double>( x ) - viewport.x ) / static_cast<double>( viewport.width ) * 2.0 - 1.0;
    const double ndcY = ( static_cast<double>( y ) - viewport.y ) / static_cast<double>( viewport.height ) * 2.0 - 1.0;

    const auto projection = camera->projectionMatrix->transform();
    const auto invProjection = vsg::inverse( projection );
    const auto eyeToWorld = vsg::inverse( camera->viewMatrix->transform() );

    // Same near/far NDC convention as vsg::LineSegmentIntersector.
    const bool reverseDepth = projection( 2, 2 ) > 0.0;
    const double zNear = reverseDepth ? viewport.maxDepth : viewport.minDepth;
    const double zFar = reverseDepth ? viewport.minDepth : viewport.maxDepth;

    const auto near4 = eyeToWorld * invProjection * vsg::dvec4{ ndcX, ndcY, zNear, 1.0 };
    const auto far4 = eyeToWorld * invProjection * vsg::dvec4{ ndcX, ndcY, zFar, 1.0 };

    const vsg::dvec3 a{ near4.x / near4.w, near4.y / near4.w, near4.z / near4.w };
    const vsg::dvec3 b{ far4.x / far4.w, far4.y / far4.w, far4.z / far4.w };

    const vsg::dvec3 dir{ b - a };

    const double denom = vsg::dot( dir, _dragPlaneNormal );
    const double t = vsg::dot( _dragPlaneAnchor - a, _dragPlaneNormal ) / denom;

    return a + dir * t;
}

void Handler::onLMBPress( vsg::PointerEvent& pointerEvent ) {
    const auto& intersections = collectIntersections( pointerEvent.x, pointerEvent.y );

    auto scene = _scenegraph->scene();
    auto bound = _scenegraph->bounding();

    if ( intersections.empty() ) {
        bound->setTransformMat( vsg::mat4{} );
        scene->setSelectedObjectUid( QUuid{}.toString() );

        return;
    }

    for ( auto& intersection : intersections ) {
        auto handled = bool{ false };

        for ( auto node : intersection->nodePath ) {
            if ( auto clickedDrawable = dynamic_cast<const SceneObjectGraph*>( node ) ) {
                auto owner = clickedDrawable->owner();
                scene->setSelectedObjectUid( owner->uid() );
                bound->setOnObject( owner );

                handled = true;
            } else if ( auto clickedDrawable = dynamic_cast<const MoveDragger*>( node ) ) {
                std::println( " dragger axis: {}", static_cast<int>( clickedDrawable->axis() ) );

                _manipulator->setDragActive( true );
                _dragAxis = clickedDrawable->axis();
                _scenegraph->gizmo()->beginDrag();

                _dragPlaneAnchor = vsg::dvec3{ _scenegraph->gizmo()->translateX(), 0.0, 0.0 };
                _dragPlaneNormal = viewDirection();
                _dragStartWorld = dragPlaneIntersection( pointerEvent.x, pointerEvent.y );

                handled = true;
            }
        }

        if ( handled ) {
            return;
        }
    }
}

void Handler::onMMBPress( vsg::PointerEvent& pointerEvent ) {
}

void Handler::onRMBPress( vsg::PointerEvent& pointerEvent ) {
}

void Handler::onLMBRelease( vsg::PointerEvent& pointerEvent ) {
    if ( _manipulator->dragActive() ) {
        _manipulator->setDragActive( false );
    }
}

void Handler::onMMBRelease( vsg::PointerEvent& pointerEvent ) {
}

void Handler::onRMBRelease( vsg::PointerEvent& pointerEvent ) {
}

}  // namespace tire
