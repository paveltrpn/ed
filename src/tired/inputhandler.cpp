
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
        _scenegraph->gizmo()->update( { moveEvent.x - _moveStart.x, moveEvent.y - _moveStart.y }, _dragAxis );
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
                _moveStart = vsg::ivec2{ pointerEvent.x, pointerEvent.y };
                _dragAxis = clickedDrawable->axis();

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
        _scenegraph->gizmo()->reset();
        _manipulator->setDragActive( false );
        _moveStart = vsg::ivec2{};
    }
}

void Handler::onMMBRelease( vsg::PointerEvent& pointerEvent ) {
}

void Handler::onRMBRelease( vsg::PointerEvent& pointerEvent ) {
}

}  // namespace tire
