
#include <print>
#include <iostream>

#include <vsg/all.h>

#include "vsgrender.h"
#include "extfunctions.h"
#include "image/color.h"

namespace tire {

vsg::ref_ptr<vsg::RenderPass> createOffscreenRenderPass( vsg::Device* device, VkFormat imageFormat,
                                                         VkFormat depthFormat, bool requiresDepthRead ) {
    auto colorAttachment = vsg::defaultColorAttachment( imageFormat );
    auto depthAttachment = vsg::defaultDepthAttachment( depthFormat );

    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;  // difference from createRenderPass

    if ( requiresDepthRead ) {
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    }

    vsg::RenderPass::Attachments attachments{ colorAttachment, depthAttachment };

    vsg::AttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    vsg::AttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    vsg::SubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachments.emplace_back( colorAttachmentRef );
    subpass.depthStencilAttachments.emplace_back( depthAttachmentRef );

    vsg::RenderPass::Subpasses subpasses{ subpass };

    // image layout transition
    vsg::SubpassDependency colorDependency = {};
    colorDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    colorDependency.dstSubpass = 0;
    colorDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    colorDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    colorDependency.srcAccessMask = 0;
    colorDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    colorDependency.dependencyFlags = 0;

    // depth buffer is shared between swap chain images
    vsg::SubpassDependency depthDependency = {};
    depthDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    depthDependency.dstSubpass = 0;
    depthDependency.srcStageMask =
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depthDependency.dstStageMask =
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depthDependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    depthDependency.dstAccessMask =
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    depthDependency.dependencyFlags = 0;

    vsg::RenderPass::Dependencies dependencies{ colorDependency, depthDependency };

    return vsg::RenderPass::create( device, attachments, subpasses, dependencies );
}

vsg::ref_ptr<vsg::ImageView> createColorImageView( vsg::ref_ptr<vsg::Device> device, const VkExtent2D& extent,
                                                   VkFormat imageFormat, VkSampleCountFlagBits samples ) {
    auto colorImage = vsg::MutableFormatImage::create();
    colorImage->imageType = VK_IMAGE_TYPE_2D;
    colorImage->format = imageFormat;
    colorImage->extent = VkExtent3D{ extent.width, extent.height, 1 };
    colorImage->mipLevels = 1;
    colorImage->arrayLayers = 1;
    colorImage->samples = samples;
    colorImage->tiling = VK_IMAGE_TILING_OPTIMAL;
    colorImage->usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    colorImage->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    // NOTE: VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT allready set!
    colorImage->flags = 0;

    colorImage->sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    return vsg::createImageView( device, colorImage, VK_IMAGE_ASPECT_COLOR_BIT );
}

vsg::ref_ptr<vsg::ImageView> createDepthImageView( vsg::ref_ptr<vsg::Device> device, const VkExtent2D& extent,
                                                   VkFormat depthFormat, VkSampleCountFlagBits samples ) {
    auto depthImage = vsg::Image::create();
    depthImage->imageType = VK_IMAGE_TYPE_2D;
    depthImage->extent = VkExtent3D{ extent.width, extent.height, 1 };
    depthImage->mipLevels = 1;
    depthImage->arrayLayers = 1;
    depthImage->samples = samples;
    depthImage->format = depthFormat;
    depthImage->tiling = VK_IMAGE_TILING_OPTIMAL;
    depthImage->usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    depthImage->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthImage->flags = 0;
    depthImage->sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    return vsg::createImageView( device, depthImage, vsg::computeAspectFlagsForFormat( depthFormat ) );
}

std::pair<vsg::ref_ptr<vsg::Commands>, vsg::ref_ptr<vsg::Image>> createColorCapture(
    vsg::ref_ptr<vsg::Device> device, const VkExtent2D& extent, vsg::ref_ptr<vsg::Image> sourceImage,
    VkFormat sourceImageFormat ) {
    auto width = extent.width;
    auto height = extent.height;

    auto physicalDevice = device->getPhysicalDevice();

    VkFormat targetImageFormat = sourceImageFormat;

    //
    // 1) Check to see if Blit is supported.
    //
    VkFormatProperties srcFormatProperties;
    vkGetPhysicalDeviceFormatProperties( *( physicalDevice ), sourceImageFormat, &srcFormatProperties );

    VkFormatProperties destFormatProperties;
    vkGetPhysicalDeviceFormatProperties( *( physicalDevice ), VK_FORMAT_R8G8B8A8_SRGB, &destFormatProperties );

    bool supportsBlit = ( ( srcFormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT ) != 0 ) &&
                        ( ( destFormatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT ) != 0 );

    if ( supportsBlit ) {
        // we can automatically convert the image format when blit, so take advantage of it to ensure RGBA
        targetImageFormat = VK_FORMAT_R8G8B8A8_SRGB;
    }

    //std::cout<<"supportsBlit = "<<supportsBlit<<std::endl;

    //
    // 2) create image to write to
    //
    auto destinationImage = vsg::MutableFormatImage::create();
    destinationImage->imageType = VK_IMAGE_TYPE_2D;
    destinationImage->format = targetImageFormat;
    destinationImage->extent.width = width;
    destinationImage->extent.height = height;
    destinationImage->extent.depth = 1;
    destinationImage->arrayLayers = 1;
    destinationImage->mipLevels = 1;
    destinationImage->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    destinationImage->samples = VK_SAMPLE_COUNT_1_BIT;
    destinationImage->tiling = VK_IMAGE_TILING_LINEAR;
    destinationImage->usage =
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    destinationImage->compile( device );

    auto deviceMemory =
        vsg::DeviceMemory::create( device, destinationImage->getMemoryRequirements( device->deviceID ),
                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );

    destinationImage->bind( deviceMemory, 0 );

    //
    // 3) create command buffer and submit to graphics queue
    //
    auto commands = vsg::Commands::create();

    // 3.a) transition destinationImage to transfer destination initialLayout
    auto transitionDestinationImageToDestinationLayoutBarrier = vsg::ImageMemoryBarrier::create(
        0,                                                                // srcAccessMask
        VK_ACCESS_TRANSFER_WRITE_BIT,                                     // dstAccessMask
        VK_IMAGE_LAYOUT_UNDEFINED,                                        // oldLayout
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,                             // newLayout
        VK_QUEUE_FAMILY_IGNORED,                                          // srcQueueFamilyIndex
        VK_QUEUE_FAMILY_IGNORED,                                          // dstQueueFamilyIndex
        destinationImage,                                                 // image
        VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }  // subresourceRange
    );

#if 1
    // 3.b) transition swapChainImage from present to transfer source initialLayout
    // NOTE: Good for NV!
    auto transitionSourceImageToTransferSourceLayoutBarrier =
        vsg::ImageMemoryBarrier::create( VK_ACCESS_MEMORY_READ_BIT,             // srcAccessMask
                                         VK_ACCESS_TRANSFER_READ_BIT,           // dstAccessMask
                                         VK_IMAGE_LAYOUT_UNDEFINED,             // oldLayout  <-- FIX
                                         VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,  // newLayout
                                         VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, sourceImage,
                                         VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 } );
#else
    // NOTE: Make radeon works!
    auto transitionSourceImageToTransferSourceLayoutBarrier = vsg::ImageMemoryBarrier::create(
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,  // srcAccessMask
        VK_ACCESS_TRANSFER_READ_BIT,           // dstAccessMask
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,  // oldLayout <- matches render pass
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,  // newLayout <- no-op, but keeps the barrier for sync
        VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, sourceImage,
        VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 } );
#endif

    auto cmd_transitionForTransferBarrier =
        vsg::PipelineBarrier::create( VK_PIPELINE_STAGE_TRANSFER_BIT,                        // srcStageMask
                                      VK_PIPELINE_STAGE_TRANSFER_BIT,                        // dstStageMask
                                      0,                                                     // dependencyFlags
                                      transitionDestinationImageToDestinationLayoutBarrier,  // barrier
                                      transitionSourceImageToTransferSourceLayoutBarrier     // barrier
        );

    commands->addChild( cmd_transitionForTransferBarrier );

    if ( supportsBlit ) {
        // 3.c.1) if blit using vkCmdBlitImage
        VkImageBlit region{};
        region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.srcSubresource.layerCount = 1;
        region.srcOffsets[0] = VkOffset3D{ 0, 0, 0 };
        region.srcOffsets[1] = VkOffset3D{ static_cast<int32_t>( width ), static_cast<int32_t>( height ), 1 };
        region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.dstSubresource.layerCount = 1;
        region.dstOffsets[0] = VkOffset3D{ 0, 0, 0 };
        region.dstOffsets[1] = VkOffset3D{ static_cast<int32_t>( width ), static_cast<int32_t>( height ), 1 };

        auto blitImage = vsg::BlitImage::create();
        blitImage->srcImage = sourceImage;
        blitImage->srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        blitImage->dstImage = destinationImage;
        blitImage->dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        blitImage->regions.push_back( region );
        blitImage->filter = VK_FILTER_NEAREST;

        commands->addChild( blitImage );
    } else {
        // 3.c.2) else use vkCmdCopyImage

        VkImageCopy region{};
        region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.srcSubresource.layerCount = 1;
        region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.dstSubresource.layerCount = 1;
        region.extent.width = width;
        region.extent.height = height;
        region.extent.depth = 1;

        auto copyImage = vsg::CopyImage::create();
        copyImage->srcImage = sourceImage;
        copyImage->srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        copyImage->dstImage = destinationImage;
        copyImage->dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        copyImage->regions.push_back( region );

        commands->addChild( copyImage );
    }

    // 3.d) transition destination image from transfer destination layout to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    auto transitionDestinationImageToMemoryReadBarrier = vsg::ImageMemoryBarrier::create(
        VK_ACCESS_TRANSFER_WRITE_BIT,                                     // srcAccessMask
        VK_ACCESS_MEMORY_READ_BIT,                                        // dstAccessMask
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,                             // oldLayout
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,                         // newLayout
        VK_QUEUE_FAMILY_IGNORED,                                          // srcQueueFamilyIndex
        VK_QUEUE_FAMILY_IGNORED,                                          // dstQueueFamilyIndex
        destinationImage,                                                 // image
        VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }  // subresourceRange
    );

    auto cmd_transitionFromTransferBarrier =
        vsg::PipelineBarrier::create( VK_PIPELINE_STAGE_TRANSFER_BIT,                // srcStageMask
                                      VK_PIPELINE_STAGE_TRANSFER_BIT,                // dstStageMask
                                      0,                                             // dependencyFlags
                                      transitionDestinationImageToMemoryReadBarrier  // barrier
        );

    commands->addChild( cmd_transitionFromTransferBarrier );

    return { commands, destinationImage };
}

std::pair<vsg::ref_ptr<vsg::Commands>, vsg::ref_ptr<vsg::Buffer>> createDepthCapture(
    vsg::ref_ptr<vsg::Device> device, const VkExtent2D& extent, vsg::ref_ptr<vsg::Image> sourceImage,
    VkFormat sourceImageFormat ) {
    auto width = extent.width;
    auto height = extent.height;

    auto memoryRequirements = sourceImage->getMemoryRequirements( device->deviceID );

    // 1. create buffer to copy to.
    VkDeviceSize bufferSize = memoryRequirements.size;
    auto destinationBuffer =
        vsg::createBufferAndMemory( device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_SHARING_MODE_EXCLUSIVE,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                                        VK_MEMORY_PROPERTY_HOST_CACHED_BIT );

    VkImageAspectFlags imageAspectFlags = vsg::computeAspectFlagsForFormat( sourceImageFormat );

    // 2.a) transition depth image for reading
    auto commands = vsg::Commands::create();

    auto transitionSourceImageToTransferSourceLayoutBarrier = vsg::ImageMemoryBarrier::create(
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,  // srcAccessMask
        VK_ACCESS_TRANSFER_READ_BIT,                                                                 // dstAccessMask
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,                                            // oldLayout
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,                                                        // newLayout
        VK_QUEUE_FAMILY_IGNORED,                                 // srcQueueFamilyIndex
        VK_QUEUE_FAMILY_IGNORED,                                 // dstQueueFamilyIndex
        sourceImage,                                             // image
        VkImageSubresourceRange{ imageAspectFlags, 0, 1, 0, 1 }  // subresourceRange
    );

    auto transitionDestinationBufferToTransferWriteBarrier =
        vsg::BufferMemoryBarrier::create( VK_ACCESS_MEMORY_READ_BIT,     // srcAccessMask
                                          VK_ACCESS_TRANSFER_WRITE_BIT,  // dstAccessMask
                                          VK_QUEUE_FAMILY_IGNORED,       // srcQueueFamilyIndex
                                          VK_QUEUE_FAMILY_IGNORED,       // dstQueueFamilyIndex
                                          destinationBuffer,             // buffer
                                          0,                             // offset
                                          bufferSize                     // size
        );

    auto cmd_transitionSourceImageToTransferSourceLayoutBarrier = vsg::PipelineBarrier::create(
        VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // srcStageMask
        VK_PIPELINE_STAGE_TRANSFER_BIT,                                                             // dstStageMask
        0,                                                                                          // dependencyFlags
        transitionSourceImageToTransferSourceLayoutBarrier,                                         // barrier
        transitionDestinationBufferToTransferWriteBarrier                                           // barrier
    );
    commands->addChild( cmd_transitionSourceImageToTransferSourceLayoutBarrier );

    // 2.b) copy image to buffer
    {
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = width;  // need to figure out actual row length from somewhere...
        region.bufferImageHeight = height;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = VkOffset3D{ 0, 0, 0 };
        region.imageExtent = VkExtent3D{ width, height, 1 };

        auto copyImage = vsg::CopyImageToBuffer::create();
        copyImage->srcImage = sourceImage;
        copyImage->srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        copyImage->dstBuffer = destinationBuffer;
        copyImage->regions.push_back( region );

        commands->addChild( copyImage );
    }

    // 2.c) transition depth image back for rendering
    auto transitionSourceImageBackToPresentBarrier = vsg::ImageMemoryBarrier::create(
        VK_ACCESS_TRANSFER_READ_BIT,                                                                 // srcAccessMask
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,  // dstAccessMask
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,                                                        // oldLayout
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,                                            // newLayout
        VK_QUEUE_FAMILY_IGNORED,                                 // srcQueueFamilyIndex
        VK_QUEUE_FAMILY_IGNORED,                                 // dstQueueFamilyIndex
        sourceImage,                                             // image
        VkImageSubresourceRange{ imageAspectFlags, 0, 1, 0, 1 }  // subresourceRange
    );

    auto transitionDestinationBufferToMemoryReadBarrier =
        vsg::BufferMemoryBarrier::create( VK_ACCESS_TRANSFER_WRITE_BIT,  // srcAccessMask
                                          VK_ACCESS_MEMORY_READ_BIT,     // dstAccessMask
                                          VK_QUEUE_FAMILY_IGNORED,       // srcQueueFamilyIndex
                                          VK_QUEUE_FAMILY_IGNORED,       // dstQueueFamilyIndex
                                          destinationBuffer,             // buffer
                                          0,                             // offset
                                          bufferSize                     // size
        );

    auto cmd_transitionSourceImageBackToPresentBarrier = vsg::PipelineBarrier::create(
        VK_PIPELINE_STAGE_TRANSFER_BIT,                                                             // srcStageMask
        VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // dstStageMask
        0,                                                                                          // dependencyFlags
        transitionSourceImageBackToPresentBarrier,                                                  // barrier
        transitionDestinationBufferToMemoryReadBarrier                                              // barrier
    );

    commands->addChild( cmd_transitionSourceImageBackToPresentBarrier );

    return { commands, destinationBuffer };
}

// ======================================================================================
// ==================== VsgRender =======================================================
// ======================================================================================

auto VsgRender::_initInstance() -> void {
    uint32_t vulkanVersion = VK_API_VERSION_1_3;

    // create instance
    vsg::Names instanceExtensions;
    vsg::Names requestedLayers;

    instanceExtensions.push_back( "VK_KHR_surface" );
    instanceExtensions.push_back( "VK_KHR_xcb_surface" );

    auto debugLayer = true;
    auto apiDumpLayer = false;

    if ( debugLayer || apiDumpLayer ) {
        instanceExtensions.push_back( VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME );
        instanceExtensions.push_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );

        requestedLayers.push_back( "VK_LAYER_KHRONOS_validation" );
        if ( apiDumpLayer ) requestedLayers.push_back( "VK_LAYER_LUNARG_api_dump" );
    }

    vsg::Names validatedNames = vsg::validateInstanceLayerNames( requestedLayers );

    _instance = vsg::Instance::create( instanceExtensions, validatedNames, vulkanVersion );
    std::tie( _physicalDevice, _queueFamily ) = _instance->getPhysicalDeviceAndQueueFamily( VK_QUEUE_GRAPHICS_BIT );
    if ( !_physicalDevice || _queueFamily < 0 ) {
        std::cout << "Could not create PhysicalDevice." << std::endl;
        std::terminate();
    }
}

auto VsgRender::_initDevice() -> void {
    vsg::Names deviceExtensions;
    deviceExtensions.push_back( VK_KHR_SWAPCHAIN_EXTENSION_NAME );
    deviceExtensions.push_back( "VK_EXT_extended_dynamic_state3" );

    vsg::QueueSettings queueSettings{ vsg::QueueSetting{ _queueFamily, { 1.0 } } };

    auto deviceFeatures = vsg::DeviceFeatures::create();
    deviceFeatures->get().samplerAnisotropy = VK_TRUE;
    deviceFeatures->get().wideLines = VK_TRUE;
    deviceFeatures->get().fillModeNonSolid = VK_TRUE;
    //deviceFeatures->get().geometryShader = enableGeometryShader;

    auto& dynamicState3Features =
        deviceFeatures->get<VkPhysicalDeviceExtendedDynamicState3FeaturesEXT,
                            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT>();
    dynamicState3Features.extendedDynamicState3PolygonMode = VK_TRUE;

    // NOTE: Error if validatedNames not empty. Not used since Vulkan API 1.0?
    // "Validation Error:
    // vkCreateDevice(): pCreateInfo->enabledLayerCount is 1 (not zero).
    // Device Layers have never worked since Vulkan 1.0 and only Instance Layers should be
    // used instead: https://docs.vulkan.org/spec/latest/appendices/legacy.html#legacy-devicelayers.
    // The Vulkan spec states: enabledLayerCount must be 0"
    vsg::Names validatedNames = vsg::validateInstanceLayerNames( {} );

    _device = vsg::Device::create( _physicalDevice, queueSettings, validatedNames, deviceExtensions, deviceFeatures );

    initExtFunctions( _device );
}

VsgRender::VsgRender() {
    _initInstance();
    _initDevice();

    {
        auto lookAt = vsg::LookAt::create( vsg::dvec3( 0.0, -16.0, 8.0 ), vsg::dvec3{ 0.0, 0.0, 0.0 },
                                           vsg::dvec3( 0.0, 0.0, 1.0 ) );

        vsg::ref_ptr<vsg::ProjectionMatrix> perspective =
            vsg::Perspective::create( 30.0, static_cast<double>( 1150 ) / static_cast<double>( 872 ), 0.01, 500.0 );

        _camera = vsg::Camera::create( perspective, lookAt, vsg::ViewportState::create( VkExtent2D{ 1150, 872 } ) );
    }

    // Create support for copying the color and depth buffers.
    {
        _colorImageView = createColorImageView( _device, _extent, _imageFormat, VK_SAMPLE_COUNT_1_BIT );
        _depthImageView = createDepthImageView( _device, _extent, _depthFormat, VK_SAMPLE_COUNT_1_BIT );
        if ( _samples == VK_SAMPLE_COUNT_1_BIT ) {
            auto renderPass = createOffscreenRenderPass( _device, _imageFormat, _depthFormat, true );
            _framebuffer = vsg::Framebuffer::create( renderPass, vsg::ImageViews{ _colorImageView, _depthImageView },
                                                     _extent.width, _extent.height, 1 );
        }

        std::tie( _colorBufferCapture, _copiedColorBuffer ) =
            createColorCapture( _device, _extent, _colorImageView->image, _imageFormat );
        std::tie( _depthBufferCapture, _copiedDepthBuffer ) =
            createDepthCapture( _device, _extent, _depthImageView->image, _depthFormat );
    }

    // Set up the RenderGraph to manage the rendering.
    {
        _renderGraph = vsg::RenderGraph::create();

        _renderGraph->framebuffer = _framebuffer;
        _renderGraph->renderArea.offset = { 0, 0 };
        _renderGraph->renderArea.extent = _extent;

        const auto clearColor = tire::Colorf{ "#92947e" };
        _renderGraph->setClearValues( { { clearColor.r(), clearColor.r(), clearColor.b(), 1.0f } },
                                      VkClearDepthStencilValue{ 0.0f, 0 } );
    }

    _sceneTransform = vsg::MatrixTransform::create();

    auto view = vsg::View::create( _camera, _sceneTransform );

    // Set up the CommandGraphs.
    vsg::CommandGraphs commandGraphs;
    {
        // NOTE: What usage of secondary command buffer in here?
        if ( false ) {
            auto secondaryCommandGraph = vsg::SecondaryCommandGraph::create( _device, _queueFamily );
            secondaryCommandGraph->addChild( view );
            secondaryCommandGraph->framebuffer = _framebuffer;
            commandGraphs.push_back( secondaryCommandGraph );

            auto executeCommands = vsg::ExecuteCommands::create();
            executeCommands->connect( secondaryCommandGraph );

            _renderGraph->contents = VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;
            _renderGraph->addChild( executeCommands );
        } else {
            _renderGraph->addChild( view );
        }

        _commandGraph = vsg::CommandGraph::create( _device, _queueFamily );
        _commandGraph->addChild( _renderGraph );
        commandGraphs.push_back( _commandGraph );

        if ( _colorBufferCapture ) {
            _commandGraph->addChild( _colorBufferCapture );
        }

        if ( _depthBufferCapture ) {
            _commandGraph->addChild( _depthBufferCapture );
        }
    }

    {
        _viewer = vsg::ref_ptr<tire::Viewer>{ new tire::Viewer{} };
        _viewer->assignRecordAndSubmitTaskAndPresentation( commandGraphs );
    }
}

auto VsgRender::needResize() const -> bool {
    return _needResize;
}

auto VsgRender::setNeedResize( bool value ) -> void {
    _needResize = value;
}

auto VsgRender::extent() const -> VkExtent2D {
    return _extent;
}

auto VsgRender::setExtent( VkExtent2D newExtent ) -> void {
    _newExtent = newExtent;
}

auto VsgRender::instance() const -> VkInstance {
    return _instance->vk();
}

auto VsgRender::queueFamily() const -> int {
    return _queueFamily;
}

auto VsgRender::physicalDevice() const -> VkPhysicalDevice {
    return _physicalDevice->vk();
}

auto VsgRender::logicalDevice() const -> VkDevice {
    return _device->vk();
}

auto VsgRender::queueFamilyIndex() const -> uint32_t {
    //TODO
    return 0;
}

auto VsgRender::vsgInstance() const -> vsg::ref_ptr<vsg::Instance> {
    return _instance;
}

auto VsgRender::vsgDevice() const -> vsg::ref_ptr<vsg::Device> {
    return _device;
}

auto VsgRender::camera() const -> vsg::ref_ptr<vsg::Camera> {
    return _camera;
}

auto VsgRender::sceneTransform() const -> vsg::ref_ptr<vsg::MatrixTransform> {
    return _sceneTransform;
}

auto VsgRender::viewer() const -> vsg::ref_ptr<tire::Viewer> {
    return _viewer;
}

auto VsgRender::passEventsToViewer() -> void {
    if ( !_bufferedEvents.empty() ) {
        _viewer->fetchEvents( _bufferedEvents );
        _bufferedEvents.clear();
    }
}

auto VsgRender::handleResize() -> void {
    if ( _needResize && ( _viewer->getFrameStamp()->frameCount > 0 ) ) {
        _needResize = false;

        _viewer->deviceWaitIdle();

        _extent = _newExtent;

        std::cout << "Resized to " << _extent.width << ", " << _extent.height << std::endl;

        auto replace_child = []( vsg::Group* group, vsg::ref_ptr<vsg::Node> previous,
                                 vsg::ref_ptr<vsg::Node> replacement ) {
            for ( auto& child : group->children ) {
                if ( child == previous ) child = replacement;
            }
        };

        auto previous_colorBufferCapture = _colorBufferCapture;
        auto previous_depthBufferCapture = _depthBufferCapture;

        _colorImageView = createColorImageView( _device, _extent, _imageFormat, VK_SAMPLE_COUNT_1_BIT );
        _depthImageView = createDepthImageView( _device, _extent, _depthFormat, VK_SAMPLE_COUNT_1_BIT );
        if ( _samples == VK_SAMPLE_COUNT_1_BIT ) {
            auto renderPass = vsg::createRenderPass( _device, _imageFormat, _depthFormat, true );
            _framebuffer = vsg::Framebuffer::create( renderPass, vsg::ImageViews{ _colorImageView, _depthImageView },
                                                     _extent.width, _extent.height, 1 );
        }

        _renderGraph->framebuffer = _framebuffer;

        // create new copy subgraphs
        std::tie( _colorBufferCapture, _copiedColorBuffer ) =
            createColorCapture( _device, _extent, _colorImageView->image, _imageFormat );
        std::tie( _depthBufferCapture, _copiedDepthBuffer ) =
            createDepthCapture( _device, _extent, _depthImageView->image, _depthFormat );

        replace_child( _commandGraph, previous_colorBufferCapture, _colorBufferCapture );
        replace_child( _commandGraph, previous_depthBufferCapture, _depthBufferCapture );
    }
}

auto VsgRender::renderNative() -> std::optional<std::tuple<VkImage, VkExtent2D, VkImageLayout>> {
    this->handleResize();

    _viewer->advanceToNextFrame();

    // NOTE: Vewer::advanceToNextFrame() discards previous events (see call pollEvents(true)),
    // i.e. clear _events list in viewer, then pass window events to viewer after it.
    this->passEventsToViewer();

    _viewer->handleEvents();

    _viewer->update();

    _viewer->recordAndSubmit();

    if ( !_copiedColorBuffer ) {
        return std::nullopt;
    }

    _viewer->waitForFences( 0, UINT64_MAX );

    return std::make_tuple( _copiedColorBuffer->vk( _device->deviceID ), _extent,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
}

}  // namespace tire
