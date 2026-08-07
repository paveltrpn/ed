
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

private:
    SceneObjectBase *_owner{};
};

}  // namespace tire
