
#include <print>

#include <QPoint>

#include "manipulator.h"

namespace tire {

// ======================================================================================
// ==================== Manipulator =======================================================
// ======================================================================================

Manipulator::Manipulator( vsg::ref_ptr<vsg::Camera> camera, vsg::ref_ptr<vsg::EllipsoidModel> ellipsoidModel,
                          QObject* parent )
    : QObject{ parent }
    , _trackball{ new Trackball{ camera, ellipsoidModel } } {
}

auto Manipulator::trackball() -> const vsg::ref_ptr<Trackball> {
    return _trackball;
}

// ======================================================================================
// ==================== Trackball =======================================================
// ======================================================================================

Trackball::Trackball( vsg::ref_ptr<vsg::Camera> camera, vsg::ref_ptr<vsg::EllipsoidModel> ellipsoidModel )
    : vsg::Trackball{ camera, ellipsoidModel } {
    this->supportsThrow = false;
}

void Trackball::apply( vsg::MoveEvent& moveEvent ) {
    vsg::Trackball::apply( moveEvent );
}

}  // namespace tire
