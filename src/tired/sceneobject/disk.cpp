
#include <vsg/all.h>

#include "disk.h"
#include "../generatorutils.h"

namespace tire {

// ======================================================================================
// ==================== DiskNode ========================================================
// ======================================================================================

DiskNode::DiskNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                    vsg::vec4 color, double radius, double innerRadius, int slices, int rings, double start,
                    double sweep )
    : SceneObjectNode{ SceneObjectTypeEnum::DISK, name, uid, position, orientation, scale, color }
    , _radius{ radius }
    , _innerRadius{ innerRadius }
    , _slices{ slices }
    , _rings{ rings }
    , _start{ start }
    , _sweep{ sweep } {
    init();
}

auto DiskNode::init() -> void {
    auto data = VsgMeshDataGenerator::disk( _radius, _innerRadius, _slices, _rings, _start, _sweep );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::BindVertexBuffers::create(
        0, vsg::DataList{ data._vertices, data._colors, data._texcrds, data._normals } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    this->addChild( drawCommands );
}

// ======================================================================================
// ==================== DiskObject ======================================================
// ======================================================================================

DiskObject::DiskObject( vsg::ref_ptr<DiskNode> node )
    : SceneObjectBase{}
    , _node{ node } {
}

}  // namespace tire
