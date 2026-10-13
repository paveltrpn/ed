
#include <vsg/all.h>

#include "capsule.h"
#include "../generatorutils.h"

namespace tire {

// ======================================================================================
// ==================== CapsuleNode =====================================================
// ======================================================================================

CapsuleNode::CapsuleNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                          vsg::dvec3 scale, vsg::vec4 color, double radius, double size, int slices,
                          int segments, int rings, double start, double sweep )
    : SceneObjectNode{ SceneObjectTypeEnum::CAPSULE, name, uid, position, orientation, scale, color }
    , _radius{ radius }
    , _size{ size }
    , _slices{ slices }
    , _segments{ segments }
    , _rings{ rings }
    , _start{ start }
    , _sweep{ sweep } {
    init();
}

auto CapsuleNode::init() -> void {
    auto data = VsgMeshDataGenerator::capsule( _radius, _size, _slices, _segments, _rings, _start, _sweep );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::BindVertexBuffers::create(
        0, vsg::DataList{ data._vertices, data._colors, data._texcrds, data._normals } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    this->addChild( drawCommands );
}

// ======================================================================================
// ==================== CapsuleObject ===================================================
// ======================================================================================

CapsuleObject::CapsuleObject( vsg::ref_ptr<CapsuleNode> node )
    : SceneObjectBase{}
    , _node{ node } {
}

}  // namespace tire
