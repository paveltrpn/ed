
#include <print>

#include <QPoint>

#include "manipulator.h"

namespace tire {

Manipulator::Manipulator( vsg::ref_ptr<vsg::Camera> camera, vsg::ref_ptr<vsg::EllipsoidModel> ellipsoidModel,
                          QObject* parent )
    : QObject{ parent }
    , _trackball{ new Trackball{ camera, ellipsoidModel } } {
}

auto Manipulator::trackball() -> const vsg::ref_ptr<Trackball> {
    return _trackball;
}

}  // namespace tire
