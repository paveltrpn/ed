
#include "sceneobjectbase.h"

namespace tire {

SceneObjectBase::SceneObjectBase( QObject* parent )
    : QObject{ parent }
    , _thisObject{ new SceneObjectGraph{ this } } {
}

auto SceneObjectBase::root() const -> vsg::ref_ptr<SceneObjectGraph> {
    return _thisObject;
}

}  // namespace tire
