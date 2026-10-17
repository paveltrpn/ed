
#include <vsg/all.h>

#include "icosphere.h"
#include "../generatorutils.h"

namespace tire {

// ======================================================================================
// ==================== IcosphereNode ===================================================
// ======================================================================================

IcosphereNode::IcosphereNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                              vsg::dvec3 scale, vsg::vec4 color, double radius, int segments )
    : SceneObjectNode{ SceneObjectTypeEnum::ICOSPHERE, name, uid, position, orientation, scale, color }
    , _radius{ radius }
    , _segments{ segments } {
    init();
}

auto IcosphereNode::init() -> void {
    auto data = VsgMeshDataGenerator::icosphere( _radius, _segments );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::BindVertexBuffers::create(
        0, vsg::DataList{ data._vertices, data._colors, data._texcrds, data._normals } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    this->addChild( drawCommands );
}

// ======================================================================================
// ==================== IcosphereObject =================================================
// ======================================================================================

IcosphereObject::IcosphereObject( vsg::ref_ptr<IcosphereNode> node )
    : SceneObjectBase{}
    , _node{ node } {
}

}  // namespace tire
