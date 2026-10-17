
#include <vsg/all.h>

#include "cappedtube.h"
#include "../generatorutils.h"

namespace tire {

// ======================================================================================
// ==================== CappedTubeNode ==================================================
// ======================================================================================

CappedTubeNode::CappedTubeNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                                vsg::dvec3 scale, vsg::vec4 color, double radius, double innerRadius, double size,
                                int slices, int segments, int rings, double start, double sweep )
    : SceneObjectNode{ SceneObjectTypeEnum::CAPPED_TUBE, name, uid, position, orientation, scale, color }
    , _radius{ radius }
    , _innerRadius{ innerRadius }
    , _size{ size }
    , _slices{ slices }
    , _segments{ segments }
    , _rings{ rings }
    , _start{ start }
    , _sweep{ sweep } {
    init();
}

auto CappedTubeNode::init() -> void {
    auto data =
        VsgMeshDataGenerator::cappedTube( _radius, _innerRadius, _size, _slices, _segments, _rings, _start, _sweep );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::BindVertexBuffers::create(
        0, vsg::DataList{ data._vertices, data._colors, data._texcrds, data._normals } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    this->addChild( drawCommands );
}

// ======================================================================================
// ==================== CappedTubeObject ================================================
// ======================================================================================

CappedTubeObject::CappedTubeObject( vsg::ref_ptr<CappedTubeNode> node )
    : SceneObjectBase{}
    , _node{ node } {
}

}  // namespace tire
