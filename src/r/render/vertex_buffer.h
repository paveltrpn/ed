#pragma once

#include <cstddef>
#include <utility>
#include <cstring>
#include <format>

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

#include "vma/vk_mem_alloc.h"

#include "r/context/context.h"
#include "log/log.h"

namespace tire {

struct VertexBuffer final {
    using value_type = float;

    VertexBuffer() = delete;
    VertexBuffer( const VertexBuffer &other ) = delete;

    VertexBuffer( VertexBuffer &&other ) noexcept;

    VertexBuffer( size_t size );

    auto operator=( const VertexBuffer &other ) -> VertexBuffer & = delete;

    auto operator=( VertexBuffer &&other ) noexcept -> VertexBuffer &;

    ~VertexBuffer();

    [[nodiscard]] auto deviceBuffer() const -> VkBuffer;

    [[nodiscard]] auto stagingBuffer() const -> VkBuffer;

    auto memcpy( const void *data, size_t size, size_t offset = 0 ) const -> void;

    [[nodiscard]] auto size() const -> size_t;

    auto clean() -> void;

private:
    auto initStagingBuffer( size_t size ) -> void;

    auto initDeviceBuffer( size_t size ) -> void;

private:
    size_t size_{};

    VkBuffer deviceBuffer_{ VK_NULL_HANDLE };
    VmaAllocation deviceAllocation_{ VK_NULL_HANDLE };

    VkBuffer stagingBuffer_{ VK_NULL_HANDLE };
    VmaAllocation stagingAllocation_{ VK_NULL_HANDLE };
};

}  // namespace tire
