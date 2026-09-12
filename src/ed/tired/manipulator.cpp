
#include <QPoint>

#include "manipulator.h"

namespace tire {

// ======================================================================================
// ==================== Manipulator =====================================================
// ======================================================================================

Manipulator::Manipulator( vsg::ref_ptr<vsg::Camera> camera, vsg::ref_ptr<vsg::EllipsoidModel> ellipsoidModel,
                          QObject* parent )
    : QObject{ parent }
    , _trackball{ new Trackball{ this, camera, ellipsoidModel } } {
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

auto Manipulator::lookMatrix() const -> vsg::dmat4 {
    auto eye = _trackball->eye();
    auto center = _trackball->center();
    auto up = _trackball->up();

    // Handle degenerate case (looking straight up/down)
    // if (std::abs(glm::dot(glm::normalize(lookVector), worldUp)) > 0.999f)
    //     worldUp = glm::vec3(0.0f, 0.0f, 1.0f);

    auto forward = vsg::normalize( center - eye );
    auto right = vsg::normalize( vsg::cross( forward, up ) );

    // Orthonormal basis, transposed (inverse rotation) — no translation
    vsg::dmat4 view( 1.0f );
    view[0][0] = right.x;
    view[1][0] = right.y;
    view[2][0] = right.z;
    view[0][1] = up.x;
    view[1][1] = up.y;
    view[2][1] = up.z;
    view[0][2] = -forward.x;
    view[1][2] = -forward.y;
    view[2][2] = -forward.z;

    return view;
}

// ======================================================================================
// ==================== Trackball =======================================================
// ======================================================================================

Trackball::Trackball( Manipulator* owner, vsg::ref_ptr<vsg::Camera> camera,
                      vsg::ref_ptr<vsg::EllipsoidModel> ellipsoidModel )
    : vsg::Trackball{ camera, ellipsoidModel }
    , _owner{ owner } {
    this->supportsThrow = false;
}

void Trackball::apply( vsg::MoveEvent& moveEvent ) {
    _owner->lookAtChanged();
    vsg::Trackball::apply( moveEvent );
}

void Trackball::apply( vsg::ScrollWheelEvent& scrollWheel ) {
    _owner->lookAtChanged();
    vsg::Trackball::apply( scrollWheel );
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

auto Trackball::projection() const -> vsg::dmat4 {
    return vsg::Trackball::_camera->projectionMatrix->transform();
}

}  // namespace tire
