
#include <vsg/all.h>

#include "roundedbox.h"
#include "../generatorutils.h"

namespace tire {

// ======================================================================================
// ==================== RoundedBoxNode ==================================================
// ======================================================================================

RoundedBoxNode::RoundedBoxNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                                vsg::dvec3 scale, vsg::vec4 color, double radius, vsg::dvec3 size, int slices,
                                vsg::ivec3 segments )
    : SceneObjectNode{ SceneObjectTypeEnum::ROUNDED_BOX, name, uid, position, orientation, scale, color }
    , _radius{ radius }
    , _size{ size }
    , _slices{ slices }
    , _segments{ segments } {
    init();
}

auto RoundedBoxNode::init() -> void {
    auto data = VsgMeshDataGenerator::roundedBox( _radius, { _size.x, _size.y, _size.z }, _slices,
                                                  { _segments.x, _segments.y, _segments.z } );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::BindVertexBuffers::create(
        0, vsg::DataList{ data._vertices, data._colors, data._texcrds, data._normals } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    this->addChild( drawCommands );
}

// ======================================================================================
// ==================== RoundedBoxObject ================================================
// ======================================================================================

RoundedBoxObject::RoundedBoxObject( vsg::ref_ptr<RoundedBoxNode> node )
    : SceneObjectBase{}
    , _node{ node } {
}

}  // namespace tire
