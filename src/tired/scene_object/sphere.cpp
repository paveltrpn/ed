
#include <vsg/all.h>

#include "sphere.h"
#include "../generatorutils.h"

namespace tire::object {

Sphere::Sphere( const SphereObjectData& data )
    : SceneObjectBase{ data._type, data._name, data._uid, data._position, data._orientation, data._scale, data._color }
    , _radius{ data._radius }
    , _slices{ data._slices }
    , _segments{ data._segments }
    , _sliceStart{ data._sliceStart }
    , _sliceSweep{ data._sliceSweep }
    , _segmentStart{ data._segmentStart }
    , _segmentSweep{ data._segmentSweep } {
    init();
}

auto Sphere::init() -> void {
    auto data = VsgMeshDataGenerator::sphere( _radius, _slices, _segments, _sliceStart, _sliceSweep, _segmentStart,
                                              _segmentSweep );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild(
        vsg::BindVertexBuffers::create( 0, vsg::DataList{ data._vertices, data._colors, data._texcrds } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    _node->addChild( drawCommands );
}

}  // namespace tire::object
