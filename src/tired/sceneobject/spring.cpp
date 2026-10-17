
#include <vsg/all.h>

#include "spring.h"
#include "../generatorutils.h"

namespace tire {

// ======================================================================================
// ==================== SpringNode ======================================================
// ======================================================================================

SpringNode::SpringNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                        vsg::dvec3 scale, vsg::vec4 color, double minor, double major, double size, int slices,
                        int segments, double minorStart, double minorSweep, double majorStart, double majorSweep )
    : SceneObjectNode{ SceneObjectTypeEnum::SPRING, name, uid, position, orientation, scale, color }
    , _minor{ minor }
    , _major{ major }
    , _size{ size }
    , _slices{ slices }
    , _segments{ segments }
    , _minorStart{ minorStart }
    , _minorSweep{ minorSweep }
    , _majorStart{ majorStart }
    , _majorSweep{ majorSweep } {
    init();
}

auto SpringNode::init() -> void {
    auto data = VsgMeshDataGenerator::spring( _minor, _major, _size, _slices, _segments, _minorStart, _minorSweep,
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
// ==================== SpringObject ====================================================
// ======================================================================================

SpringObject::SpringObject( vsg::ref_ptr<SpringNode> node )
    : SceneObjectBase{}
    , _node{ node } {
}

}  // namespace tire
