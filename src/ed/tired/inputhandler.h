
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

namespace tire {

struct InputHandler;

struct Handler final : vsg::Visitor {
    vsg::KeySymbol closeKey = vsg::KEY_Escape;

    Handler( InputHandler* inputHandler );

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
    void lineSegmentIntersector( vsg::PointerEvent& pointerEvent );

private:
    InputHandler* _inputHandler{};
};

// =======================================================================

struct InputHandler final : QObject {
    Q_OBJECT

    Q_PROPERTY( QPoint mousePos READ mousePos WRITE setMousePos NOTIFY mousePosUpdated )

public:
    InputHandler( vsg::ref_ptr<vsg::Camera> camera, vsg::ref_ptr<vsg::Viewer> viwer, Scenegraph* scenegraph,
                  QObject* parent = nullptr );

    auto handler() -> const vsg::ref_ptr<Handler>;
    auto camera() -> vsg::ref_ptr<vsg::Camera>;
    auto viewer() -> const vsg::ref_ptr<vsg::Viewer>;

    auto scenegraph() -> const Scenegraph*;

public:
    void setMousePos( QPoint value );
    QPoint mousePos();

signals:
    void mousePosUpdated();

private:
    vsg::ref_ptr<Handler> _handler{};

    // TODO: We not owning this, use raw pointer!
    vsg::ref_ptr<vsg::Camera> _camera{};
    vsg::ref_ptr<vsg::Viewer> _viewer{};

    Scenegraph* _scenegraph{};

    QPoint mousePos_{};
};

}  // namespace tire
