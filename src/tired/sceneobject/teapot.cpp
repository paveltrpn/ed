
#include <vsg/all.h>

#include "teapot.h"
#include "../generatorutils.h"

namespace tire {

// ======================================================================================
// ==================== TeapotNode ======================================================
// ======================================================================================

TeapotNode::TeapotNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                        vsg::dvec3 scale, vsg::vec4 color, int segments )
    : SceneObjectNode{ SceneObjectTypeEnum::TEAPOT, name, uid, position, orientation, scale, color }
    , _segments{ segments } {
    init();
}

auto TeapotNode::init() -> void {
    auto data = VsgMeshDataGenerator::teapot( _segments );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::BindVertexBuffers::create(
        0, vsg::DataList{ data._vertices, data._colors, data._texcrds, data._normals } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    this->addChild( drawCommands );
}

// ======================================================================================
// ==================== TeapotObject ====================================================
// ======================================================================================

TeapotObject::TeapotObject( vsg::ref_ptr<TeapotNode> node )
    : SceneObjectBase{}
    , _node{ node } {
}

}  // namespace tire
