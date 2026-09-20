
#include <vsg/all.h>

#include "capsule.h"
#include "../generatorutils.h"

namespace tire::object {

Capsule::Capsule( QObject* parent )
    : SceneObjectBase{} {
    init();
}

Capsule::Capsule( const CapsuleObjectData& data )
    : SceneObjectBase{ data._type, data._name, data._uid, data._position, data._orientation, data._scale, data._color }
    , _radius{ data._radius }
    , _height{ data._height } {
    init();
}

auto Capsule::init() -> void {
    auto data = VsgMeshDataGenerator::capsule( /* radius */ 0.5,
                                               /* size */ 0.5,
                                               /* slices */ 16,
                                               /* segments */ 4,
                                               /* rings */ 8,
                                               /* start */ 0.0,
                                               /* sweep */ gml::radians( 360.0 ) );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild(
        vsg::BindVertexBuffers::create( 0, vsg::DataList{ data._vertices, data._colors, data._texcrds } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    _node->addChild( drawCommands );
}
}  // namespace tire::object