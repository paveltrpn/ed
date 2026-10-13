
#pragma once

#include <vsg/all.h>
#include "vsg/core/ref_ptr.h"

#include "sceneobjectdata.h"
#include "../subgraph/boundingdraw.h"

namespace tire {

struct SceneObjectBase;
struct GizmoUIProxy;

// ======================================================================================
// ==================== SceneObjectNode =================================================
// ======================================================================================

struct SceneObjectNode : public vsg::MatrixTransform {
public:
    friend SceneObjectBase;
    friend GizmoUIProxy;

    SceneObjectNode( SceneObjectTypeEnum type, std::string name, std::string uid, vsg::dvec3 position,
                     vsg::dvec3 orientation, vsg::dvec3 scale, vsg::vec4 color );

    auto setOwner( SceneObjectBase* owner ) -> void {
        //
        _owner = owner;
    };

    [[nodiscard]] auto owner() const -> SceneObjectBase*;

    [[nodiscard]] auto boundingDraw() const -> vsg::ref_ptr<BoundingDraw>;

    auto setOrigin( vsg::dvec3 value ) -> void;

    auto setRotation( vsg::dvec3 value ) -> void;
    auto setRotation( double yaw, double pitch, double roll ) -> void;
    auto setRotation( vsg::dvec3 ax, double angl ) -> void;

    auto setScale( vsg::dvec3 value ) -> void;

private:
    auto updateMatrix() -> void;

private:
    SceneObjectTypeEnum _type{};
    std::string _name{};
    std::string _uid{};
    vsg::dvec3 _position{};
    vsg::dvec3 _orientation{};
    vsg::dvec3 _scale{};
    vsg::vec4 _color{};

private:
    SceneObjectBase* _owner{};

    vsg::ref_ptr<BoundingDraw> _bounding{};

    vsg::dmat4 _originMatrix{};
    vsg::dmat4 _rotationMatrix{};
    vsg::dmat4 _scaleMatrix{};
};

}  // namespace tire
