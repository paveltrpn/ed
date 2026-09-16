
#include "sceneobjectbase.h"

namespace tire {

SceneObjectBase::SceneObjectBase( QObject* parent )
    : QObject{ parent }
    , _node{ new SceneObjectGraph{ this } } {
}

auto SceneObjectBase::node() const -> vsg::ref_ptr<SceneObjectGraph> {
    return _node;
}

}  // namespace tire
