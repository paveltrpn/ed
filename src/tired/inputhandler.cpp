
#include <print>

#include "inputhandler.h"
#include "scene_object/sceneobjectgraph.h"

namespace tire {

// ======================================================================================
// ==================== InputHandler ====================================================
// ======================================================================================

InputHandler::InputHandler( vsg::ref_ptr<vsg::Camera> camera, vsg::ref_ptr<vsg::Viewer> viwer, Scenegraph* scenegraph,
                            QObject* parent )
    : QObject{ parent }
    , _handler{ new Handler{ this } }
    , _camera{ camera }
    , _viewer{ viwer }
    , _scenegraph{ scenegraph } {
}

auto InputHandler::handler() -> const vsg::ref_ptr<Handler> {
    return _handler;
}

auto InputHandler::camera() -> vsg::ref_ptr<vsg::Camera> {
    return _camera;
}

auto InputHandler::viewer() -> const vsg::ref_ptr<vsg::Viewer> {
    return _viewer;
};

auto InputHandler::scenegraph() -> const Scenegraph* {
    return _scenegraph;
}

void InputHandler::setMousePos( QPoint value ) {
    mousePos_ = value;
    emit mousePosUpdated();
}

QPoint InputHandler::mousePos() {
    return mousePos_;
}

// ======================================================================================
// ==================== Handler =========================================================
// ======================================================================================

Handler::Handler( InputHandler* inputHandler )
    : vsg::Visitor{}
    , _inputHandler{ inputHandler } {
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
    if ( buttonPress.button == 1 ) {
        lineSegmentIntersector( buttonPress );
    }
}

void Handler::apply( vsg::ButtonReleaseEvent& buttonRelease ) {
}

void Handler::apply( vsg::MoveEvent& moveEvent ) {
    _inputHandler->setMousePos( { moveEvent.x, moveEvent.y } );
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
    vsg::ref_ptr<vsg::Viewer> viewer = _inputHandler->viewer();
    if ( viewer ) {
        viewer->close();
    }
}

auto Handler::collectIntersections( int32_t x, int32_t y )
    -> std::vector<vsg::ref_ptr<vsg::LineSegmentIntersector::Intersection>> {
    auto intersector = vsg::LineSegmentIntersector::create( *_inputHandler->camera().get(), x, y );

    // const auto beforeIntersection = vsg::clock::now();

    auto scenegraph = _inputHandler->scenegraph();
    scenegraph->root()->accept( *intersector );

    // const auto afterIntersection = vsg::clock::now();

    // Sort the intersections front to back.
    std::ranges::sort( intersector->intersections, []( auto& lhs, auto& rhs ) {
        //
        return lhs->ratio < rhs->ratio;
    } );

    return intersector->intersections;
}

void Handler::lineSegmentIntersector( vsg::PointerEvent& pointerEvent ) {
    const auto& intersections = collectIntersections( pointerEvent.x, pointerEvent.y );

    auto scenegraph = _inputHandler->scenegraph();

    if ( intersections.empty() ) {
        scenegraph->bounding()->setTransformMat( vsg::mat4{} );
        scenegraph->scene()->setSelectedObjectUid( QUuid{}.toString() );
        return;
    }

    for ( auto& intersection : intersections ) {
        auto handled = bool{ false };

        for ( auto node : intersection->nodePath ) {
            if ( auto clickedDrawable = dynamic_cast<const SceneObjectGraph*>( node ) ) {
                auto owner = clickedDrawable->owner();
                scenegraph->scene()->setSelectedObjectUid( owner->uid() );
                scenegraph->bounding()->setOnObject( owner );

                handled = true;
            } else if ( auto clickedDrawable = dynamic_cast<const MoveDragger*>( node ) ) {
                std::println( " dragger axis: {}", static_cast<int>( clickedDrawable->axis() ) );

                handled = true;
            }
        }

        if ( handled ) {
            return;
        }
    }
}

}  // namespace tire
