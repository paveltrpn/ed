
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

QVector3D Manipulator::eye() const {
    return QVector3D{ static_cast<float>( _trackball->eye().x ), static_cast<float>( _trackball->eye().y ),
                      static_cast<float>( _trackball->eye().z ) };
}

QVector3D Manipulator::center() const {
    return QVector3D{ static_cast<float>( _trackball->center().x ), static_cast<float>( _trackball->center().y ),
                      static_cast<float>( _trackball->center().z ) };
}

QVector3D Manipulator::up() const {
    return QVector3D{ static_cast<float>( _trackball->up().x ), static_cast<float>( _trackball->up().y ),
                      static_cast<float>( _trackball->up().z ) };
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

auto Trackball::eye() const -> vsg::dvec3 {
    return vsg::Trackball::_lookAt->eye;
}

auto Trackball::center() const -> vsg::dvec3 {
    return vsg::Trackball::_lookAt->center;
}

auto Trackball::up() const -> vsg::dvec3 {
    return vsg::Trackball::_lookAt->up;
}

}  // namespace tire
