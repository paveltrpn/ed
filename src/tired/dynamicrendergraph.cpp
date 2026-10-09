
#include "dynamicrendergraph.h"

#include <vsg/vk/State.h>
#include <vsg/vk/Context.h>
namespace vsg {

DynamicRenderGraph::DynamicRenderGraph( Device* in_device, vsg::ref_ptr<ImageView> colorAttachment,
                                        vsg::ref_ptr<ImageView> depthAttachment, uint32_t width, uint32_t height )
    : viewportState( ViewportState::create() )
    , windowResizeHandler( WindowResizeHandler::create() )
    , _device( in_device )
    , _colorAttachment( colorAttachment )
    , _depthAttachment( depthAttachment )
    , _width( width )
    , _height( height ) {
}

void DynamicRenderGraph::accept( RecordTraversal& recordTraversal ) const {
    GPU_INSTRUMENTATION_L1_NC( recordTraversal.instrumentation, *recordTraversal.getCommandBuffer(), "RenderGraph",
                               COLOR_RECORD_L1 );

    auto extent = getExtent();

    VkClearValue clearColor = { .color = { 0.0f, 0.0f, 0.0f, 1.0f } };

    // Dynamic render.
    VkRenderingAttachmentInfoKHR colorAttachmentInfo{ .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
                                                      .imageView = _colorAttachment->vk( _device->deviceID ),
                                                      .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                                      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                                      .clearValue = clearColor };

    VkClearValue clearDepth = { .depthStencil = { 1.0f, 0 } };

    auto depthAttachmentInfo =
        VkRenderingAttachmentInfoKHR{ .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
                                      .imageView = _depthAttachment->vk( _device->deviceID ),
                                      .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                      .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                      .clearValue = clearDepth };

    auto renderingInfo = VkRenderingInfoKHR{
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
        .renderArea = { { 0, 0 }, { extent.width, extent.height } },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentInfo,
        .pDepthAttachment = &depthAttachmentInfo  // nullptr, если глубина не используется
    };

    recordTraversal.getState()->viewportStateHint = viewportStateHint;

    VkCommandBuffer vk_commandBuffer = *( recordTraversal.getState()->_commandBuffer );
    vkCmdBeginRendering( vk_commandBuffer, &renderingInfo );

    // sync the viewportState and push
    viewportState->set( renderArea.offset.x, renderArea.offset.y, renderArea.extent.width, renderArea.extent.height );

    if ( ( viewportStateHint & DYNAMIC_VIEWPORTSTATE ) ) {
        recordTraversal.getState()->pushView( viewportState );

        // traverse the subgraph to place commands into the command buffer.
        traverse( recordTraversal );

        recordTraversal.getState()->popView( viewportState );
    } else {
        // traverse the subgraph to place commands into the command buffer.
        traverse( recordTraversal );
    }
}

void DynamicRenderGraph::setClearValues( VkClearColorValue clearColor, VkClearDepthStencilValue clearDepthStencil ) {
}

void DynamicRenderGraph::resized() {
}

}  // namespace vsg
