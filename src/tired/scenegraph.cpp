
#include "scenegraph.h"
#include "tired/subgraph/boundingrender.h"

namespace tire {

Scenegraph::Scenegraph( vsg::observer_ptr<vsg::Viewer> viewer, QObject* parent )
    : QObject{ parent }
    , _root{ new vsg::Group{} }
    , _viewer{ viewer }
    , _boundingRender{ new BoundingRender{ _viewer } }
    , _grid{ new Grid{ _viewer, this } }
    , _navbox{ new Navbox{ _viewer, this } }
    , _gizmo{ new Gizmo{ _viewer, this } }
    , _scene{ new Scene{ _viewer, vsg::observer_ptr<BoundingRender>{ _boundingRender }, this } } {
    //
    _root->addChild( _boundingRender );

    _root->addChild( _navbox->node() );
    _root->addChild( _scene->scene() );
    _root->addChild( _grid->node() );
    _root->addChild( _gizmo->node() );

    connect( _scene, &Scene::selectedObjectChanged, _gizmo, [this]( SceneObjectBase* object ) {
        //
        const auto pos = object->position();
        _gizmo->setTranslation( vsg::dvec3{ pos.x(), pos.y(), pos.z() } );
    } );
}

auto Scenegraph::root() const -> vsg::ref_ptr<vsg::Group> {
    return _root;
}

auto Scenegraph::scenegraphViewer() const -> vsg::observer_ptr<vsg::Viewer> {
    return _viewer;
}

auto Scenegraph::navbox() const -> Navbox* {
    return _navbox;
}

auto Scenegraph::grid() const -> Grid* {
    return _grid;
}

auto Scenegraph::gizmo() const -> Gizmo* {
    return _gizmo;
}

auto Scenegraph::scene() const -> Scene* {
    return _scene;
}

void Scenegraph::lookChanged( const vsg::dvec3& eye, const vsg::dvec3& cnt, const vsg::dvec3& up ) {
    _navbox->updateViewMatrix( eye, cnt, up );
    _grid->updateCameraPosition( vsg::vec3{ eye } );
}

}  // namespace tire
