
#include "dynamicrendergraph.h"

#include <vsg/vk/State.h>
#include <vsg/vk/Context.h>
#include <vsg/state/Image.h>
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

    VkClearValue clearColor = { .color = { 0.2f, 0.2f, 0.5f, 1.0f } };

    // Dynamic render.
    VkRenderingAttachmentInfoKHR colorAttachmentInfo{ .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
                                                      .imageView = _colorAttachment->vk( _device->deviceID ),
                                                      .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                                      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                                      .clearValue = clearColor };

    VkClearValue clearDepth = { .depthStencil = { 0.0f, 0 } };

    auto depthAttachmentInfo =
        VkRenderingAttachmentInfoKHR{ .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
                                      .imageView = _depthAttachment->vk( _device->deviceID ),
                                      .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                      .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                      .clearValue = clearDepth };

    auto renderingInfo = VkRenderingInfoKHR{ .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
                                             .renderArea = { { 0, 0 }, { extent.width, extent.height } },
                                             .layerCount = 1,
                                             .colorAttachmentCount = 1,
                                             .pColorAttachments = &colorAttachmentInfo,
                                             .pDepthAttachment = &depthAttachmentInfo };

    recordTraversal.getState()->viewportStateHint = viewportStateHint;

    VkCommandBuffer vk_commandBuffer = *( recordTraversal.getState()->_commandBuffer );

    // Transition attachments into the layouts declared for dynamic rendering.
    // oldLayout = UNDEFINED is always legal and discards contents, which is fine
    // because both attachments use VK_ATTACHMENT_LOAD_OP_CLEAR.
    VkImageMemoryBarrier barriers[2]{};
    barriers[0] = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                    nullptr,
                    VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                    VK_IMAGE_LAYOUT_UNDEFINED,
                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    VK_QUEUE_FAMILY_IGNORED,
                    VK_QUEUE_FAMILY_IGNORED,
                    _colorAttachment->image->vk( _device->deviceID ),
                    { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 } };
    barriers[1] = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                    nullptr,
                    VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                    VK_IMAGE_LAYOUT_UNDEFINED,
                    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                    VK_QUEUE_FAMILY_IGNORED,
                    VK_QUEUE_FAMILY_IGNORED,
                    _depthAttachment->image->vk( _device->deviceID ),
                    { vsg::computeAspectFlagsForFormat( _depthAttachment->image->format ), 0, 1, 0, 1 } };

    vkCmdPipelineBarrier( vk_commandBuffer,
                          VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                              VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                          VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                          0, 0, nullptr, 0, nullptr, 2, barriers );

    // sync the viewportState and push
    viewportState->set( renderArea.offset.x, renderArea.offset.y, renderArea.extent.width, renderArea.extent.height );

    vkCmdBeginRendering( vk_commandBuffer, &renderingInfo );

    if ( ( viewportStateHint & DYNAMIC_VIEWPORTSTATE ) ) {
        recordTraversal.getState()->pushView( viewportState );

        // traverse the subgraph to place commands into the command buffer.
        traverse( recordTraversal );

        recordTraversal.getState()->popView( viewportState );
    } else {
        // traverse the subgraph to place commands into the command buffer.
        traverse( recordTraversal );
    }

    vkCmdEndRendering( vk_commandBuffer );
}

void DynamicRenderGraph::setClearValues( VkClearColorValue clearColor, VkClearDepthStencilValue clearDepthStencil ) {
}

void DynamicRenderGraph::resized() {
}

void DynamicRenderGraph::setAttachments( vsg::ref_ptr<ImageView> colorAttachment,
                                         vsg::ref_ptr<ImageView> depthAttachment, uint32_t width, uint32_t height ) {
    _colorAttachment = colorAttachment;
    _depthAttachment = depthAttachment;
    _width = width;
    _height = height;
    renderArea.extent = { width, height };
}

}  // namespace vsg
