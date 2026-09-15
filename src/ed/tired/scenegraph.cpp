
#include "scenegraph.h"
#include "scene_object/sceneobjectdata.h"
#include "scene_object/box.h"

namespace tire {

Scenegraph::Scenegraph( vsg::Viewer* viewer, QObject* parent )
    : QObject{ parent }
    , _root{ new vsg::Group{} }
    , _viewer{ viewer } {
    _grid = new Grid{ _viewer, this };
    _navbox = new Navbox{ _viewer, this };
    _bounding = new Bounding{ _viewer, this };
    _markerSubgraph = new MarkerSubgraph{ _viewer };
    _sceneObjects = new SceneObjects{ _viewer, this };

    _root->addChild( _sceneObjects->node() );
    _root->addChild( _bounding->node() );
    _root->addChild( _grid->node() );
    _root->addChild( _navbox->node() );

    _markerSubgraph->initPipeline();
    _root->addChild( _markerSubgraph );
}

auto Scenegraph::root() const -> vsg::ref_ptr<vsg::Group> {
    return _root;
}

auto Scenegraph::navbox() const -> Navbox* {
    return _navbox;
}

auto Scenegraph::grid() const -> Grid* {
    return _grid;
}

auto Scenegraph::bounding() const -> Bounding* {
    return _bounding;
}

void Scenegraph::addExBox( float px, float py, float pz, float rx, float ry, float rz, float sx, float sy, float sz ) {
    auto data = BoxObjectData{};
    data.setPosition( { px, py, pz } );
    data.setOrientation( { rx, ry, rz } );
    data.setScale( { sx, sy, sz } );

    auto exbox = std::make_shared<object::Box>( data );

    _sceneObjects->node()->link( exbox );
}

void Scenegraph::setGizmoMode( int value ) {
    _gizmoMode = static_cast<GizmoModes>( value );
}

int Scenegraph::gizmoMode() {
    return static_cast<int>( _gizmoMode );
}

void Scenegraph::lookChanged( const vsg::dvec3& eye, const vsg::dvec3& cnt, const vsg::dvec3& up ) {
    _navbox->updateViewMatrix( eye, cnt, up );
    _grid->updateCameraPosition( vsg::vec3{ eye } );
}

}  // namespace tire
