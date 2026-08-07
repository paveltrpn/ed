
#pragma once

#include <vulkan/vulkan.h>

namespace tire {

struct VKInstance;
struct VKDevice;

struct ContextPools final {
    ContextPools( const VKInstance *instance, const VKDevice *device );

    ContextPools( const ContextPools &other ) = delete;
    ContextPools( ContextPools &&other ) = delete;

    auto operator=( const ContextPools &other ) -> ContextPools & = delete;
    auto operator=( ContextPools &&other ) -> ContextPools & = delete;

    ~ContextPools();

    [[nodiscard]] auto commandPool() const -> VkCommandPool;
    [[nodiscard]] auto descriptorPool() const -> VkDescriptorPool;

private:
    auto createCommandPool() -> void;
    auto createDescriptorPool() -> void;

private:
    const VKInstance *_instance{};
    const VKDevice *_device{};

    VkCommandPool commandPool_{ VK_NULL_HANDLE };
    VkDescriptorPool descriptorPool_{ VK_NULL_HANDLE };
};

}  // namespace tire
