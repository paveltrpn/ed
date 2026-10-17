
#include <vsg/all.h>

#include "icosahedron.h"
#include "../generatorutils.h"

namespace tire {

// ======================================================================================
// ==================== IcosahedronNode =================================================
// ======================================================================================

IcosahedronNode::IcosahedronNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                                  vsg::dvec3 scale, vsg::vec4 color, double radius, int segments )
    : SceneObjectNode{ SceneObjectTypeEnum::ICOSAHEDRON, name, uid, position, orientation, scale, color }
    , _radius{ radius }
    , _segments{ segments } {
    init();
}

auto IcosahedronNode::init() -> void {
    auto data = VsgMeshDataGenerator::icosahedron( _radius, _segments );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::BindVertexBuffers::create(
        0, vsg::DataList{ data._vertices, data._colors, data._texcrds, data._normals } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    this->addChild( drawCommands );
}

// ======================================================================================
// ==================== IcosahedronObject ===============================================
// ======================================================================================

IcosahedronObject::IcosahedronObject( vsg::ref_ptr<IcosahedronNode> node )
    : SceneObjectBase{}
    , _node{ node } {
}

}  // namespace tire
