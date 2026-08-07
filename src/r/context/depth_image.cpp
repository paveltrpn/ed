
#include "depth_image.h"
#include "log/log.h"

namespace tire {


    DepthImage::DepthImage( const Context *context, uint32_t width, uint32_t height )
        : context_{ context }
        , width_{ width }
        , height_{ height }
        , depthFormat_{
              findSupportedFormat( { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
                                   VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT ) } {
        //

        initDeviceImage();
        initImageView();
        transitionImageLayout();
    }


    auto DepthImage::view() const -> VkImageView {
        //
        return depthImageView_;
    }

    auto  DepthImage::clean() -> void {
        //
        vmaDestroyImage( context_->allocator(), depthImage_, depthImageAllocation_ );
        vkDestroyImageView( context_->device(), depthImageView_, nullptr );
    }

     DepthImage::~DepthImage() {
        //
        clean();
    };

    auto  DepthImage::initDeviceImage() -> void {
        const auto imageExtent = VkExtent3D{
            //
            .width = width_,
            .height = height_,
            .depth = 1,
        };

        const auto imgCreateInfo = VkImageCreateInfo{
            //
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .flags = 0,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = depthFormat_,
            .extent = imageExtent,
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        const auto allocCreateInfo = VmaAllocationCreateInfo{
            .flags = 0,
            .usage = VMA_MEMORY_USAGE_GPU_ONLY,
            .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        };

        {
            const auto err = vmaCreateImage( context_->allocator(), &imgCreateInfo, &allocCreateInfo, &depthImage_,
                                             &depthImageAllocation_, nullptr );
            if ( err != VK_SUCCESS ) {
                log::fatal()( "error while creating device image {}", string_VkResult( err ) );
            }
        }
    }

    auto  DepthImage::initImageView() -> void {
        const auto subResRange = VkImageSubresourceRange{
            //
            .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };

        VkImageViewCreateInfo imageinfo = {
            //
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = depthImage_,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = depthFormat_,
            .subresourceRange = subResRange,
        };

        {
            const auto err = vkCreateImageView( context_->device(), &imageinfo, nullptr, &depthImageView_ );
            if ( err != VK_SUCCESS ) {
                log::fatal()( "error while create image view {}", string_VkResult( err ) );
            }
        }
    }

    auto  DepthImage::transitionImageLayout() -> void {
        //
        auto c = context_->immediateCommand();

        const auto oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        const auto newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        const auto range = VkImageSubresourceRange{
            //
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };

        VkImageMemoryBarrier barrier{
            //
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = depthImage_,
            .subresourceRange = range,
        };

        VkPipelineStageFlags sourceStage{};
        VkPipelineStageFlags destinationStage{};

        if ( newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            // Has stncil component?
            if ( depthFormat_ == VK_FORMAT_D32_SFLOAT_S8_UINT || depthFormat_ == VK_FORMAT_D24_UNORM_S8_UINT ) {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        } else {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        if ( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) {
            barrier.srcAccessMask = 0;

            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if ( oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                    newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if ( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
                    newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ) {
            barrier.srcAccessMask = 0;

            barrier.dstAccessMask =
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        } else {
            log::fatal()( "unsupported layout transition!" );
        }
        /*
            if ( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
                 newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            } else if ( oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                        newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) {
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            } else {
                throw std::invalid_argument( "unsupported layout transition!" );
            }
           */

        vkCmdPipelineBarrier( c.buf(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier );
    }

    auto  DepthImage::findSupportedFormat( const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                            VkFormatFeatureFlags features ) const -> VkFormat {
        for ( VkFormat format : candidates ) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties( context_->physicalDevice(), format, &props );

            if ( tiling == VK_IMAGE_TILING_LINEAR && ( props.linearTilingFeatures & features ) == features ) {
                return format;
            } else if ( tiling == VK_IMAGE_TILING_OPTIMAL && ( props.optimalTilingFeatures & features ) == features ) {
                return format;
            }
        }

        log::fatal()( "failed to find supported format!" );

        // Silence warning
        return {};
    }

}  // namespace tire
