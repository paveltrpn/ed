
#pragma once

#include <QObject>
#include <QPoint>

#include <vsg/all.h>
#include <vsg/app/Viewer.h>
#include <vsg/ui/Keyboard.h>
#include <vsg/ui/PointerEvent.h>
#include <vsg/ui/ScrollWheelEvent.h>
#include <vsg/ui/TouchEvent.h>

#include "trackball.h"
#include "inputhandler.h"

namespace tire {

struct Manipulator final : QObject {
    Q_OBJECT

public:
    Manipulator( vsg::ref_ptr<vsg::Camera> camera, vsg::ref_ptr<vsg::EllipsoidModel> ellipsoidModel = {},
                 QObject* parent = nullptr );

    auto trackball() -> const vsg::ref_ptr<Trackball>;

private:
    vsg::ref_ptr<Trackball> _trackball{};

    QPoint mousePos_{};
};

}  // namespace tire
