
#pragma once

#include <vector>
#include <optional>

#include <vulkan/vulkan.h>

namespace tire {

struct VKInstance;

struct VKDevice final {
    VKDevice( const VKInstance *instance );

    VKDevice( const VKDevice &other ) = delete;
    VKDevice( VKDevice &&other ) = delete;

    auto operator=( const VKDevice &other ) -> VKDevice & = delete;
    auto operator=( VKDevice &&other ) -> VKDevice & = delete;

    ~VKDevice();

    [[nodiscard]] auto get() const -> VkDevice;
    [[nodiscard]] auto presentQueue() const -> VkQueue;
    [[nodiscard]] auto graphicsQueue() const -> VkQueue;
    [[nodiscard]] auto physicalDevice() const -> VkPhysicalDevice;
    [[nodiscard]] auto graphicsFamilyQueueId() const -> uint32_t;
    [[nodiscard]] auto presentSupportQueueId() const -> uint32_t;

private:
    struct PhysicalDevice final {
        VkPhysicalDevice device{ VK_NULL_HANDLE };
        VkPhysicalDeviceProperties properties{};
        VkPhysicalDeviceFeatures features{};
        std::vector<VkExtensionProperties> extensions{};
        std::vector<VkQueueFamilyProperties> queueFamilyProperties{};
    };

private:
    auto collectPhysicalDevices() -> void;
    auto pickDevice( const std::vector<PhysicalDevice> &physDevList ) -> std::optional<int>;
    auto createDevice() -> void;

private:
    const VKInstance *instance_{};

    std::vector<PhysicalDevice> physicalDevices_{};
    uint32_t pickedPhysicalDeviceId_{ UINT32_MAX };
    uint32_t presentSupportQueueId_{ UINT32_MAX };
    VkQueue presentQueue_{ VK_NULL_HANDLE };
    VkPhysicalDevice physicalDevice_{};
    // The logical device itself.
    VkDevice device_{ VK_NULL_HANDLE };

    VkQueue graphicsQueue_{ VK_NULL_HANDLE };
    uint32_t graphicsFamilyQueueId_{ UINT32_MAX };
};

}  // namespace tire