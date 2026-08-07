
#pragma once

#include <print>
#include <vector>
#include <expected>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace tire::vk {

struct Context final {
    Context( VkInstance instance, VkPhysicalDevice pDevice, VkDevice device, VkSurfaceKHR surface, VkRenderPass rp,
             uint32_t graphicsFamilyQueueId, uint32_t graphicsQueueId );

    ~Context() = default;

    Context( const Context& other ) = delete;
    Context( Context&& other ) = delete;
    Context& operator=( const Context& other ) = delete;
    Context& operator=( Context&& other ) = delete;

    auto queryDeviceInfo() -> void;
    auto querySurface() -> void;

    [[nodiscard]] auto instance() const -> VkInstance {
        //
        return instance_;
    };

    [[nodiscard]] auto device() const -> VkDevice {
        //
        return device_;
    };

    [[nodiscard]] auto renderPass() const -> VkRenderPass {
        //
        return renderPass_;
    };

    // [[nodiscard]] const VkSurfaceFormatKHR& surfaceFormat() const {
    // return surfaceFormat_;
    // };

    [[nodiscard]] auto memoryRequirements( uint32_t typeFilter, VkMemoryPropertyFlags properties ) const -> uint32_t;

    [[nodiscard]] auto findSupportedFormat( const std::vector<VkFormat>& candidates, VkImageTiling tiling,
                                            VkFormatFeatureFlags features ) const -> VkFormat;

    // [[nodiscard]] auto framesCount() const -> uint32_t { return framesCount_; };

    // [[nodiscard]] const VkQueue& graphicsQueue() const {
    // return graphicsQueue_;
    // }

    //[[nodiscard]] VkQueue presentQueue() const { return presentQueue_; };

    [[nodiscard]] auto graphicsFamily() const -> uint32_t {
        //
        return graphicsFamilyQueueId_;
    };

    [[nodiscard]] auto currentExtent() const -> const VkExtent2D& {
        //
        return surfaceCapabilities_.currentExtent;
    };

    [[nodiscard]] auto getDeviceNameString() -> std::string {
        //
        return pDeviceProperties_.deviceName;
    }

    [[nodiscard]] auto getDeviceDriverVersionString() -> std::string {
        return std::format( "{}.{}.{}", VK_VERSION_MAJOR( pDeviceProperties_.driverVersion ),
                            VK_VERSION_MINOR( pDeviceProperties_.driverVersion ),
                            VK_VERSION_PATCH( pDeviceProperties_.driverVersion ) );
    }

    [[nodiscard]] auto getApiVersionString() -> std::string {
        return std::format( "{}.{}.{}", VK_VERSION_MAJOR( pDeviceProperties_.apiVersion ),
                            VK_VERSION_MINOR( pDeviceProperties_.apiVersion ),
                            VK_VERSION_PATCH( pDeviceProperties_.apiVersion ) );
    }

private:
    // Instance
    VkInstance instance_{ VK_NULL_HANDLE };

    // Physical device
    VkPhysicalDevice pDevice_{ VK_NULL_HANDLE };
    VkPhysicalDeviceProperties pDeviceProperties_{};
    VkPhysicalDeviceFeatures pDeviceFeatures_{};
    std::vector<VkExtensionProperties> pDeviceExtensions_{};
    std::vector<VkQueueFamilyProperties> queueFamilyProperties_{};

    // Logical device
    VkDevice device_{ VK_NULL_HANDLE };

    // Surface
    VkSurfaceKHR surface_{ VK_NULL_HANDLE };
    VkSurfaceCapabilitiesKHR surfaceCapabilities_{};

    // Render pass
    VkRenderPass renderPass_{ VK_NULL_HANDLE };

    // Graphics queue resources.
    uint32_t graphicsFamilyQueueId_{};
    uint32_t graphicsQueueId_{};
    VkQueue graphicsQueue_{};

    // uint32_t framesCount_{ CONFIG_FRAMES_COUNT };
};

}  // namespace tire::vk
