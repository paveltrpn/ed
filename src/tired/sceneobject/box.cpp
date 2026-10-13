
#include <vsg/all.h>

#include "box.h"
#include "../generatorutils.h"

namespace tire {

// ======================================================================================
// ==================== BoxNode =========================================================
// ======================================================================================

BoxNode::BoxNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                  vsg::vec4 color, vsg::dvec3 size, vsg::ivec3 segments, SceneObjectBase* owner )
    : SceneObjectNode{ SceneObjectTypeEnum::BOX, name, uid, position, orientation, scale, color, owner }
    , _size{ size }
    , _segments{ segments } {
    init();
}

auto BoxNode::init() -> void {
    auto data = VsgMeshDataGenerator::box( { _size.x, _size.y, _size.z }, { _segments.x, _segments.y, _segments.z } );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::BindVertexBuffers::create(
        0, vsg::DataList{ data._vertices, data._colors, data._texcrds, data._normals } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    this->addChild( drawCommands );
}

// ======================================================================================
// ==================== BoxObject =======================================================
// ======================================================================================

BoxObject::BoxObject( vsg::ref_ptr<BoxNode> node )
    : SceneObjectBase{}
    , _node{ node } {
}

}  // namespace tire