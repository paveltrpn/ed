
#include <vsg/all.h>

#include "sphere.h"
#include "../generatorutils.h"

namespace tire::object {

Sphere::Sphere( QObject* parent )
    : SceneObjectBase{} {
    init();
};

Sphere::Sphere( const SphereObjectData& data )
    : SceneObjectBase{ data._type, data._name, data._uid, data._position, data._orientation, data._scale, data._color }
    , _radius{ data._radius } {
    init();
}

auto Sphere::init() -> void {
    auto data = VsgMeshDataGenerator::sphere( /* radius */ 0.5,
                                              /* slices */ 8,
                                              /* segments */ 8,
                                              /* sliceStart */ 0.0,
                                              /* sliceSweep */ gml::radians( 360.0 ),
                                              /* segmentStart */ 0.0,
                                              /* segmentSweep */ gml::radians( 180.0 ) );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild(
        vsg::BindVertexBuffers::create( 0, vsg::DataList{ data._vertices, data._colors, data._texcrds } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    _node->addChild( drawCommands );
}

}  // namespace tire::object
