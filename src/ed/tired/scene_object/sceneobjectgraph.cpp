
#include "sceneobjectgraph.h"
#include "sceneobjectbase.h"

namespace tire {

SceneObjectGraph::SceneObjectGraph( SceneObjectBase* owner )
    : _owner{ owner } {
}

auto SceneObjectGraph::owner() const -> SceneObjectBase* {
    return _owner;
}

auto SceneObjectGraph::dmatrix() const -> vsg::dmat4 {
    return this->matrix;
}

auto SceneObjectGraph::fmatrix() const -> vsg::mat4 {
    return static_cast<vsg::mat4>( this->matrix );
}

auto SceneObjectGraph::setOrigin( vsg::dvec3 value ) -> void {
    _origin = vsg::translate( value );
    updateMatrix();
}

auto SceneObjectGraph::setRotation( vsg::dvec3 value ) -> void {
    const auto rtX = vsg::rotate( vsg::radians( value.x ), vsg::dvec3{ 1.0, 0.0, 0.0 } );
    const auto rtY = vsg::rotate( vsg::radians( value.y ), vsg::dvec3{ 0.0, 1.0, 0.0 } );
    const auto rtZ = vsg::rotate( vsg::radians( value.z ), vsg::dvec3{ 0.0, 0.0, 1.0 } );

    _rotation = rtX * rtY * rtZ;

    updateMatrix();
}

auto SceneObjectGraph::setRotation( double yaw, double pitch, double roll ) -> void {
    const auto tmp = vsg::dvec3{ yaw, pitch, roll };
    this->setRotation( tmp );
}

auto SceneObjectGraph::setRotation( vsg::dvec3 ax, double angl ) -> void {
    // TODO!
    updateMatrix();
}

auto SceneObjectGraph::setScale( vsg::dvec3 value ) -> void {
    _scale = vsg::scale( value );
    updateMatrix();
}

auto SceneObjectGraph::updateMatrix() -> void {
    this->matrix = _origin * _rotation * _scale;
}

}  // namespace tire
