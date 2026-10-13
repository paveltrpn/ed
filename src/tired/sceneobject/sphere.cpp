
#include <vsg/all.h>

#include "sphere.h"
#include "../generatorutils.h"

namespace tire {

// ======================================================================================
// ==================== SphereNode ======================================================
// ======================================================================================

SphereNode::SphereNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                        vsg::dvec3 scale, vsg::vec4 color, double radius, int slices, int segments,
                        double sliceStart, double sliceSweep, double segmentStart, double segmentSweep,
                        SceneObjectBase* owner )
    : SceneObjectNode{ SceneObjectTypeEnum::SPHERE, name, uid, position, orientation, scale, color, owner }
    , _radius{ radius }
    , _slices{ slices }
    , _segments{ segments }
    , _sliceStart{ sliceStart }
    , _sliceSweep{ sliceSweep }
    , _segmentStart{ segmentStart }
    , _segmentSweep{ segmentSweep } {
    init();
}

auto SphereNode::init() -> void {
    auto data = VsgMeshDataGenerator::sphere( _radius, _slices, _segments, _sliceStart, _sliceSweep,
                                              _segmentStart, _segmentSweep );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::BindVertexBuffers::create(
        0, vsg::DataList{ data._vertices, data._colors, data._texcrds, data._normals } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    this->addChild( drawCommands );
}

// ======================================================================================
// ==================== SphereObject ====================================================
// ======================================================================================

SphereObject::SphereObject( vsg::ref_ptr<SphereNode> node )
    : SceneObjectBase{}
    , _node{ node } {
}

}  // namespace tire
