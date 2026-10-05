
#include <vsg/vk/CommandBuffer.h>

#include "setcullmode.h"
#include "../extfunctions.h"

namespace vsg {

SetCullMode::SetCullMode( VkCullModeFlagBits in_mode )
    : mode( in_mode ) {
}

void SetCullMode::record( CommandBuffer& commandBuffer ) const {
    tire::vkCmdSetCullMode( commandBuffer, mode );
}

}  // namespace vsg
