
#include <vsg/all.h>

#include "tube.h"
#include "../generatorutils.h"

namespace tire {

// ======================================================================================
// ==================== TubeNode ========================================================
// ======================================================================================

TubeNode::TubeNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                    vsg::vec4 color, double radius, double innerRadius, double size, int slices, int segments,
                    double start, double sweep )
    : SceneObjectNode{ SceneObjectTypeEnum::TUBE, name, uid, position, orientation, scale, color }
    , _radius{ radius }
    , _innerRadius{ innerRadius }
    , _size{ size }
    , _slices{ slices }
    , _segments{ segments }
    , _start{ start }
    , _sweep{ sweep } {
    init();
}

auto TubeNode::init() -> void {
    auto data = VsgMeshDataGenerator::tube( _radius, _innerRadius, _size, _slices, _segments, _start, _sweep );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::BindVertexBuffers::create(
        0, vsg::DataList{ data._vertices, data._colors, data._texcrds, data._normals } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    this->addChild( drawCommands );
}

// ======================================================================================
// ==================== TubeObject ======================================================
// ======================================================================================

TubeObject::TubeObject( vsg::ref_ptr<TubeNode> node )
    : SceneObjectBase{}
    , _node{ node } {
}

}  // namespace tire
