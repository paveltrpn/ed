
#include "inputhandler.h"
#include "sceneobject/sceneobjectnode.h"

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

auto Handler::apply( vsg::KeyPressEvent& keyPress ) -> void {
    if ( closeKey != vsg::KEY_Undefined && keyPress.keyBase == closeKey ) {
        close();
    }
}

auto Handler::apply( vsg::KeyReleaseEvent& keyRelease ) -> void {
}

auto Handler::apply( vsg::FocusInEvent& focusIn ) -> void {
}

auto Handler::apply( vsg::FocusOutEvent& focusOut ) -> void {
}

auto Handler::apply( vsg::ButtonPressEvent& buttonPress ) -> void {
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

        default: {
            break;
        }
    }
}

auto Handler::apply( vsg::ButtonReleaseEvent& buttonRelease ) -> void {
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

        default: {
            break;
        }
    }
}

auto Handler::apply( vsg::MoveEvent& moveEvent ) -> void {
    switch ( _clickState ) {
        case HandlerClickStates::INTERSECTED: {
            _clickState = HandlerClickStates::UNDEFINED;
            break;
        }

        case HandlerClickStates::PRESSED: {
            _clickState = HandlerClickStates::VIEW_MOVED;
            break;
        }

        default: {
            break;
        }
    }

    if ( _manipulator->dragActive() ) {
        switch ( _dragGizmoType ) {
            case GizmoType::MOVE:
            case GizmoType::SCALE: {
                const auto mouseDisplacement = vsg::dvec2{ static_cast<double>( moveEvent.x ) - _dragPressScreen.x,
                                                           static_cast<double>( moveEvent.y ) - _dragPressScreen.y };

                // Project the mouse displacement onto the axis's screen direction: the gizmo follows the
                // cursor 1:1 along the projected axis, whatever the camera distance or orientation.
                const auto dirLenSq = vsg::dot( _dragAxisScreenDir, _dragAxisScreenDir );

                if ( dirLenSq > std::numeric_limits<double>::epsilon() ) {
                    const auto delta = vsg::dot( mouseDisplacement, _dragAxisScreenDir ) / dirLenSq;

                    if ( _dragGizmoType == GizmoType::MOVE ) {
                        _scenegraph->gizmo()->update( _dragAxisWorld * delta );
                    } else {
                        _scenegraph->gizmo()->updateScale( _dragAxis, delta );
                    }
                }

                break;
            }

            case GizmoType::ROTATE: {
                const auto currentPos =
                    vsg::dvec2{ static_cast<double>( moveEvent.x ), static_cast<double>( moveEvent.y ) };

                const auto prev = _dragLastScreenPos - _dragGizmoCenterScreen;
                const auto cur = currentPos - _dragGizmoCenterScreen;

                if ( vsg::length( prev ) < 4.0 || vsg::length( cur ) < 4.0 ) {
                    return;
                }

                const auto dAngle =
                    vsg::degrees( std::atan2( prev.x * cur.y - prev.y * cur.x, vsg::dot( prev, cur ) ) );

                _dragAngleAccum += _dragRotateSign * dAngle;
                _dragLastScreenPos = currentPos;

                _scenegraph->gizmo()->updateRotate( _dragAxis, _dragAngleAccum );

                break;
            }
        }
    }
}

auto Handler::apply( vsg::ScrollWheelEvent& scrollWheel ) -> void {
}

auto Handler::apply( vsg::TouchDownEvent& touchDown ) -> void {
}

auto Handler::apply( vsg::TouchUpEvent& touchUp ) -> void {
}

auto Handler::apply( vsg::TouchMoveEvent& touchMove ) -> void {
}

auto Handler::apply( vsg::FrameEvent& frame ) -> void {
}

auto Handler::apply( vsg::CloseWindowEvent& ) -> void {
    close();
}

auto Handler::apply( vsg::TerminateEvent& ) -> void {
    close();
}

auto Handler::close() -> void {
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

auto Handler::onLMBPress( vsg::PointerEvent& pointerEvent ) -> void {
    const auto& intersections = collectIntersections( pointerEvent.x, pointerEvent.y );

    auto scene = _scenegraph->scene();

    if ( intersections.empty() ) {
        _clickState = HandlerClickStates::PRESSED;
        return;
    }

    // The gizmo renders on top of everything else (depth is cleared before it),
    // so a dragger visible on screen must win the pick even when scene geometry
    // is closer along the ray. Check draggers in a dedicated first pass.
    for ( auto& intersection : intersections ) {
        for ( auto node : intersection->nodePath ) {
            if ( auto clickedDragger = dynamic_cast<const Dragger*>( node ) ) {
                clickedDragger->setSelected();

                if ( dynamic_cast<const MoveDragger*>( node ) ) {
                    beginGizmoDrag( clickedDragger, GizmoType::MOVE, pointerEvent );
                } else if ( dynamic_cast<const RotationDragger*>( node ) ) {
                    beginGizmoDrag( clickedDragger, GizmoType::ROTATE, pointerEvent );
                } else if ( dynamic_cast<const ScaleDragger*>( node ) ) {
                    beginGizmoDrag( clickedDragger, GizmoType::SCALE, pointerEvent );
                }

                _clickState = HandlerClickStates::INTERSECTED;

                return;
            }
        }
    }

    for ( auto& intersection : intersections ) {
        for ( auto node : intersection->nodePath ) {
            if ( auto clickedDrawable = dynamic_cast<const SceneObjectNode*>( node ) ) {
                auto owner = clickedDrawable->owner();
                scene->setSelectedObjectUid( owner->uid() );

                _clickState = HandlerClickStates::INTERSECTED;

                return;
            }
        }
    }
}

auto Handler::beginGizmoDrag( const Dragger* dragger, GizmoType type, vsg::PointerEvent& pointerEvent ) -> void {
    auto gizmo = _scenegraph->gizmo();

    _manipulator->setDragActive( true );

    _dragAxis = dragger->axis();

    _dragGizmoType = type;

    gizmo->beginDrag();

    _dragPressScreen = vsg::ivec2{ pointerEvent.x, pointerEvent.y };

    _dragAxisWorld = gizmo->dragAxisWorld( _dragAxis );

    const auto anchor = gizmo->translation();

    auto worldToScreen = [this]( const vsg::dvec3& worldPoint ) -> vsg::dvec2 {
        const auto camera = _camera.get();

        const auto viewport = camera->getViewport();

        // Same projection/viewport convention as vsg::LineSegmentIntersector picking.
        const auto clip = camera->projectionMatrix->transform() * camera->viewMatrix->transform() *
                          vsg::dvec4{ worldPoint.x, worldPoint.y, worldPoint.z, 1.0 };

        const auto ndcX = clip.x / clip.w * 0.5 + 0.5;
        const auto ndcY = clip.y / clip.w * 0.5 + 0.5;

        return { viewport.x + ndcX * viewport.width, viewport.y + ndcY * viewport.height };
    };

    _dragAxisScreenDir = worldToScreen( anchor + _dragAxisWorld ) - worldToScreen( anchor );

    if ( type == GizmoType::ROTATE ) {
        _dragGizmoCenterScreen = worldToScreen( anchor );
        _dragLastScreenPos = vsg::dvec2{ static_cast<double>( pointerEvent.x ), static_cast<double>( pointerEvent.y ) };
        _dragAngleAccum = 0.0;

        // VSG camera looks down -Z in view space; axis pointing away from the camera means
        // screen-CCW cursor motion is a positive right-hand rotation about that axis.
        const auto axisView = _camera.get()->viewMatrix->transform() *
                              vsg::dvec4{ _dragAxisWorld.x, _dragAxisWorld.y, _dragAxisWorld.z, 0.0 };

        _dragRotateSign = axisView.z < 0.0 ? 1.0 : -1.0;
    }
}

auto Handler::onMMBPress( vsg::PointerEvent& pointerEvent ) -> void {
}

auto Handler::onRMBPress( vsg::PointerEvent& pointerEvent ) -> void {
}

auto Handler::onLMBRelease( vsg::PointerEvent& pointerEvent ) -> void {
    switch ( _clickState ) {
        case HandlerClickStates::PRESSED: {
            auto scene = _scenegraph->scene();
            scene->setSelectedObjectUid( QUuid{}.toString() );
            _clickState = HandlerClickStates::UNDEFINED;
            break;
        }

        default: {
            _clickState = HandlerClickStates::UNDEFINED;
            break;
        }
    }

    if ( _manipulator->dragActive() ) {
        _manipulator->setDragActive( false );
        _scenegraph->gizmo()->setAllDraggersDeselected();
    }
}

auto Handler::onMMBRelease( vsg::PointerEvent& pointerEvent ) -> void {
}

auto Handler::onRMBRelease( vsg::PointerEvent& pointerEvent ) -> void {
}

}  // namespace tire
