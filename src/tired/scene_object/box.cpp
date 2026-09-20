
#include <vsg/all.h>

#include "box.h"
#include "../generatorutils.h"

namespace tire::object {

Box::Box( const BoxObjectData& data )
    : SceneObjectBase{ data }
    , _segmentsX{ data._segmentsX }
    , _segmentsY{ data._segmentsY }
    , _segmentsZ{ data._segmentsZ } {
    init();
}

auto Box::init() -> void {
    auto data = VsgMeshDataGenerator::box( { 0.5, 0.5, 0.5 }, { _segmentsX, _segmentsY, _segmentsZ } );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild(
        vsg::BindVertexBuffers::create( 0, vsg::DataList{ data._vertices, data._colors, data._texcrds } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    _node->addChild( drawCommands );
}

}  // namespace tire::object