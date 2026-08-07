#pragma once

#include <vector>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#include "context.h"
#include "program.h"

namespace tire {

struct Pipeline {
    Pipeline();

    Pipeline( const Pipeline &other ) = delete;
    Pipeline( Pipeline &&other ) = delete;
    auto operator=( const Pipeline &other ) -> Pipeline & = delete;
    auto operator=( Pipeline &&other ) -> Pipeline & = delete;

    virtual ~Pipeline();

    [[nodiscard]] auto pipeline() const -> VkPipeline;

    [[nodiscard]] auto layout() const -> VkPipelineLayout;

    [[nodiscard]] auto renderpass() const -> VkRenderPass;

    virtual auto buildPipeline( const Program &program ) -> void = 0;

protected:
    VkPipeline pipeline_{ VK_NULL_HANDLE };
    VkPipelineLayout layout_{ VK_NULL_HANDLE };
    VkRenderPass renderPass_{ VK_NULL_HANDLE };

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages_{};
};

}  // namespace tire
