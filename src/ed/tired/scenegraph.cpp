
#include "scenegraph.h"
#include "scene_object/sceneobjectdata.h"
#include "scene_object/box.h"

namespace tire {

Scenegraph::Scenegraph( vsg::Viewer* viewer, QObject* parent )
    : QObject{ parent }
    , _root{ new vsg::Group{} }
    , _viewer{ viewer } {
}

auto Scenegraph::initSubgraphs() -> void {
    _grid = new Grid{ _viewer, this };
    _testbox = new Testbox{ _viewer, this };
    _bounding = new Bounding{ _viewer, this };
    _markerSubgraph = new MarkerSubgraph{ _viewer };
    _sceneObjectSubgraph = new SceneObjectSubgraph{ _viewer };

    _markerSubgraph->initPipeline();
    _sceneObjectSubgraph->initPipeline();

    _root->addChild( _sceneObjectSubgraph );
    _root->addChild( _bounding->node() );
    _root->addChild( _grid->node() );
    _root->addChild( _testbox->node() );
    _root->addChild( _markerSubgraph );
}

auto Scenegraph::root() const -> vsg::ref_ptr<vsg::Group> {
    return _root;
}

auto Scenegraph::testbox() const -> Testbox* {
    return _testbox;
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

    _sceneObjectSubgraph->link( exbox );
    _objectsList.push_back( std::move( exbox ) );
}

}  // namespace tire
