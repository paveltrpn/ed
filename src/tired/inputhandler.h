
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

struct Handler final : vsg::Visitor {
    vsg::KeySymbol closeKey = vsg::KEY_Escape;

    Handler( Scenegraph* scenegraph, Manipulator* manipuilator, vsg::observer_ptr<vsg::Camera> camera );

    void apply( vsg::KeyPressEvent& keyPress ) override;
    void apply( vsg::KeyReleaseEvent& keyRelease ) override;
    void apply( vsg::FocusInEvent& focusIn ) override;
    void apply( vsg::FocusOutEvent& focusOut ) override;
    void apply( vsg::ButtonPressEvent& buttonPress ) override;
    void apply( vsg::ButtonReleaseEvent& buttonRelease ) override;
    void apply( vsg::MoveEvent& moveEvent ) override;
    void apply( vsg::ScrollWheelEvent& scrollWheel ) override;
    void apply( vsg::TouchDownEvent& touchDown ) override;
    void apply( vsg::TouchUpEvent& touchUp ) override;
    void apply( vsg::TouchMoveEvent& touchMove ) override;
    void apply( vsg::FrameEvent& frame ) override;
    void apply( vsg::CloseWindowEvent& ) override;
    void apply( vsg::TerminateEvent& ) override;

private:
    void close();

    auto collectIntersections( int32_t x, int32_t y )
        -> std::vector<vsg::ref_ptr<vsg::LineSegmentIntersector::Intersection>>;

    void onLMBPress( vsg::PointerEvent& pointerEvent );
    void onMMBPress( vsg::PointerEvent& pointerEvent );
    void onRMBPress( vsg::PointerEvent& pointerEvent );

    void onLMBRelease( vsg::PointerEvent& pointerEvent );
    void onMMBRelease( vsg::PointerEvent& pointerEvent );
    void onRMBRelease( vsg::PointerEvent& pointerEvent );

private:
    Scenegraph* _scenegraph{};
    Manipulator* _manipulator{};
    vsg::observer_ptr<vsg::Camera> _camera{};

    vsg::ivec2 _dragPressScreen{};
    vsg::dvec2 _dragAxisScreenDir{};
    vsg::dvec3 _dragAxisWorld{};
    DraggerAxis _dragAxis{};
};

}  // namespace tire
