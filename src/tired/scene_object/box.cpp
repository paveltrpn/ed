
#include <vsg/all.h>

#include "box.h"
#include "../generatorutils.h"

namespace tire::object {

Box::Box( const BoxObjectData& data )
    : SceneObjectBase{ data }
    , _data{ data } {
    init();
}

auto Box::init() -> void {
    auto data =
        VsgMeshDataGenerator::box( { 0.5, 0.5, 0.5 }, { _data._segmentsX, _data._segmentsY, _data._segmentsZ } );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild(
        vsg::BindVertexBuffers::create( 0, vsg::DataList{ data._vertices, data._colors, data._texcrds } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    _node->addChild( drawCommands );
}

}  // namespace tire::object