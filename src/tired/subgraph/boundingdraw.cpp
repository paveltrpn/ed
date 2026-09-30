
#include "boundingdraw.h"

// ======================================================================================
// ==================== BoundingDraw ====================================================
// ======================================================================================

namespace tire {

BoundingDraw::BoundingDraw() {
    _transformArray = vsg::mat4Array::create( 1 );
    _transformArray->set( 0, _transform );
    _transformArray->properties.dataVariance = vsg::DYNAMIC_DATA;

    const auto dc = vsg::Draw::create( 48, 1, 0, 0 );
    this->addChild( dc );

    const auto pc = vsg::PushConstants::create( VK_SHADER_STAGE_VERTEX_BIT, 128, _transformArray );
    this->addChild( pc );
}

auto BoundingDraw::setTransform( const vsg::mat4& matrix ) -> void {
    _transform = matrix;

    _transformArray->set( 0, _transform );
    _transformArray->dirty();
}

}  // namespace tire