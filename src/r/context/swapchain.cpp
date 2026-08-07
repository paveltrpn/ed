#include <format>
#include <memory>

#define VK_USE_PLATFORM_XLIB_KHR
#include <vulkan/vk_enum_string_helper.h>

#include "context.h"
#include "depth_image.h"
#include "log/log.h"
#include "config/config.h"

namespace tire {

auto Context::makeSwapchain() -> void {
    log::info()(
        "surface capabilities minImageCount: {}, "
        "maxImageCount: {}",
        surfaceCapabilities().minImageCount, surfaceCapabilities().maxImageCount );

    // NOTE: Warning from validation layers:
    // A Swapchain is being created with minImageCount set to 2, which means double buffering
    // is going to be used. Using double buffering and vsync locks rendering to an integer
    // fraction of the vsync rate. In turn, reducing the performance of the application if
    // rendering is slower than vsync. Consider setting minImageCount to 3 to use
    // triple buffering to maximize performance in such cases.
    // Skip all logic above, just use value from config
    framesCount_ = Config::instance().get<int>( "frame_count" );

    //if ( ( framesCount_ < surfaceCapabilities_.minImageCount ) ||
    //    ( framesCount_ > surfaceCapabilities_.maxImageCount ) ) {
    //  log::fatal(
    //      "vk::Swapchain === desired frame count not fit to available "
    //      "surface image count limits" );
    //}

    log::debug()(
        "vulkan swapchain surface capabilities image count "
        "set to "
        "{}",
        framesCount_ );

    // Use instead of extent acuired from surface data.
    const auto [viewportWidth, viewportHeight] = viewportSize();

    VkSwapchainCreateInfoKHR createInfo{
        //
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface(),
        // The implementation will either create the swapchain with at least
        // that many images, or it will fail to create the swapchain.
        .minImageCount = framesCount_,
        .imageFormat = surfaceFormat().format,
        .imageColorSpace = surfaceFormat().colorSpace,
        .imageExtent = { viewportWidth, viewportHeight },  //currentExtent_,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = surfaceCapabilities().currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode(),
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    // Choose image sharing mode for swapchain images. That useful if graphics queue
    // dont support present and we intendet to use differt queue to preset.
    std::array<uint32_t, 2> queueFamilyIndices = { graphicsFamilyQueueId(), presentSupportQueueId() };
    if ( graphicsFamilyQueueId() != presentSupportQueueId() ) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    {
        const auto err = vkCreateSwapchainKHR( device(), &createInfo, nullptr, &swapchain_ );
        if ( err != VK_SUCCESS ) {
            log::fatal()( "failed to create swapchain code {}\n!", string_VkResult( err ) );
        } else {
            log::info()( "vulkan swapchain created!" );
        }
    }

    // Get swapchain images count. Sudden, this number is
    // equal to previously defined in VkSwapchainCreateInfoKHR.minImageCount (== framesCount_).
    // But we still try to get image count that way.
    {
        const auto err = vkGetSwapchainImagesKHR( device(), swapchain_, &swapchainImageCount_, nullptr );
        if ( err != VK_SUCCESS ) {
            log::fatal()( "failed to get swapchain images count with code {}\n!", string_VkResult( err ) );
        } else {
            log::debug()( "swapchain images count: {}", swapchainImageCount_ );
        }
    }

    // Depth image
    depthImage_ = std::make_shared<DepthImage>( this, viewportWidth, viewportHeight );
}

auto Context::makeFrames() -> void {
    // Reserve space for frames images render into
    frames_.resize( framesCount_ );

    // Acquire all swapchain images at one call
    std::vector<VkImage> swapChainImages;
    swapChainImages.resize( framesCount_ );
    if ( const auto err = vkGetSwapchainImagesKHR( device(), swapchain_, &framesCount_, swapChainImages.data() );
         err != VK_SUCCESS ) {
        log::fatal()( "failed to get swapchain images with code {}\n!", string_VkResult( err ) );
    } else {
        log::debug()( "images acquired!" );
    }

    // Create frame related vulkan entities - images, image views, framebuffers and sync primitieves.
    for ( size_t i{ 0 }; i < framesCount_; ++i ) {
        // Frame image
        frames_[i].image_ = swapChainImages[i];

        // Frame image view
        const auto subResRange = VkImageSubresourceRange{
            //
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };

        const VkImageViewCreateInfo createInfo{
            //
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapChainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = surfaceFormat().format,
            .components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                            VK_COMPONENT_SWIZZLE_IDENTITY },
            .subresourceRange = subResRange,
        };

        if ( const auto err = vkCreateImageView( device(), &createInfo, nullptr, &frames_[i].view_ );
             err != VK_SUCCESS ) {
            log::fatal()( "failed to create swapchain image views with code {}\n!", string_VkResult( err ) );
        } else {
            log::debug()( "image view {} created!", i );
        }

        // Frame framebuffer
        std::array<VkImageView, 2> attachments = { frames_[i].view_, depthImage_->view() };
        const auto [viewportWidth, viewportHeight] = viewportSize();
        const VkFramebufferCreateInfo framebufferInfo{
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = renderPass(),
            .attachmentCount = static_cast<uint32_t>( attachments.size() ),
            .pAttachments = attachments.data(),
            .width = viewportWidth,
            .height = viewportHeight,
            .layers = 1,
        };

        if ( const auto err = vkCreateFramebuffer( device(), &framebufferInfo, nullptr, &frames_[i].framebuffer_ );
             err != VK_SUCCESS ) {
            log::fatal()( "failed to create framebuffer at {} with code {}!", i, string_VkResult( err ) );
        } else {
            log::debug()( "framebuffer {} created!", i );
        }

        // Frame synchronization primitieves
        VkSemaphoreCreateInfo semaphoreInfo{
            //
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
        };

        VkFenceCreateInfo fenceInfo{
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .pNext = nullptr, .flags = VK_FENCE_CREATE_SIGNALED_BIT };

        if ( vkCreateSemaphore( device(), &semaphoreInfo, nullptr, &frames_[i].imageAvailableSemaphore_ ) !=
                 VK_SUCCESS ||
             vkCreateSemaphore( device(), &semaphoreInfo, nullptr, &frames_[i].renderFinishedSemaphore_ ) !=
                 VK_SUCCESS ||
             vkCreateFence( device(), &fenceInfo, nullptr, &frames_[i].inFlightFence_ ) != VK_SUCCESS ) {
            log::fatal()( "failed to create semaphores!" );
        }

        const VkCommandBufferAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = commandPool(),
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        const auto err = vkAllocateCommandBuffers( device(), &allocInfo, &frames_[i].cbPrimary_ );
        if ( err != VK_SUCCESS ) {
            log::fatal()( "failed to allocate command buffers with code {}!", string_VkResult( err ) );
        } else {
            log::debug()( "primary command buffer created!" );
        };
    }
}

}  // namespace tire
