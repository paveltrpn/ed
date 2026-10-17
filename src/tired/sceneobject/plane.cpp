
#include <vsg/all.h>

#include "plane.h"
#include "../generatorutils.h"

namespace tire {

// ======================================================================================
// ==================== PlaneNode =======================================================
// ======================================================================================

PlaneNode::PlaneNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                      vsg::dvec3 scale, vsg::vec4 color, vsg::dvec2 size, vsg::ivec2 segments )
    : SceneObjectNode{ SceneObjectTypeEnum::PLANE, name, uid, position, orientation, scale, color }
    , _size{ size }
    , _segments{ segments } {
    init();
}

auto PlaneNode::init() -> void {
    auto data = VsgMeshDataGenerator::plane( { _size.x, _size.y }, { _segments.x, _segments.y } );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::BindVertexBuffers::create(
        0, vsg::DataList{ data._vertices, data._colors, data._texcrds, data._normals } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    this->addChild( drawCommands );
}

// ======================================================================================
// ==================== PlaneObject =====================================================
// ======================================================================================

PlaneObject::PlaneObject( vsg::ref_ptr<PlaneNode> node )
    : SceneObjectBase{}
    , _node{ node } {
}

}  // namespace tire
