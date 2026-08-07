#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#include "r/context/context.h"
#include "r/context/pipeline.h"

namespace tire {

struct PipelineUi final : Pipeline {
    PipelineUi();

    auto buildPipeline( const Program &program ) -> void override;

    [[nodiscard]] auto pipelineSescSetsLayout() const -> std::array<VkDescriptorSetLayout, 1>;

private:
    VkDescriptorSetLayout textureDescSetLayout_{};
};

}  // namespace tire