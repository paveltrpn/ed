
#include "mutableformatimage.h"

namespace vsg {

VkResult MutableFormatImage::compile( Device* device ) {
    auto& vd = _vulkanData[device->deviceID];

    if ( vd.image != VK_NULL_HANDLE ) {
        return VK_SUCCESS;
    }

    VkFormat formats[] = { VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_R8G8B8A8_UNORM };

    VkImageFormatListCreateInfo formatList{
        .sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO, .viewFormatCount = 2, .pViewFormats = formats };

    VkImageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext = &formatList;
    info.flags = flags | VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
    info.imageType = imageType;
    info.format = format;
    info.extent = extent;
    info.mipLevels = mipLevels;
    info.arrayLayers = arrayLayers;
    info.samples = samples;
    info.tiling = tiling;
    info.usage = usage;
    info.sharingMode = sharingMode;
    info.queueFamilyIndexCount = static_cast<uint32_t>( queueFamilyIndices.size() );
    info.pQueueFamilyIndices = queueFamilyIndices.data();
    info.initialLayout = initialLayout;

    vd.device = device;

    vd.requiresDataCopy = data.valid();

    VkResult result = vkCreateImage( *vd.device, &info, vd.device->getAllocationCallbacks(), &vd.image );

    if ( result != VK_SUCCESS ) {
        throw Exception{ "Error: Failed to create VkImage.", result };
    }

    return result;
}

}  // namespace vsg
