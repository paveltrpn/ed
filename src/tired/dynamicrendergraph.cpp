
#include <array>

#include "dynamicrendergraph.h"

#include <vsg/vk/State.h>
#include <vsg/vk/Context.h>
#include <vsg/state/Image.h>

namespace vsg {

// ======================================================================================
// ==================== DynamicRenderGraph ==============================================
// ======================================================================================

DynamicRenderGraph::DynamicRenderGraph( Device* in_device, vsg::ref_ptr<ImageView> colorAttachment,
                                        vsg::ref_ptr<ImageView> depthAttachment, VkExtent2D extent )
    : viewportState( ViewportState::create() )
    , windowResizeHandler( WindowResizeHandler::create() )
    , _device( in_device )
    , _colorAttachment( colorAttachment )
    , _depthAttachment( depthAttachment )
    , _extent( extent ) {
}

void DynamicRenderGraph::accept( RecordTraversal& recordTraversal ) const {
    GPU_INSTRUMENTATION_L1_NC( recordTraversal.instrumentation, *recordTraversal.getCommandBuffer(), "RenderGraph",
                               COLOR_RECORD_L1 );

    auto extent = getExtent();

    // Dynamic render.
    VkRenderingAttachmentInfoKHR colorAttachmentInfo{ .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
                                                      .imageView = _colorAttachment->vk( _device->deviceID ),
                                                      .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                                      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                                      .clearValue = _clearColorValue };

    auto depthAttachmentInfo =
        VkRenderingAttachmentInfoKHR{ .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
                                      .imageView = _depthAttachment->vk( _device->deviceID ),
                                      .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                      .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                      .clearValue = _clearDepthValue };

    auto renderingInfo = VkRenderingInfoKHR{ .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
                                             .renderArea = { { 0, 0 }, { extent.width, extent.height } },
                                             .layerCount = 1,
                                             .colorAttachmentCount = 1,
                                             .pColorAttachments = &colorAttachmentInfo,
                                             .pDepthAttachment = &depthAttachmentInfo };

    recordTraversal.getState()->viewportStateHint = viewportStateHint;

    VkCommandBuffer vk_commandBuffer = *( recordTraversal.getState()->_commandBuffer );

    // Sync the viewportState and push.
    viewportState->set( renderArea.offset.x, renderArea.offset.y, renderArea.extent.width, renderArea.extent.height );

    vkCmdBeginRendering( vk_commandBuffer, &renderingInfo );

    if ( ( viewportStateHint & DYNAMIC_VIEWPORTSTATE ) ) {
        recordTraversal.getState()->pushView( viewportState );

        traverse( recordTraversal );

        recordTraversal.getState()->popView( viewportState );
    } else {
        traverse( recordTraversal );
    }

    vkCmdEndRendering( vk_commandBuffer );
}

void DynamicRenderGraph::setClearValues( VkClearColorValue clearColor, VkClearDepthStencilValue clearDepthStencil ) {
    _clearColorValue.color = clearColor;
    _clearDepthValue.depthStencil = clearDepthStencil;
}

void DynamicRenderGraph::resized() {
}

void DynamicRenderGraph::setAttachments( vsg::ref_ptr<ImageView> colorAttachment,
                                         vsg::ref_ptr<ImageView> depthAttachment, VkExtent2D extent ) {
    _colorAttachment = colorAttachment;
    _depthAttachment = depthAttachment;
    _extent = extent;
    renderArea.extent = _extent;
}

}  // namespace vsg
