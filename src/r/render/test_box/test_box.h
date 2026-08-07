#pragma once

#include <memory>

#include <vulkan/vulkan.h>
#include "vulkan/vulkan_core.h"
#include <vulkan/vk_enum_string_helper.h>

#include "vma/vk_mem_alloc.h"

#include "algebra/vector3.h"
#include "algebra/matrix4.h"

#include "test_box_shader.h"
#include "pipeline_test_box.h"

namespace tire {

struct TestBox final {
    TestBox();

    auto draw( const VkCommandBuffer cb, float duration ) -> void;

    auto setPosition( float x, float y, float z ) -> void;
    auto setPosition( algebra::vector3f p ) -> void;

    auto clean() -> void;

private:
    std::unique_ptr<Pipeline> pipeline_{};

    algebra::vector3f position_{ 0.0, 0.0, 0.0 };

    float angle_{};
};

}  // namespace tire
