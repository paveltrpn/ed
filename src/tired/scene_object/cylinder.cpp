
#include <vsg/all.h>

#include "cylinder.h"
#include "../generatorutils.h"

namespace tire::object {

Cylinder::Cylinder( QObject* parent ) {
    init();
}

Cylinder::Cylinder( const CylinderObjectData& data )
    : SceneObjectBase{ data._type, data._name, data._uid, data._position, data._orientation, data._scale, data._color }
    , _radius{ data._radius }
    , _height{ data._height } {
    init();
}

auto Cylinder::init() -> void {
    auto data = VsgMeshDataGenerator::cylinder( /* radius */ 0.5,
                                                /* size */ 0.5,
                                                /* slices */ 32,
                                                /* segments */ 8,
                                                /* rings */ 4,
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