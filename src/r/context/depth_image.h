#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#include "vma/vk_mem_alloc.h"

#include "context.h"
#include "log/log.h"

// import : commands;

namespace tire {

struct DepthImage final {
    DepthImage() = delete;
    DepthImage( const DepthImage &other ) = delete;
    DepthImage( DepthImage &&other ) = delete;

    DepthImage( const Context *context, uint32_t width, uint32_t height );

    auto operator=( const DepthImage &other ) -> DepthImage & = delete;
    auto operator=( DepthImage &&other ) -> DepthImage & = delete;

    [[nodiscard]] auto view() const -> VkImageView ;

    auto clean() -> void ;

    ~DepthImage() ;

private:
    auto initDeviceImage() -> void ;

    auto initImageView() -> void ;

    auto transitionImageLayout() -> void ;

    [[nodiscard]] auto findSupportedFormat( const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                            VkFormatFeatureFlags features ) const -> VkFormat;

private:
    const Context *context_{};

    uint32_t width_{};
    uint32_t height_{};
    VkFormat depthFormat_{};

    VkImage depthImage_{};
    VmaAllocation depthImageAllocation_{};
    VkImageView depthImageView_{};
};

}  // namespace tire
