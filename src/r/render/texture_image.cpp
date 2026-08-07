
#include <cstring>

#include "texture_image.h"

#include "r/context/context.h"
#include "log/log.h"
#include "image/image.h"
#include "image/tga.h"
#include "log/log.h"

namespace tire {

TextureImage::TextureImage( const std::string &fname )
    : imageFormat_{ VK_FORMAT_R8G8B8A8_SRGB } {
    //
    tire::Tga textureData{ fname };

    VkDeviceSize imageSize = textureData.width() * textureData.height() * textureData.components();

    imageExtent_ = VkExtent3D{
        //
        .width = static_cast<uint32_t>( textureData.width() ),
        .height = static_cast<uint32_t>( textureData.height() ),
        .depth = 1,
    };

    initStagingBuffer( imageSize );
    initDeviceImage( imageSize );
    uploadToStaging( textureData.data(), imageSize );
    uploadCmd();
    generateMipmaps( deviceImage_, imageExtent_.width, imageExtent_.height );
    initImageView();
}

auto TextureImage::view() const -> VkImageView {
    //
    return imageView_;
}

TextureImage::~TextureImage() {
    //
    clean();
};

auto TextureImage::clean() -> void {
    //
    vmaDestroyImage( Context::instance().allocator(), deviceImage_, deviceAllocation_ );
    vmaDestroyBuffer( Context::instance().allocator(), stagingBuffer_, stagingAllocation_ );
    vkDestroyImageView( Context::instance().device(), imageView_, nullptr );
}

auto TextureImage::initStagingBuffer( VkDeviceSize size ) -> void {
    const auto stagingBufferInfo = VkBufferCreateInfo{
        //
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .size = size,
        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    };

    const auto vmaallocInfo = VmaAllocationCreateInfo{
        //
        .usage = VMA_MEMORY_USAGE_CPU_ONLY,
    };

    {
        const auto err = vmaCreateBuffer( Context::instance().allocator(), &stagingBufferInfo, &vmaallocInfo,
                                          &stagingBuffer_, &stagingAllocation_, nullptr );
        if ( err != VK_SUCCESS ) {
            log::fatal()( "error while creating staging buffer {}", string_VkResult( err ) );
        }
    }
}

auto TextureImage::uploadToStaging( const void *data, VkDeviceSize size ) -> void {
    void *mappedPtr{};
    vmaMapMemory( Context::instance().allocator(), stagingAllocation_, &mappedPtr );
    memcpy( mappedPtr, data, size );
    vmaUnmapMemory( Context::instance().allocator(), stagingAllocation_ );
}

auto TextureImage::initDeviceImage( VkDeviceSize size ) -> void {
    const auto imgCreateInfo = VkImageCreateInfo{
        //
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .flags = 0,  // Optional
        .imageType = VK_IMAGE_TYPE_2D,
        .format = imageFormat_,
        .extent = imageExtent_,
        .mipLevels = mipLevels_,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    const auto allocCreateInfo = VmaAllocationCreateInfo{
        .flags = 0,
        .usage = VMA_MEMORY_USAGE_GPU_ONLY,
        .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    };

    {
        const auto err = vmaCreateImage( Context::instance().allocator(), &imgCreateInfo, &allocCreateInfo,
                                         &deviceImage_, &deviceAllocation_, nullptr );
        if ( err != VK_SUCCESS ) {
            log::fatal()( "error while creating device image {}", string_VkResult( err ) );
        }
    }
}

auto TextureImage::uploadCmd() -> void {
    auto c = Context::instance().immediateCommand();

    const auto range = VkImageSubresourceRange{
        //
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    const auto imageTransferBarrier = VkImageMemoryBarrier{
        //
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .image = deviceImage_,
        .subresourceRange = range,
    };

    // Barrier the image into the transfer-receive layout.
    vkCmdPipelineBarrier( c.buf(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                          nullptr, 1, &imageTransferBarrier );

    const auto imageSubresource = VkImageSubresourceLayers{
        //
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .mipLevel = 0,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    const auto copyRegion = VkBufferImageCopy{
        //
        .bufferOffset = 0,           .bufferRowLength = 0, .bufferImageHeight = 0, .imageSubresource = imageSubresource,
        .imageExtent = imageExtent_,
    };

    // Copy the buffer into the image.
    vkCmdCopyBufferToImage( c.buf(), stagingBuffer_, deviceImage_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                            &copyRegion );

    VkImageMemoryBarrier imageReadableBarrier = imageTransferBarrier;

    imageReadableBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageReadableBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageReadableBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageReadableBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    // Barrier the image into the shader readable layout.
    vkCmdPipelineBarrier( c.buf(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
                          0, nullptr, 1, &imageReadableBarrier );
}

auto TextureImage::initImageView() -> void {
    const auto subResRange = VkImageSubresourceRange{
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = mipLevels_,
        .baseArrayLayer = 0,
        .layerCount = 1,

    };

    VkImageViewCreateInfo imageinfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = deviceImage_,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = VK_FORMAT_R8G8B8A8_SRGB,
        .components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B,
                        VK_COMPONENT_SWIZZLE_A },
        .subresourceRange = subResRange,
    };

    {
        const auto err = vkCreateImageView( Context::instance().device(), &imageinfo, nullptr, &imageView_ );
        if ( err != VK_SUCCESS ) {
            log::fatal()( "error while create image view {}", string_VkResult( err ) );
        }
    }
}

auto TextureImage::generateMipmaps( VkImage image, uint32_t texWidth, uint32_t texHeight ) -> void {
    auto c = Context::instance().immediateCommand();

    const auto subResource = VkImageSubresourceRange{
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
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = subResource,
    };

    auto mipWidth = static_cast<int32_t>( texWidth );
    auto mipHeight = static_cast<int32_t>( texHeight );

    for ( uint32_t i = 1; i < mipLevels_; i++ ) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier( c.buf(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                              nullptr, 1, &barrier );

        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage( c.buf(), image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR );

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier( c.buf(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
                              nullptr, 0, nullptr, 1, &barrier );

        if ( mipWidth > 1 ) mipWidth /= 2;
        if ( mipHeight > 1 ) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels_ - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier( c.buf(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
                          0, nullptr, 1, &barrier );
}

auto TextureImage::mipLevels() const -> uint32_t {
    //
    return mipLevels_;
}

}  // namespace tire
