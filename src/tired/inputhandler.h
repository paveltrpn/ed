
#pragma once

#include <QObject>
#include <QPoint>

#include <vsg/all.h>
#include <vsg/app/Viewer.h>
#include <vsg/ui/Keyboard.h>
#include <vsg/ui/PointerEvent.h>
#include <vsg/ui/ScrollWheelEvent.h>
#include <vsg/ui/TouchEvent.h>

#include "scenegraph.h"
#include "manipulator.h"

namespace tire {

// ======================================================================================
// ==================== InputHandler ====================================================
// ======================================================================================

struct Handler;

struct InputHandler final : QObject {
    Q_OBJECT

public:
    InputHandler( Scenegraph* scenegraph, Manipulator* manipuilator, vsg::observer_ptr<vsg::Camera> camera,
                  QObject* parent = nullptr );

    auto handler() -> const vsg::ref_ptr<Handler>;

private:
    vsg::ref_ptr<Handler> _handler{};
};

// ======================================================================================
// ==================== Handler =========================================================
// ======================================================================================

enum class HandlerClickStates {
    //
    UNDEFINED,
    PRESSED,
    VIEW_MOVED,
    INTERSECTED
};

struct Handler final : vsg::Visitor {
    vsg::KeySymbol closeKey = vsg::KEY_Escape;

    Handler( Scenegraph* scenegraph, Manipulator* manipuilator, vsg::observer_ptr<vsg::Camera> camera );

    auto apply( vsg::KeyPressEvent& keyPress ) -> void override;
    auto apply( vsg::KeyReleaseEvent& keyRelease ) -> void override;
    auto apply( vsg::FocusInEvent& focusIn ) -> void override;
    auto apply( vsg::FocusOutEvent& focusOut ) -> void override;
    auto apply( vsg::ButtonPressEvent& buttonPress ) -> void override;
    auto apply( vsg::ButtonReleaseEvent& buttonRelease ) -> void override;
    auto apply( vsg::MoveEvent& moveEvent ) -> void override;
    auto apply( vsg::ScrollWheelEvent& scrollWheel ) -> void override;
    auto apply( vsg::TouchDownEvent& touchDown ) -> void override;
    auto apply( vsg::TouchUpEvent& touchUp ) -> void override;
    auto apply( vsg::TouchMoveEvent& touchMove ) -> void override;
    auto apply( vsg::FrameEvent& frame ) -> void override;
    auto apply( vsg::CloseWindowEvent& ) -> void override;
    auto apply( vsg::TerminateEvent& ) -> void override;

private:
    auto close() -> void;

    auto collectIntersections( int32_t x, int32_t y )
        -> std::vector<vsg::ref_ptr<vsg::LineSegmentIntersector::Intersection>>;

    auto onLMBPress( vsg::PointerEvent& pointerEvent ) -> void;
    auto beginGizmoDrag( const Dragger* dragger, GizmoType type, vsg::PointerEvent& pointerEvent ) -> void;

    auto onMMBPress( vsg::PointerEvent& pointerEvent ) -> void;
    auto onRMBPress( vsg::PointerEvent& pointerEvent ) -> void;

    auto onLMBRelease( vsg::PointerEvent& pointerEvent ) -> void;
    auto onMMBRelease( vsg::PointerEvent& pointerEvent ) -> void;
    auto onRMBRelease( vsg::PointerEvent& pointerEvent ) -> void;

private:
    Scenegraph* _scenegraph{};
    Manipulator* _manipulator{};
    vsg::observer_ptr<vsg::Camera> _camera{};

    vsg::ivec2 _dragPressScreen{};
    vsg::dvec2 _dragAxisScreenDir{};
    vsg::dvec3 _dragAxisWorld{};
    DraggerAxis _dragAxis{};
    GizmoType _dragGizmoType{ GizmoType::MOVE };
    vsg::dvec2 _dragGizmoCenterScreen{};
    vsg::dvec2 _dragLastScreenPos{};
    double _dragAngleAccum{};
    double _dragRotateSign{ 1.0 };

    HandlerClickStates _clickState{ HandlerClickStates::UNDEFINED };
};

}  // namespace tire
