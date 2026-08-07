
#include <cstddef>
#include <utility>
#include <cstring>
#include <format>

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

#include "vma/vk_mem_alloc.h"

#include "vertex_buffer.h"
#include "r/context/context.h"
#include "log/log.h"

namespace tire {

VertexBuffer::VertexBuffer( VertexBuffer &&other ) noexcept {
    size_ = std::exchange( other.size_, 0 );

    deviceBuffer_ = std::exchange( other.deviceBuffer_, VK_NULL_HANDLE );
    deviceAllocation_ = std::exchange( other.deviceAllocation_, VK_NULL_HANDLE );

    stagingBuffer_ = std::exchange( other.stagingBuffer_, VK_NULL_HANDLE );
    stagingAllocation_ = std::exchange( other.stagingAllocation_, VK_NULL_HANDLE );
}

VertexBuffer::VertexBuffer( size_t size )
    : size_{ size } {
    initStagingBuffer( size );
    initDeviceBuffer( size );
}

auto VertexBuffer::operator=( VertexBuffer &&other ) noexcept -> VertexBuffer & {
    size_ = std::exchange( other.size_, 0 );

    deviceBuffer_ = std::exchange( other.deviceBuffer_, VK_NULL_HANDLE );
    deviceAllocation_ = std::exchange( other.deviceAllocation_, VK_NULL_HANDLE );

    stagingBuffer_ = std::exchange( other.stagingBuffer_, VK_NULL_HANDLE );
    stagingAllocation_ = std::exchange( other.stagingAllocation_, VK_NULL_HANDLE );

    return *this;
}

VertexBuffer::~VertexBuffer() {
    //
    clean();
};

auto VertexBuffer::deviceBuffer() const -> VkBuffer {
    //
    return deviceBuffer_;
}

auto VertexBuffer::stagingBuffer() const -> VkBuffer {
    //
    return stagingBuffer_;
}

auto VertexBuffer::memcpy( const void *data, size_t size, size_t offset ) const -> void {
    if ( size > size_ ) {
        log::warning()( "target memory chunk larger than allocated!" );
    }

    void *mappedPtr{};
    vmaMapMemory( Context::instance().allocator(), stagingAllocation_, &mappedPtr );

    char *offsettedPtr = static_cast<char *>( mappedPtr ) + offset;

    std::memcpy( offsettedPtr, data, size );

    vmaUnmapMemory( Context::instance().allocator(), stagingAllocation_ );
}

auto VertexBuffer::size() const -> size_t {
    //
    return size_;
}

auto VertexBuffer::clean() -> void {
    vmaDestroyBuffer( Context::instance().allocator(), deviceBuffer_, deviceAllocation_ );
    vmaDestroyBuffer( Context::instance().allocator(), stagingBuffer_, stagingAllocation_ );
}

auto VertexBuffer::initStagingBuffer( size_t size ) -> void {
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

    vmaCreateBuffer( Context::instance().allocator(), &stagingBufferInfo, &vmaallocInfo, &stagingBuffer_,
                     &stagingAllocation_, nullptr );
}

auto VertexBuffer::initDeviceBuffer( size_t size ) -> void {
    const auto bufCreateInfo = VkBufferCreateInfo{
        //
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size_,
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    };

    const auto allocCreateInfo = VmaAllocationCreateInfo{
        //
        .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = VMA_MEMORY_USAGE_GPU_ONLY,

    };

    vmaCreateBuffer( Context::instance().allocator(), &bufCreateInfo, &allocCreateInfo, &deviceBuffer_,
                     &deviceAllocation_, nullptr );
}

}  // namespace tire
