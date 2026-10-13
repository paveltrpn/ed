
#include <vsg/all.h>

#include "torus.h"
#include "../generatorutils.h"

namespace tire {

// ======================================================================================
// ==================== TorusNode =======================================================
// ======================================================================================

TorusNode::TorusNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                      vsg::dvec3 scale, vsg::vec4 color, double minor, double major, int slices, int segments,
                      double minorStart, double minorSweep, double majorStart, double majorSweep )
    : SceneObjectNode{ SceneObjectTypeEnum::TORUS, name, uid, position, orientation, scale, color }
    , _minor{ minor }
    , _major{ major }
    , _slices{ slices }
    , _segments{ segments }
    , _minorStart{ minorStart }
    , _minorSweep{ minorSweep }
    , _majorStart{ majorStart }
    , _majorSweep{ majorSweep } {
    init();
}

auto TorusNode::init() -> void {
    auto data = VsgMeshDataGenerator::torus( _minor, _major, _slices, _segments, _minorStart, _minorSweep,
                                             _majorStart, _majorSweep );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::BindVertexBuffers::create(
        0, vsg::DataList{ data._vertices, data._colors, data._texcrds, data._normals } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    this->addChild( drawCommands );
}

// ======================================================================================
// ==================== TorusObject =====================================================
// ======================================================================================

TorusObject::TorusObject( vsg::ref_ptr<TorusNode> node )
    : SceneObjectBase{}
    , _node{ node } {
}

}  // namespace tire
