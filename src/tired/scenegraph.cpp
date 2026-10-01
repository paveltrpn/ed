
#include "scenegraph.h"
#include "tired/subgraph/boundingrender.h"

namespace tire {

Scenegraph::Scenegraph( vsg::observer_ptr<vsg::Viewer> viewer, QObject* parent )
    : QObject{ parent }
    , _root{ new vsg::Group{} }
    , _viewer{ viewer }
    , _boundingRender{ new BoundingRender{ _viewer } }
    , _gridNode{ new GridSubgraph{ _viewer } }
    , _grid{ new GridUIProxy{ vsg::observer_ptr<GridSubgraph>{ _gridNode }, this } }
    , _navboxNode{ new NavboxSubgraph{ _viewer } }
    , _navbox{ new NavboxUIProxy{ vsg::observer_ptr<NavboxSubgraph>{ _navboxNode }, this } }
    , _gizmoNode{ new GizmoSubgraph{ _viewer } }
    , _gizmo{ new GizmoUIProxy{ vsg::observer_ptr<GizmoSubgraph>{ _gizmoNode }, this } }
    , _sceneNode{ new SceneSubgraph{ _viewer } }
    , _scene{ new SceneUIProxy{ vsg::observer_ptr<SceneSubgraph>{ _sceneNode },
                                vsg::observer_ptr<BoundingRender>{ _boundingRender }, this } } {
    //
    _root->addChild( _boundingRender );

    _root->addChild( _navboxNode );
    _root->addChild( _sceneNode );
    _root->addChild( _gridNode );
    _root->addChild( _gizmoNode );

    connect( _scene, &SceneUIProxy::selectedObjectChanged, _gizmo, [this]( SceneObjectBase* object ) {
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

auto Scenegraph::navbox() const -> NavboxUIProxy* {
    return _navbox;
}

auto Scenegraph::grid() const -> GridUIProxy* {
    return _grid;
}

auto Scenegraph::gizmo() const -> GizmoUIProxy* {
    return _gizmo;
}

auto Scenegraph::scene() const -> SceneUIProxy* {
    return _scene;
}

void Scenegraph::lookChanged( const vsg::dvec3& eye, const vsg::dvec3& cnt, const vsg::dvec3& up ) {
    _navbox->updateViewMatrix( eye, cnt, up );
    _grid->updateCameraPosition( vsg::vec3{ eye } );
}

}  // namespace tire
