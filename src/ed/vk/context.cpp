
#include "vulkan/vulkan.h"
#include <vulkan/vk_enum_string_helper.h>

#include "context.h"
#include "log/log.h"

namespace tire::vk {

Context::Context( VkInstance instance, VkPhysicalDevice pDevice, VkDevice device, VkSurfaceKHR surface, VkRenderPass rp,
                  uint32_t graphicsFamilyQueueId, uint32_t graphicsQueueId )
    : instance_{ instance }
    , pDevice_{ pDevice }
    , device_{ device }
    , surface_{ surface }
    , renderPass_{ rp }
    , graphicsFamilyQueueId_{ graphicsFamilyQueueId }
    , graphicsQueueId_{ graphicsQueueId } {
    //

    uint32_t version{};
    vkEnumerateInstanceVersion( &version );
    log::info()( "acquired api instance version is {}.{}", VK_API_VERSION_MAJOR( version ),
                 VK_API_VERSION_MINOR( version ) );

    queryDeviceInfo();
    querySurface();

    // Graphic queue.
    vkGetDeviceQueue( device_, graphicsFamilyQueueId_, graphicsQueueId_, &graphicsQueue_ );
}

auto Context::queryDeviceInfo() -> void {
    // Collect physical devices and its properties
    vkGetPhysicalDeviceProperties( pDevice_, &pDeviceProperties_ );
    log::info()( "device name: {}", pDeviceProperties_.deviceName );

    vkGetPhysicalDeviceFeatures( pDevice_, &pDeviceFeatures_ );

    // Collect physical device queue families properies
    uint32_t queueFamilyCount{};
    vkGetPhysicalDeviceQueueFamilyProperties( pDevice_, &queueFamilyCount, nullptr );

    queueFamilyProperties_.resize( queueFamilyCount );
    vkGetPhysicalDeviceQueueFamilyProperties( pDevice_, &queueFamilyCount, queueFamilyProperties_.data() );
    log::debug()( "device queue family count: {}", queueFamilyCount );

    // Collect physical device extensions info
    uint32_t extensionCount{};
    if ( const auto err = vkEnumerateDeviceExtensionProperties( pDevice_, nullptr, &extensionCount, nullptr );
         err != VK_SUCCESS ) {
        log::fatal()(
            "can't enumerate physical device extensions "
            "with code: {}\n",
            string_VkResult( err ) );
    } else {
        log::debug()(
            "physical device extensions enumerated for "
            "device: {}, "
            "count: {}",
            pDeviceProperties_.deviceName, extensionCount );
    }

    pDeviceExtensions_.resize( extensionCount );
    if ( const auto err =
             vkEnumerateDeviceExtensionProperties( pDevice_, nullptr, &extensionCount, pDeviceExtensions_.data() );
         err != VK_SUCCESS ) {
        log::fatal()(
            "can't acquire physical device extensions "
            "with code: {}\n",
            string_VkResult( err ) );
    } else {
        log::debug()(
            "physical device extensions acquired for "
            "device: "
            "{}",
            pDeviceProperties_.deviceName );
    }

    log::info()(
        "vulkan "
        "info:\ndev:\t{}\ndrv:\t{}\napi:\t{}",
        getDeviceNameString(), getDeviceDriverVersionString(), getApiVersionString() );
}

auto Context::querySurface() -> void {
    if ( surface_ == VK_NULL_HANDLE ) {
        log::error()( "can not get surface handle for window!" );
    }

    // Physical device surface capabilities.
    // NOTE: got error on amd APU.
    if ( const auto err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR( pDevice_, surface_, &surfaceCapabilities_ );
         err != VK_SUCCESS ) {
        log::fatal()(
            "failed to obtain surface capabilities with code "
            "{}!\n",
            string_VkResult( err ) );
    } else {
        log::debug()(
            "physical device surface capabilities acquire "
            "success!" );
    }
}

auto Context::memoryRequirements( uint32_t typeFilter, VkMemoryPropertyFlags properties ) const -> uint32_t {
    VkPhysicalDeviceMemoryProperties memProperties{};
    vkGetPhysicalDeviceMemoryProperties( pDevice_, &memProperties );

    for ( uint32_t i = 0; i < memProperties.memoryTypeCount; i++ ) {
        if ( ( typeFilter & ( 1 << i ) ) &&
             ( memProperties.memoryTypes[i].propertyFlags & properties ) == properties ) {
            return i;
        }
    }

    log::fatal()( "failed to find suitable memory type!" );

    // Silence warning
    return {};
}

auto Context::findSupportedFormat( const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                   VkFormatFeatureFlags features ) const -> VkFormat {
    for ( VkFormat format : candidates ) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties( pDevice_, format, &props );

        if ( tiling == VK_IMAGE_TILING_LINEAR && ( props.linearTilingFeatures & features ) == features ) {
            return format;
        } else if ( tiling == VK_IMAGE_TILING_OPTIMAL && ( props.optimalTilingFeatures & features ) == features ) {
            return format;
        }
    }

    throw std::runtime_error( std::format( "failed to find supported format!" ) );

    // Silence warning
    return {};
}

}  // namespace tire::vk
