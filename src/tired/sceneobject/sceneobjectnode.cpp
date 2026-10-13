
#include "sceneobjectnode.h"
#include "sceneobjectbase.h"
#include "tired/subgraph/boundingdraw.h"

namespace tire {

// ======================================================================================
// ==================== SceneObjectNode =================================================
// ======================================================================================

SceneObjectNode::SceneObjectNode( SceneObjectBase* owner )
    : _owner{ owner }
    , _bounding{ new BoundingDraw{} } {
    //
    _bounding->setTransform( vsg::mat4{ this->matrix } );
}

auto SceneObjectNode::owner() const -> SceneObjectBase* {
    return _owner;
}

auto SceneObjectNode::boundingDraw() const -> vsg::ref_ptr<BoundingDraw> {
    return _bounding;
}

auto SceneObjectNode::setOrigin( vsg::dvec3 value ) -> void {
    _origin = vsg::translate( value );
    updateMatrix();
}

auto SceneObjectNode::setRotation( vsg::dvec3 value ) -> void {
    const auto rtX = vsg::rotate( vsg::radians( value.x ), vsg::dvec3{ 1.0, 0.0, 0.0 } );
    const auto rtY = vsg::rotate( vsg::radians( value.y ), vsg::dvec3{ 0.0, 1.0, 0.0 } );
    const auto rtZ = vsg::rotate( vsg::radians( value.z ), vsg::dvec3{ 0.0, 0.0, 1.0 } );

    _rotation = rtX * rtY * rtZ;

    updateMatrix();
}

auto SceneObjectNode::setRotation( double yaw, double pitch, double roll ) -> void {
    const auto tmp = vsg::dvec3{ yaw, pitch, roll };
    this->setRotation( tmp );
}

auto SceneObjectNode::setRotation( vsg::dvec3 ax, double angl ) -> void {
    // TODO!
    updateMatrix();
}

auto SceneObjectNode::setScale( vsg::dvec3 value ) -> void {
    _scale = vsg::scale( value );
    updateMatrix();
}

auto SceneObjectNode::updateMatrix() -> void {
    this->matrix = _origin * _rotation * _scale;
    _bounding->setTransform( vsg::mat4{ this->matrix } );
}

}  // namespace tire
