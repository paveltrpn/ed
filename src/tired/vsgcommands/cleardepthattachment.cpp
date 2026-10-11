
#include <vsg/vk/CommandBuffer.h>

#include "cleardepthattachment.h"
#include "../dynamicrendergraph.h"

namespace vsg {

ClearDepthAttachment::ClearDepthAttachment( observer_ptr<DynamicRenderGraph> renderGraph )
    : _renderGraph( renderGraph ) {
}

void ClearDepthAttachment::record( CommandBuffer& commandBuffer ) const {
    ref_ptr<DynamicRenderGraph> renderGraph = _renderGraph;
    if ( !renderGraph ) {
        return;
    }

    const VkExtent2D extent = renderGraph->getExtent();

    // Depth clear value 0.0f is reversed-Z "far" and must stay in sync with the
    // render-graph depth clear value set in vsgrender.cpp (setClearValues call).
    const VkClearAttachment attachment{
        VK_IMAGE_ASPECT_DEPTH_BIT, 0, VkClearValue{ .depthStencil = { 0.0f, 0 } } };

    const VkClearRect rect{ VkRect2D{ { 0, 0 }, extent }, 0, 1 };

    vkCmdClearAttachments( commandBuffer, 1, &attachment, 1, &rect );
}

}  // namespace vsg
