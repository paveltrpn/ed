
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#include "pipeline.h"
#include "context.h"
#include "program.h"

namespace tire {

Pipeline::Pipeline() {
}

Pipeline::~Pipeline() {
    vkDestroyRenderPass( Context::instance().device(), renderPass_, nullptr );
    vkDestroyPipelineLayout( Context::instance().device(), layout_, nullptr );
    vkDestroyPipeline( Context::instance().device(), pipeline_, nullptr );
}

auto Pipeline::pipeline() const -> VkPipeline {
    //
    return pipeline_;
}

auto Pipeline::layout() const -> VkPipelineLayout {
    //
    return layout_;
};

auto Pipeline::renderpass() const -> VkRenderPass {
    //
    return renderPass_;
}

}  // namespace tire
