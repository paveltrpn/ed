
#include <vsg/all.h>

#include "sphere.h"
#include "../generatorutils.h"

namespace tire::object {

Sphere::Sphere( const SphereObjectData& data )
    : SceneObjectBase{ data }
    , _data{ data } {
    init();
}

auto Sphere::init() -> void {
    auto data = VsgMeshDataGenerator::sphere( _data._radius, _data._slices, _data._segments, _data._sliceStart,
                                              _data._sliceSweep, _data._segmentStart, _data._segmentSweep );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild(
        vsg::BindVertexBuffers::create( 0, vsg::DataList{ data._vertices, data._colors, data._texcrds } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    _node->addChild( drawCommands );
}

}  // namespace tire::object
