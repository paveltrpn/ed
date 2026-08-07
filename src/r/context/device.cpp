
#include <algorithm>
#include <optional>
#include <set>
#include <ranges>

#define VK_USE_PLATFORM_XLIB_KHR
#include <vulkan/vk_enum_string_helper.h>

#include "vkinstance.h"
#include "device.h"
#include "surface.h"
#include "log/log.h"
#include "config/config.h"

namespace tire {

VKDevice::VKDevice( const VKInstance *instance )
    : instance_{ instance } {
    collectPhysicalDevices();
    createDevice();
}

VKDevice::~VKDevice() {
    vkDestroyDevice( device_, nullptr );
}

void VKDevice::collectPhysicalDevices() {
    uint32_t devCount{};

    // Enumerate physical devices
    if ( const auto err = vkEnumeratePhysicalDevices( instance_->get(), &devCount, nullptr ); err != VK_SUCCESS ) {
        log::fatal()( "can't enumerate physical devices with code: {}", string_VkResult( err ) );
    } else {
        log::debug()( "physical devices enumerate success, count: {}", devCount );
    }

    if ( devCount == 0 ) {
        log::fatal()( "no vulkan physical devices in system" );
    }

    std::vector<VkPhysicalDevice> physicalDevices{};
    physicalDevices.resize( devCount );

    // Get physical devices
    {
        const auto err = vkEnumeratePhysicalDevices( instance_->get(), &devCount, physicalDevices.data() );
        if ( err != VK_SUCCESS ) {
            log::fatal()( "can't acquire physical devices with code: {}", string_VkResult( err ) );
        } else {
            log::debug()( "physical devices acquire success" );
        }
    }

    // Get physical device info for each device
    log::info()( "collect physical device info..." );
    for ( const auto device : physicalDevices ) {
        // Collect physical devices and its properties

        VkPhysicalDeviceProperties devProps;
        vkGetPhysicalDeviceProperties( device, &devProps );
        log::info()( "device name: {}", devProps.deviceName );

        VkPhysicalDeviceFeatures devFeatures;
        vkGetPhysicalDeviceFeatures( device, &devFeatures );

        // Collect physical device queue families properies
        uint32_t queueFamilyCount{};
        vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, nullptr );

        std::vector<VkQueueFamilyProperties> qfp{};
        qfp.resize( queueFamilyCount );
        vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, qfp.data() );
        log::debug()( "device queue family count: {}", queueFamilyCount );

        // Collect physical device extensions info
        uint32_t extensionCount{};
        if ( const auto err = vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, nullptr );
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
                devProps.deviceName, extensionCount );
        }

        std::vector<VkExtensionProperties> availableExtensions;
        availableExtensions.resize( extensionCount );

        if ( const auto err =
                 vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, availableExtensions.data() );
             err != VK_SUCCESS ) {
            log::fatal()(
                "can't acquire physical device extensions "
                "with code: {}",
                string_VkResult( err ) );
        } else {
            log::debug()(
                "physical device extensions acquired for "
                "device: "
                "{}",
                devProps.deviceName );
        }

        // Store physical device in list
        physicalDevices_.emplace_back( PhysicalDevice{ .device = device,
                                                       .properties = devProps,
                                                       .features = devFeatures,
                                                       .extensions = availableExtensions,
                                                       .queueFamilyProperties = qfp } );
    }
}

auto VKDevice::pickDevice( const std::vector<PhysicalDevice> &physDevList ) -> std::optional<int> {
    // Check which devices available on machine.
    int discreetGpuId{ -1 };
    int integratedGpuId{ -1 };
    int otherGpuId{ -1 };
    int virtualGpuId{ -1 };
    int cpuGpuId{ -1 };
    for ( auto i{ 0 }; const auto &physicalDevice : physicalDevices_ ) {
        const auto &deviceProps = physicalDevice.properties;
        switch ( physicalDevice.properties.deviceType ) {
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: {
                discreetGpuId = i;
                break;
            }
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: {
                integratedGpuId = i;
                break;
            }
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: {
                virtualGpuId = i;
                break;
            }
            case VK_PHYSICAL_DEVICE_TYPE_CPU: {
                cpuGpuId = i;
                break;
            }
            case VK_PHYSICAL_DEVICE_TYPE_OTHER: {
                otherGpuId = i;
                break;
            }
            default: {
                break;
            }
        }
        ++i;
    }

    // First try to pick discreete GPU.
    if ( discreetGpuId != -1 ) {
        return discreetGpuId;
    } else if ( integratedGpuId != -1 ) {
        return integratedGpuId;
    } else if ( cpuGpuId != -1 ) {
        return cpuGpuId;
    }

    return std::nullopt;
}

auto VKDevice::createDevice() -> void {
    {
        const auto d = pickDevice( physicalDevices_ );
        if ( !d ) {
            log::fatal()( "no suitable vulkan devices found!" );
        } else {
            pickedPhysicalDeviceId_ = d.value();
        }
    }

    // Base class member.
    physicalDevice_ = physicalDevices_[pickedPhysicalDeviceId_].device;

    log::info()( "pick {}", physicalDevices_[pickedPhysicalDeviceId_].properties.deviceName );

    // Choose queue family with VK_QUEUE_GRAPHICS_BIT.
    // The good news is that
    // any queue family with VK_QUEUE_GRAPHICS_BIT or VK_QUEUE_COMPUTE_BIT ca-
    // pabilities already implicitly support VK_QUEUE_TRANSFER_BIT operations.
    for ( auto i{ 0 }; const auto &queueFamily : physicalDevices_[pickedPhysicalDeviceId_].queueFamilyProperties ) {
        if ( queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT ) {
            graphicsFamilyQueueId_ = i;
            break;
        }
        ++i;
    }

    if ( graphicsFamilyQueueId_ == UINT32_MAX ) {
        log::fatal()(
            "failed to get device with queue family that "
            "VK_QUEUE_GRAPHICS_BIT" );
    }

    // NOTE: just an assumption!
    presentSupportQueueId_ = graphicsFamilyQueueId_;

    log::debug()( "graphics family: {}", graphicsFamilyQueueId_ );
    log::debug()( "present family: {}", presentSupportQueueId_ );

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    const std::set<uint32_t> uniqueQueueFamilies = { graphicsFamilyQueueId_, presentSupportQueueId_ };

    const float queuePriority{ 1.0f };
    for ( const uint32_t queueFamily : uniqueQueueFamilies ) {
        VkDeviceQueueCreateInfo queueCreateInfo{ .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                                 .queueFamilyIndex = queueFamily,
                                                 .queueCount = 1,
                                                 .pQueuePriorities = &queuePriority };
        queueCreateInfos.push_back( queueCreateInfo );
    }

    std::vector<const char *> desiredExtensionsList{};

    auto loadBaseExt = [&desiredExtensionsList]( bool use ) -> void {
        if ( use ) {
            auto ext = std::vector<const char *>{ "VK_KHR_swapchain", "VK_KHR_spirv_1_4" };
            std::ranges::for_each(
                ext, [&desiredExtensionsList]( auto e ) -> void { desiredExtensionsList.emplace_back( e ); } );
        }
    };

    auto loadRTExt = [&desiredExtensionsList]( bool use ) -> void {
        if ( use ) {
            auto ext = std::vector<const char *>{ "VK_KHR_ray_query",
                                                  "VK_KHR_ray_tracing_pipeline",
                                                  "VK_KHR_ray_tracing_position_fetch",
                                                  "VK_KHR_ray_tracing_maintenance1",
                                                  "VK_KHR_acceleration_structure",
                                                  "VK_EXT_descriptor_indexing",
                                                  "VK_KHR_buffer_device_address",
                                                  "VK_KHR_deferred_host_operations",
                                                  "VK_KHR_shader_float_controls" };
            std::ranges::for_each(
                ext, [&desiredExtensionsList]( auto e ) -> void { desiredExtensionsList.emplace_back( e ); } );
        }
    };

    loadBaseExt( true );
    loadRTExt( Config::instance().get<bool>( "enable_raytracing_extensions" ) );

    auto deviceCreateInfo = VkDeviceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast<uint32_t>( queueCreateInfos.size() ),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledExtensionCount = static_cast<uint32_t>( desiredExtensionsList.size() ),
        .ppEnabledExtensionNames = desiredExtensionsList.data(),
        .pEnabledFeatures = &physicalDevices_[pickedPhysicalDeviceId_].features,
    };

    // Force use validation layers
    if ( Config::instance().get<bool>( "enable_validation_layers" ) ) {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>( instance_->desiredValidationLayerList().size() );
        deviceCreateInfo.ppEnabledLayerNames = instance_->desiredValidationLayerList().data();
    } else {
        deviceCreateInfo.enabledLayerCount = 0;
        deviceCreateInfo.ppEnabledLayerNames = nullptr;
    }

    // Create a logical device
    if ( const auto err = vkCreateDevice( physicalDevice_, &deviceCreateInfo, nullptr, &device_ ); err != VK_SUCCESS ) {
        log::fatal()( "failed to create logical device with code {}!\n", string_VkResult( err ) );
    } else {
        log::info()( "logical device create success!" );
    }

    // Graphic and present queue id
    vkGetDeviceQueue( device_, graphicsFamilyQueueId_, 0, &graphicsQueue_ );
    vkGetDeviceQueue( device_, presentSupportQueueId_, 0, &presentQueue_ );
}

auto VKDevice::get() const -> VkDevice {
    return device_;
}

auto VKDevice::presentQueue() const -> VkQueue {
    return presentQueue_;
}

auto VKDevice::graphicsQueue() const -> VkQueue {
    return graphicsQueue_;
}

auto VKDevice::physicalDevice() const -> VkPhysicalDevice {
    return physicalDevice_;
}

auto VKDevice::graphicsFamilyQueueId() const -> uint32_t {
    return graphicsFamilyQueueId_;
}

auto VKDevice::presentSupportQueueId() const -> uint32_t {
    return presentSupportQueueId_;
}

}  // namespace tire
