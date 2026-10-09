
#pragma once

#include <vsg/state/GraphicsPipeline.h>
#include <vsg/state/PipelineLayout.h>
#include <vsg/state/ShaderStage.h>
#include <vsg/state/StateCommand.h>

namespace vsg {

/// GraphicsPipeline encapsulates graphics VkPipeline and the VkGraphicsPipelineCreateInfo settings used to set it up.
class VSG_DECLSPEC DynamicRenderGraphicsPipeline : public Inherit<GraphicsPipeline, DynamicRenderGraphicsPipeline> {
public:
    DynamicRenderGraphicsPipeline();

    DynamicRenderGraphicsPipeline( PipelineLayout* pipelineLayout, const ShaderStages& shaderStages,
                                   const GraphicsPipelineStates& pipelineStates, uint32_t subpass = 0 );

    int compare( const Object& rhs_object ) const override;

    void read( Input& input ) override;
    void write( Output& output ) const override;

    // compile the Vulkan object, context parameter used for Device
    void compile( Context& context );

protected:
    ~DynamicRenderGraphicsPipeline() override;

    struct Implementation : public Inherit<Object, Implementation> {
        Implementation( Context& context, Device* device, const PipelineLayout* pipelineLayout,
                        const ShaderStages& shaderStages, const GraphicsPipelineStates& pipelineStates,
                        uint32_t subpass );

        ~Implementation() override;

        GraphicsPipelineStates _pipelineStates;
        VkPipeline _pipeline;

        ref_ptr<Device> _device;
    };

    std::vector<ref_ptr<Implementation>> _implementation;
};
VSG_type_name( vsg::DynamicRenderGraphicsPipeline );

/// BindGraphicsPipeline state command encapsulates the vkCmdBindPipeline call for a GraphicsPipeline.
class VSG_DECLSPEC BindDynamicRenderGraphicsPipeline
    : public Inherit<BindGraphicsPipeline, BindDynamicRenderGraphicsPipeline> {
public:
    explicit BindDynamicRenderGraphicsPipeline( ref_ptr<DynamicRenderGraphicsPipeline> in_pipeline = {} );

    /// pipeline to pass in the vkCmdBindPipeline call;
    ref_ptr<DynamicRenderGraphicsPipeline> pipeline;

    int compare( const Object& rhs_object ) const override;

    void read( Input& input ) override;
    void write( Output& output ) const override;

    void record( CommandBuffer& commandBuffer ) const override;

    // compile the Vulkan object, context parameter used for Device
    void compile( Context& context ) override;

public:
    ~BindDynamicRenderGraphicsPipeline() override;
};
VSG_type_name( vsg::BindDynamicRenderGraphicsPipeline );

}  // namespace vsg