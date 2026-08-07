
#include "scenegraph.h"
#include "scene_object/sceneobjectdata.h"
#include "scene_object/box.h"

namespace tire {

Scenegraph::Scenegraph( vsg::Viewer* viewer, QObject* parent )
    : QObject{ parent }
    , _root{ new vsg::Group{} }
    , _viewer{ viewer }
    , _grid{ new Grid{ viewer, this } }
    , _testbox{ new Testbox{ viewer, this } }
    , _bounding{ new Bounding{ viewer, this } }
    , _sceneObjectSubgraph{ new SceneObjectSubgraph{ viewer } }
    , _markerSubgraph{ new MarkerSubgraph{ viewer } } {
    //

    _root->addChild( _grid->grid() );
    _root->addChild( _testbox->testbox() );
    _root->addChild( _bounding->bounding() );

    _sceneObjectSubgraph->initPipeline();
    _markerSubgraph->initPipeline();

    _root->addChild( _sceneObjectSubgraph );
    _root->addChild( _markerSubgraph );
}

auto Scenegraph::root() const -> vsg::ref_ptr<vsg::Group> {
    return _root;
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
