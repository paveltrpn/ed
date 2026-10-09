
#pragma once

#include <vsg/state/GraphicsPipeline.h>
#include <vsg/state/PipelineLayout.h>
#include <vsg/state/ShaderStage.h>
#include <vsg/state/StateCommand.h>

namespace vsg {

/// GraphicsPipeline encapsulates graphics VkPipeline and the VkGraphicsPipelineCreateInfo settings used to set it up.
class VSG_DECLSPEC DynamicRenderGraphicsPipeline : public Inherit<Object, DynamicRenderGraphicsPipeline> {
public:
    DynamicRenderGraphicsPipeline();

    DynamicRenderGraphicsPipeline( PipelineLayout* pipelineLayout, const ShaderStages& shaderStages,
                                   const GraphicsPipelineStates& pipelineStates, uint32_t subpass = 0 );

    /// return the Vulkan Pipeline for specified viewID.
    VkPipeline vk( uint32_t viewID ) const { return _implementation[viewID]->_pipeline; }

    /// variant of vk(viewID) method that is slower but adds validation of the viewID parameter
    VkPipeline validated_vk( uint32_t viewID ) const {
        return ( viewID < _implementation.size() )
                   ? ( _implementation[viewID] ? _implementation[viewID]->_pipeline : 0 )
                   : 0;
    }

    /// VkGraphicsPipelineCreateInfo settings
    ShaderStages stages;
    GraphicsPipelineStates pipelineStates;
    ref_ptr<PipelineLayout> layout;
    uint32_t subpass;

    int compare( const Object& rhs_object ) const override;

    void read( Input& input ) override;
    void write( Output& output ) const override;

    // compile the Vulkan object, context parameter used for Device
    void compile( Context& context );

    // remove the local reference to the Vulkan implementation
    void release( uint32_t viewID ) {
        if ( viewID < static_cast<uint32_t>( _implementation.size() ) ) _implementation[viewID] = {};
    }
    void release() { _implementation.clear(); }

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

}  // namespace vsg