
#include <format>
#include <cstring>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#include "pipeline_ui.h"

#include "r/context/context.h"
#include "r/context/pipeline.h"

#include "log/log.h"

#include "algebra/vector2.h"
#include "algebra/vector3.h"
#include "algebra/vector4.h"

namespace tire {

using namespace algebra;

PipelineUi::PipelineUi() {
}

#define SHADER_ENTRY_POINT "main"

auto PipelineUi::buildPipeline( const Program &program ) -> void {
    // Add VERTEX stage
    if ( const auto module = program.get<ShaderStageType::VERTEX>(); module != VK_NULL_HANDLE ) {
        const VkPipelineShaderStageCreateInfo stage{ .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                                     .pNext = nullptr,
                                                     .flags = 0,
                                                     .stage = VK_SHADER_STAGE_VERTEX_BIT,
                                                     .module = module,
                                                     .pName = SHADER_ENTRY_POINT };
        shaderStages_.push_back( stage );
    }

    // Add FRAGMENT stage
    if ( const auto module = program.get<ShaderStageType::FRAGMENT>(); module != VK_NULL_HANDLE ) {
        const VkPipelineShaderStageCreateInfo stage{ .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                                     .pNext = nullptr,
                                                     .flags = 0,
                                                     .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                                                     .module = module,
                                                     .pName = SHADER_ENTRY_POINT };
        shaderStages_.push_back( stage );
    }

    // Add TESSEALTION_EVALUATION stage
    if ( const auto module = program.get<ShaderStageType::TESSELATION_EVAL>(); module != VK_NULL_HANDLE ) {
        const VkPipelineShaderStageCreateInfo stage{ .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                                     .pNext = nullptr,
                                                     .flags = 0,
                                                     .stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
                                                     .module = module,
                                                     .pName = "main" };
        shaderStages_.push_back( stage );
    }

    // Add TESSELATION CONTROL stage
    if ( const auto module = program.get<ShaderStageType::TESSELATION_CTRL>(); module != VK_NULL_HANDLE ) {
        const VkPipelineShaderStageCreateInfo stage{ .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                                     .pNext = nullptr,
                                                     .flags = 0,
                                                     .stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
                                                     .module = module,
                                                     .pName = SHADER_ENTRY_POINT };
        shaderStages_.push_back( stage );
    }

    // Add GEOMETRY stage
    if ( const auto module = program.get<ShaderStageType::GEOMETRY>(); module != VK_NULL_HANDLE ) {
        const VkPipelineShaderStageCreateInfo stage{ .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                                     .pNext = nullptr,
                                                     .flags = 0,
                                                     .stage = VK_SHADER_STAGE_GEOMETRY_BIT,
                                                     .module = module,
                                                     .pName = SHADER_ENTRY_POINT };
        shaderStages_.push_back( stage );
    }

    // Add MESH stage
    if ( const auto module = program.get<ShaderStageType::MESH>(); module != VK_NULL_HANDLE ) {
        const VkPipelineShaderStageCreateInfo stage{ .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                                     .pNext = nullptr,
                                                     .flags = 0,
                                                     .stage = VK_SHADER_STAGE_MESH_BIT_EXT,
                                                     .module = module,
                                                     .pName = SHADER_ENTRY_POINT };
        shaderStages_.push_back( stage );
    }

    // ===========================================================================
    // Init fixed stages
    // ===========================================================================

    // NOTE: https://docs.vulkan.org/guide/latest/vertex_input_data_processing.html

    const auto vericiesBindingDescription = VkVertexInputBindingDescription{
        .binding = 0,
        .stride = sizeof( vector3f ),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    const auto texcrdsBindingDescription = VkVertexInputBindingDescription{
        .binding = 1,
        .stride = sizeof( algebra::vector2f ),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    const auto vclrsBindingDescription = VkVertexInputBindingDescription{
        .binding = 2,
        .stride = sizeof( algebra::vector4f ),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    std::array<VkVertexInputBindingDescription, 3> bindingDescriptions{
        vericiesBindingDescription, texcrdsBindingDescription, vclrsBindingDescription };

    const auto verticiesInputAttrDesc = VkVertexInputAttributeDescription{
        .location = 0,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = 0,
    };

    const auto texcrdsInputAttrDesc = VkVertexInputAttributeDescription{
        .location = 1,
        .binding = 1,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = 0,
    };

    const auto vclrsInputAttrDesc = VkVertexInputAttributeDescription{
        .location = 2,
        .binding = 2,
        .format = VK_FORMAT_R32G32B32A32_SFLOAT,
        .offset = 0,
    };

    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{ verticiesInputAttrDesc,
                                                                            texcrdsInputAttrDesc, vclrsInputAttrDesc };

    const VkPipelineVertexInputStateCreateInfo vertexInput{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = bindingDescriptions.size(),
        .pVertexBindingDescriptions = bindingDescriptions.data(),
        .vertexAttributeDescriptionCount = static_cast<uint32_t>( attributeDescriptions.size() ),
        .pVertexAttributeDescriptions = attributeDescriptions.data(),
    };

    const VkPipelineInputAssemblyStateCreateInfo inputAssembly{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    const VkPipelineRasterizationStateCreateInfo rasterizer{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthClampEnable = VK_TRUE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0f,
    };

    const VkPipelineMultisampleStateCreateInfo multisampling{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    const VkPipelineColorBlendAttachmentState colorBlendAttachment{
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    const VkPipelineColorBlendStateCreateInfo colorBlending{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment,
        .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f } };

    const std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH,
    };

    const VkPipelineDynamicStateCreateInfo dynamicState{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = static_cast<uint32_t>( dynamicStates.size() ),
        .pDynamicStates = dynamicStates.data(),
    };

    const auto viewportState = VkPipelineViewportStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,
    };

    const auto depthStencil = VkPipelineDepthStencilStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_FALSE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
        .front = {},
        .back = {},
        .minDepthBounds = 0.0f,
        .maxDepthBounds = 1.0f,
    };

    // =============================================================================

    // Setup descriptor sets.
    // Texture sampler descriptor.
    const auto textureBind = VkDescriptorSetLayoutBinding{
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
    };

    const auto textureDescSetCreateInfo = VkDescriptorSetLayoutCreateInfo{
        //
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .bindingCount = 1,
        .pBindings = &textureBind,
    };

    vkCreateDescriptorSetLayout( Context::instance().device(), &textureDescSetCreateInfo, nullptr,
                                 &textureDescSetLayout_ );

    std::array<VkDescriptorSetLayout, 1> descSetLayouts{ textureDescSetLayout_ };

    // Setup push constants.

    const auto viewInfoPc = VkPushConstantRange{
        //
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .offset = 0,
        .size = sizeof( float ) * 4,
    };

    const auto textureFlagPc = VkPushConstantRange{
        //
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .offset = sizeof( float ) * 4,
        .size = sizeof( uint32_t ) * 4,
    };

    std::array<VkPushConstantRange, 2> constants{ viewInfoPc, textureFlagPc };

    const auto pipelineLayoutInfo = VkPipelineLayoutCreateInfo{
        //
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = descSetLayouts.size(),
        .pSetLayouts = descSetLayouts.data(),
        .pushConstantRangeCount = constants.size(),
        .pPushConstantRanges = constants.data(),
    };

    if ( const auto err =
             vkCreatePipelineLayout( Context::instance().device(), &pipelineLayoutInfo, nullptr, &layout_ );
         err != VK_SUCCESS ) {
        log::fatal()( "failed to create pipeline layout with code {}!", string_VkResult( err ) );
    }

    // Init render pass.
    renderPass_ = Context::instance().renderPass();

    // Create pipeline
    const auto pipelineInfo = VkGraphicsPipelineCreateInfo{
        //
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = static_cast<uint32_t>( shaderStages_.size() ),
        .pStages = shaderStages_.data(),
        .pVertexInputState = &vertexInput,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = &depthStencil,
        .pColorBlendState = &colorBlending,
        .pDynamicState = &dynamicState,
        .layout = layout_,
        .renderPass = renderPass_,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    if ( const auto err = vkCreateGraphicsPipelines( Context::instance().device(), VK_NULL_HANDLE, 1, &pipelineInfo,
                                                     nullptr, &pipeline_ );
         err != VK_SUCCESS ) {
        log::fatal()( "failed to create graphics pipeline with code {}!", string_VkResult( err ) );
    }
}

auto PipelineUi::pipelineSescSetsLayout() const -> std::array<VkDescriptorSetLayout, 1> {
    //
    return { textureDescSetLayout_ };
}

}  // namespace tire