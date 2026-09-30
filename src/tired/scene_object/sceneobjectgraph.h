
#pragma once

#include <vsg/all.h>
#include "vsg/core/ref_ptr.h"

#include "../subgraph/boundingdraw.h"

namespace tire {

struct SceneObjectBase;

struct SceneObjectGraph : public vsg::MatrixTransform {
public:
    SceneObjectGraph( SceneObjectBase* owner );

    [[nodiscard]]
    auto owner() const -> SceneObjectBase*;

    [[nodiscard]]
    auto boundingDraw() const -> vsg::ref_ptr<BoundingDraw>;

    auto setOrigin( vsg::dvec3 value ) -> void;

    auto setRotation( vsg::dvec3 value ) -> void;
    auto setRotation( double yaw, double pitch, double roll ) -> void;
    auto setRotation( vsg::dvec3 ax, double angl ) -> void;

    auto setScale( vsg::dvec3 value ) -> void;

private:
    auto updateMatrix() -> void;

private:
    SceneObjectBase* _owner{};

    vsg::ref_ptr<BoundingDraw> _bounding{};

    vsg::dmat4 _origin{};
    vsg::dmat4 _rotation{};
    vsg::dmat4 _scale{};
};

}  // namespace tire
