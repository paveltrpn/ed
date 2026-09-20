
#include <vsg/vk/CommandBuffer.h>
#include "setpolygonmode.h"

namespace vsg {

SetPolygonMode::SetPolygonMode( VkPolygonMode in_mode )
    : mode( in_mode ) {
}

void SetPolygonMode::record( CommandBuffer& commandBuffer ) const {
    // vkCmdSetPolygonModeEXT( commandBuffer, mode );
}

}  // namespace vsg
