
#include <vsg/all.h>

#include "capsule.h"
#include "../generatorutils.h"

namespace tire::object {

Capsule::Capsule( const CapsuleObjectData& data )
    : SceneObjectBase{ data }
    , _data{ data } {
    init();
}

auto Capsule::init() -> void {
    auto data = VsgMeshDataGenerator::capsule( _data._radius, _data._size, _data._slices, _data._segments, _data._rings,
                                               _data._start, _data._sweep );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::BindVertexBuffers::create(
        0, vsg::DataList{ data._vertices, data._colors, data._texcrds, data._normals } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    _node->addChild( drawCommands );
}
}  // namespace tire::object