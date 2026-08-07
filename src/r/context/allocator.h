
#pragma once

#include <vulkan/vulkan.h>

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include "vma/vk_mem_alloc.h"

namespace tire {

struct VKInstance;
struct VKDevice;

struct VMAllocator final {
    VMAllocator( const VKInstance *instnce, const VKDevice *device );

    VMAllocator( const VMAllocator &other ) = delete;
    VMAllocator( VMAllocator &&other ) = delete;

    auto operator=( const VMAllocator &other ) -> VMAllocator & = delete;
    auto operator=( VMAllocator &&other ) -> VMAllocator & = delete;

    ~VMAllocator();

    [[nodiscard]] auto get() const -> VmaAllocator;

private:
    VmaAllocator allocator_{};
};

}  // namespace tire