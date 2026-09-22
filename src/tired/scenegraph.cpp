
#include "scenegraph.h"

namespace tire {

Scenegraph::Scenegraph( vsg::Viewer* viewer, QObject* parent )
    : QObject{ parent }
    , _root{ new vsg::Group{} }
    , _viewer{ viewer } {
    _grid = new Grid{ _viewer, this };
    _navbox = new Navbox{ _viewer, this };
    _bounding = new Bounding{ _viewer, this };
    _scene = new Scene{ _viewer, this };

    _root->addChild( _grid->node() );
    _root->addChild( _bounding->node() );
    _root->addChild( _navbox->node() );
    _root->addChild( _scene->node() );

    connect( _scene, &Scene::selectedObjectChanged, _bounding, &Bounding::onSelectedObjectChanged );
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

auto Scenegraph::gizmo() const -> Gizmo* {
    return _gizmo;
}

auto Scenegraph::scene() const -> Scene* {
    return _scene;
}

void Scenegraph::setGizmoMode( int value ) {
    _gizmoMode = static_cast<GizmoModes>( value );
    emit gizmoModeChanged();
}

int Scenegraph::gizmoMode() {
    return static_cast<int>( _gizmoMode );
}

void Scenegraph::lookChanged( const vsg::dvec3& eye, const vsg::dvec3& cnt, const vsg::dvec3& up ) {
    _navbox->updateViewMatrix( eye, cnt, up );
    _grid->updateCameraPosition( vsg::vec3{ eye } );
}

}  // namespace tire
