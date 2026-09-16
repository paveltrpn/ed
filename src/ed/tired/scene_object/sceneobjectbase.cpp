
#include "sceneobjectbase.h"

namespace tire {

SceneObjectBase::SceneObjectBase( QObject* parent )
    : QObject{ parent }
    , _node{ new SceneObjectGraph{ this } } {
}

SceneObjectBase::SceneObjectBase( const SceneObjectData& data )
    : _node{ new SceneObjectGraph{ this } }
    , _type{ data._type }
    , _name{ data._name }
    , _uid{ data._uid }
    , _position{ data._position }
    , _orientation{ data._orientation }
    , _scale{ data._scale }
    , _color{ data._color } {
}

SceneObjectBase::SceneObjectBase( SceneObjectTypeEnum type, const QString& name, const QUuid& uid, vsg::dvec3 position,
                                  vsg::dvec3 orientation, vsg::dvec3 scale, vsg::dvec4 color )
    : _node{ new SceneObjectGraph{ this } }
    , _type{ type }
    , _name{ name }
    , _uid{ uid }
    , _position{ position }
    , _orientation{ orientation }
    , _scale{ scale }
    , _color{ color } {
}

auto SceneObjectBase::node() const -> vsg::ref_ptr<SceneObjectGraph> {
    return _node;
}

}  // namespace tire
