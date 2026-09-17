
#pragma once

#include <vsg/all.h>

namespace tire {

struct SceneObjectBase;

struct SceneObjectGraph : public vsg::MatrixTransform {
public:
    SceneObjectGraph( SceneObjectBase *owner );

    auto owner() const -> SceneObjectBase *;

    auto dmatrix() const -> vsg::dmat4;
    auto fmatrix() const -> vsg::mat4;

    auto setOrigin( vsg::dvec3 value ) -> void;

    auto setRotation( vsg::dvec3 value ) -> void;
    auto setRotation( double yaw, double pitch, double roll ) -> void;
    auto setRotation( vsg::dvec3 ax, double angl ) -> void;

    auto setScale( vsg::dvec3 value ) -> void;

private:
    auto updateMatrix() -> void;

private:
    SceneObjectBase *_owner{};

    vsg::dmat4 _origin{};
    vsg::dmat4 _rotation{};
    vsg::dmat4 _scale{};
};

}  // namespace tire
