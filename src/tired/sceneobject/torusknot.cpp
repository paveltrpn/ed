
#include <vsg/all.h>

#include "torusknot.h"
#include "../generatorutils.h"

namespace tire {

// ======================================================================================
// ==================== TorusKnotNode ===================================================
// ======================================================================================

TorusKnotNode::TorusKnotNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                              vsg::dvec3 scale, vsg::vec4 color, int p, int q, int slices, int segments )
    : SceneObjectNode{ SceneObjectTypeEnum::TORUS_KNOT, name, uid, position, orientation, scale, color }
    , _p{ p }
    , _q{ q }
    , _slices{ slices }
    , _segments{ segments } {
    init();
}

auto TorusKnotNode::init() -> void {
    auto data = VsgMeshDataGenerator::torusKnot( _p, _q, _slices, _segments );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::BindVertexBuffers::create(
        0, vsg::DataList{ data._vertices, data._colors, data._texcrds, data._normals } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    this->addChild( drawCommands );
}

// ======================================================================================
// ==================== TorusKnotObject =================================================
// ======================================================================================

TorusKnotObject::TorusKnotObject( vsg::ref_ptr<TorusKnotNode> node )
    : SceneObjectBase{}
    , _node{ node } {
}

}  // namespace tire
