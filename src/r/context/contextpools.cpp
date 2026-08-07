
#include <vulkan/vk_enum_string_helper.h>

#include "contextpools.h"

#include "vkinstance.h"
#include "device.h"

#include "log/log.h"

namespace tire {

ContextPools::ContextPools( const VKInstance *instance, const VKDevice *device )
    : _instance{ instance }
    , _device{ device } {
    createCommandPool();
    createDescriptorPool();
}

ContextPools::~ContextPools() {
    vkDestroyDescriptorPool( _device->get(), descriptorPool_, nullptr );
    vkDestroyCommandPool( _device->get(), commandPool_, nullptr );
}

auto ContextPools::createCommandPool() -> void {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = _device->graphicsFamilyQueueId();

    if ( const auto err = vkCreateCommandPool( _device->get(), &poolInfo, nullptr, &commandPool_ );
         err != VK_SUCCESS ) {
        log::fatal()( "failed to create command pool woth code {}!", string_VkResult( err ) );
    } else {
        log::debug()( "command pool created!" );
    }
}

auto ContextPools::createDescriptorPool() -> void {
    std::vector<VkDescriptorPoolSize> sizes = {
        //
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10 },
    };

    const auto poolInfo = VkDescriptorPoolCreateInfo{
        //
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = 0,
        .maxSets = 10,
        .poolSizeCount = static_cast<uint32_t>( sizes.size() ),
        .pPoolSizes = sizes.data(),
    };

    {
        const auto err = vkCreateDescriptorPool( _device->get(), &poolInfo, nullptr, &descriptorPool_ );
        if ( err != VK_SUCCESS ) {
            log::fatal()( "failed to create descriptor pool {}!", string_VkResult( err ) );
        } else {
            log::debug()( "descriptors pool created!" );
        }
    }
}

auto ContextPools::commandPool() const -> VkCommandPool {
    return commandPool_;
}

auto ContextPools::descriptorPool() const -> VkDescriptorPool {
    return descriptorPool_;
}

}  // namespace tire
