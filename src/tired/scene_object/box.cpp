
#include <vsg/all.h>

#include "generator/BoxMesh.hpp"

#include "box.h"
#include "../generatorutils.h"

namespace tire::object {

Box::Box( const BoxObjectData& data )
    : SceneObjectBase{ data._type, data._name, data._uid, data._position, data._orientation, data._scale, data._color }
    , _width{ data._width }
    , _height{ data._height }
    , _depth{ data._depth } {
    init();
}

Box::Box( SceneObjectTypeEnum type, const QString& name, const QUuid& uid, vsg::dvec3 position, vsg::dvec3 orientation,
          vsg::dvec3 scale, vsg::dvec4 color, double width, double height, double depth )
    : SceneObjectBase{ type, name, uid, position, orientation, scale, color }
    , _width{ width }
    , _height{ height }
    , _depth{ depth } {
}

Box::Box( QObject* parent )
    : SceneObjectBase{} {
    init();
}

auto Box::init() -> void {
    auto data = VsgMeshDataGenerator::box();

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild(
        vsg::BindVertexBuffers::create( 0, vsg::DataList{ data._vertices, data._colors, data._texcrds } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    _node->addChild( drawCommands );
}

}  // namespace tire::object