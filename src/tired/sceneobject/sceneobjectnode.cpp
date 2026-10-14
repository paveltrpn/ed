
#include "sceneobjectnode.h"
#include "sceneobjectbase.h"
#include "tired/subgraph/boundingdraw.h"

namespace tire {

// ======================================================================================
// ==================== SceneObjectNode =================================================
// ======================================================================================

SceneObjectNode::SceneObjectNode( SceneObjectTypeEnum type, std::string name, std::string uid, vsg::dvec3 position,
                                  vsg::dvec3 orientation, vsg::dvec3 scale, vsg::vec4 color )
    : _type{ type }
    , _name{ name }
    , _uid{ uid }
    , _position{ position }
    , _orientation{ orientation }
    , _scale{ scale }
    , _color{ color }
    , _bounding{ new BoundingDraw{} } {
    //
    setOrigin( _position );
    setRotation( _orientation );
    setScale( _scale );
}

auto SceneObjectNode::owner() const -> SceneObjectBase* {
    return _owner;
}

auto SceneObjectNode::boundingDraw() -> vsg::ref_ptr<BoundingDraw> {
    updateBoundingDraw();
    return _bounding;
}

auto SceneObjectNode::setOrigin( vsg::dvec3 value ) -> void {
    _originMatrix = vsg::translate( value );
    updateMatrix();
}

auto SceneObjectNode::setRotation( vsg::dvec3 value ) -> void {
    const auto rtX = vsg::rotate( vsg::radians( value.x ), vsg::dvec3{ 1.0, 0.0, 0.0 } );
    const auto rtY = vsg::rotate( vsg::radians( value.y ), vsg::dvec3{ 0.0, 1.0, 0.0 } );
    const auto rtZ = vsg::rotate( vsg::radians( value.z ), vsg::dvec3{ 0.0, 0.0, 1.0 } );

    _rotationMatrix = rtX * rtY * rtZ;

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
    _scaleMatrix = vsg::scale( value );
    updateMatrix();
}

auto SceneObjectNode::updateMatrix() -> void {
    this->matrix = _originMatrix * _rotationMatrix * _scaleMatrix;
    updateBoundingDraw();
}

auto SceneObjectNode::computeLocalBounds() -> void {
    if ( _localBoundsValid ) {
        return;
    }

    vsg::ComputeBounds cb{};
    for ( const auto& child : children ) {
        child->accept( cb );
    }

    if ( cb.bounds.valid() ) {
        _localBounds = cb.bounds;
        _localBoundsValid = true;
    }
}

auto SceneObjectNode::updateBoundingDraw() -> void {
    computeLocalBounds();

    auto bounds = _localBounds;
    if ( !_localBoundsValid ) {
        bounds = vsg::dbox{ { -0.5, -0.5, -0.5 }, { 0.5, 0.5, 0.5 } };
    }

    vsg::dbox world{};
    for ( auto i = 0; i < 8; ++i ) {
        const vsg::dvec3 corner{ ( i & 1 ) ? bounds.max.x : bounds.min.x,
                                 ( i & 2 ) ? bounds.max.y : bounds.min.y,
                                 ( i & 4 ) ? bounds.max.z : bounds.min.z };
        world.add( this->matrix * corner );
    }

    const auto center = ( world.min + world.max ) * 0.5;
    auto extent = world.max - world.min;
    extent.x = std::max( extent.x, 1e-6 );
    extent.y = std::max( extent.y, 1e-6 );
    extent.z = std::max( extent.z, 1e-6 );

    _bounding->setTransform( vsg::mat4{ vsg::translate( center ) * vsg::scale( extent ) } );
}

}  // namespace tire
