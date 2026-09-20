
#include <vsg/vk/CommandBuffer.h>
#include "setcullmode.h"

namespace vsg {

SetCullMode::SetCullMode( VkCullModeFlagBits in_mode )
    : mode( in_mode ) {
}

void SetCullMode::record( CommandBuffer& commandBuffer ) const {
    vkCmdSetCullModeEXT( commandBuffer, mode );
}

}  // namespace vsg

