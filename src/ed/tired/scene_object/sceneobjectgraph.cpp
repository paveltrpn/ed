
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

}  // namespace tire
