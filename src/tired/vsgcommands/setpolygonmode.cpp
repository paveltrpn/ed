
#include <vsg/vk/CommandBuffer.h>

#include "setpolygonmode.h"
#include "../extfunctions.h"

namespace vsg {

SetPolygonMode::SetPolygonMode( VkPolygonMode in_mode )
    : mode( in_mode ) {
}

void SetPolygonMode::record( CommandBuffer& commandBuffer ) const {
    tire::vkCmdSetPolygonMode( commandBuffer, mode );
}

}  // namespace vsg
