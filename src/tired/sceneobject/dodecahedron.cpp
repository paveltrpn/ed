
#include <vsg/all.h>

#include "dodecahedron.h"
#include "../generatorutils.h"

namespace tire {

// ======================================================================================
// ==================== DodecahedronNode ================================================
// ======================================================================================

DodecahedronNode::DodecahedronNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                                    vsg::dvec3 scale, vsg::vec4 color, double radius, int segments, int rings )
    : SceneObjectNode{ SceneObjectTypeEnum::DODECAHEDRON, name, uid, position, orientation, scale, color }
    , _radius{ radius }
    , _segments{ segments }
    , _rings{ rings } {
    init();
}

auto DodecahedronNode::init() -> void {
    auto data = VsgMeshDataGenerator::dodecahedron( _radius, _segments, _rings );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::BindVertexBuffers::create(
        0, vsg::DataList{ data._vertices, data._colors, data._texcrds, data._normals } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    this->addChild( drawCommands );
}

// ======================================================================================
// ==================== DodecahedronObject ==============================================
// ======================================================================================

DodecahedronObject::DodecahedronObject( vsg::ref_ptr<DodecahedronNode> node )
    : SceneObjectBase{}
    , _node{ node } {
}

}  // namespace tire
