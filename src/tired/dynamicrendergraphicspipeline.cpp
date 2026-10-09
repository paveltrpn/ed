
#include <vsg/core/Exception.h>
#include <vsg/core/compare.h>
#include <vsg/io/Logger.h>
#include <vsg/state/DynamicState.h>
#include <vsg/state/GraphicsPipeline.h>
#include <vsg/state/ViewportState.h>
#include <vsg/vk/Context.h>

#include "dynamicrendergraphicspipeline.h"

namespace vsg {
////////////////////////////////////////////////////////////////////////
//
// DynamicRenderGraphicsPipeline
//
DynamicRenderGraphicsPipeline::DynamicRenderGraphicsPipeline() {
}

DynamicRenderGraphicsPipeline::DynamicRenderGraphicsPipeline( PipelineLayout* in_pipelineLayout,
                                                              const ShaderStages& in_shaderStages,
                                                              const GraphicsPipelineStates& in_pipelineStates,
                                                              uint32_t in_subpass )
    : Inherit<GraphicsPipeline, DynamicRenderGraphicsPipeline>( in_pipelineLayout, in_shaderStages, in_pipelineStates,
                                                                in_subpass ) {
}

DynamicRenderGraphicsPipeline::~DynamicRenderGraphicsPipeline() {
}

int DynamicRenderGraphicsPipeline::compare( const Object& rhs_object ) const {
    int result = Object::compare( rhs_object );
    if ( result != 0 ) return result;

    const auto& rhs = static_cast<decltype( *this )>( rhs_object );

    if ( ( result = compare_pointer_container( stages, rhs.stages ) ) ) return result;
    if ( ( result = compare_pointer_container( pipelineStates, rhs.pipelineStates ) ) ) return result;
    if ( ( result = compare_pointer( layout, rhs.layout ) ) ) return result;
    return compare_value( subpass, rhs.subpass );
}

void DynamicRenderGraphicsPipeline::read( Input& input ) {
    Object::read( input );

    input.readObject( "layout", layout );
    input.readObjects( "stages", stages );
    input.readObjects( "pipelineStates", pipelineStates );
    input.read( "subpass", subpass );
}

void DynamicRenderGraphicsPipeline::write( Output& output ) const {
    Object::write( output );

    output.writeObject( "layout", layout );
    output.writeObjects( "stages", stages );
    output.writeObjects( "pipelineStates", pipelineStates );
    output.write( "subpass", subpass );
}

void DynamicRenderGraphicsPipeline::compile( Context& context ) {
    uint32_t viewID = context.viewID;
    if ( static_cast<uint32_t>( _implementation.size() ) < ( viewID + 1 ) ) {
        _implementation.resize( viewID + 1 );
    }

    if ( !_implementation[viewID] ) {
        GraphicsPipelineStates combined_pipelineStates;
        combined_pipelineStates.reserve( context.defaultPipelineStates.size() + pipelineStates.size() +
                                         context.overridePipelineStates.size() );
        mergeGraphicsPipelineStates( context.mask, combined_pipelineStates, context.defaultPipelineStates );
        mergeGraphicsPipelineStates( context.mask, combined_pipelineStates, pipelineStates );
        mergeGraphicsPipelineStates( context.mask, combined_pipelineStates, context.overridePipelineStates );

        for ( const auto& imp : _implementation ) {
            if ( imp && vsg::compare_pointer_container( imp->_pipelineStates, combined_pipelineStates ) == 0 ) {
                _implementation[viewID] = imp;
                return;
            }
        }

        // compile shaders if required
        bool requiresShaderCompiler = false;
        for ( const auto& shaderStage : stages ) {
            if ( shaderStage->module ) {
                if ( shaderStage->module->code.empty() && !( shaderStage->module->source.empty() ) ) {
                    requiresShaderCompiler = true;
                }
            }
        }

        if ( requiresShaderCompiler ) {
            auto shaderCompiler = context.getOrCreateShaderCompiler();
            if ( shaderCompiler ) {
                shaderCompiler->compile( stages );  // may need to map defines and paths in some fashion
            } else {
                fatal( "VulkanSceneGraph not compiled with GLSLang, unable to compile shaders." );
            }
        }

        // compile Vulkan objects
        layout->compile( context );

        for ( auto& shaderStage : stages ) {
            shaderStage->compile( context );
        }

        _implementation[viewID] = DynamicRenderGraphicsPipeline::Implementation::create(
            context, context.device, layout, stages, combined_pipelineStates, subpass );
    }
}

////////////////////////////////////////////////////////////////////////
//
// GraphicsPipeline::Implementation
//
DynamicRenderGraphicsPipeline::Implementation::Implementation( Context& context, Device* device,
                                                               const PipelineLayout* pipelineLayout,
                                                               const ShaderStages& shaderStages,
                                                               const GraphicsPipelineStates& pipelineStates,
                                                               uint32_t subpass )
    : _pipelineStates( pipelineStates )
    , _device( device ) {
    auto renderingInfo = VkPipelineRenderingCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
    renderingInfo.colorAttachmentCount = 1;

    // NOTE: HARDCODED.
    std::array<VkFormat, 1> formats = { VK_FORMAT_R8G8B8A8_SRGB };
    renderingInfo.pColorAttachmentFormats = formats.data(), renderingInfo.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;
    renderingInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;  // no stencil

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.layout = pipelineLayout->vk( device->deviceID );
    pipelineInfo.renderPass = VK_NULL_HANDLE;
    pipelineInfo.subpass = subpass;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.pNext = &renderingInfo;

    auto shaderStageCreateInfo =
        context.scratchMemory->allocate<VkPipelineShaderStageCreateInfo>( shaderStages.size() );
    uint32_t i = 0;
    for ( auto& shaderStage : shaderStages ) {
        // check if ShaderStage is appropriate to assign to stageInfo
        if ( ( context.mask & shaderStage->mask ) != 0 ) {
            shaderStageCreateInfo[i].flags = 0;
            shaderStageCreateInfo[i].pNext = nullptr;
            shaderStage->apply( context, shaderStageCreateInfo[i] );
            ++i;
        }
    }

    pipelineInfo.stageCount = i;
    pipelineInfo.pStages = shaderStageCreateInfo;

    for ( auto pipelineState : pipelineStates ) {
        pipelineState->apply( context, pipelineInfo );
    }

    VkResult result = vkCreateGraphicsPipelines( *device, VK_NULL_HANDLE, 1, &pipelineInfo,
                                                 _device->getAllocationCallbacks(), &_pipeline );

    context.scratchMemory->release();

    if ( result != VK_SUCCESS ) {
        throw Exception{ "Error: vsg::GraphicsPipeline failed to create VkPipeline.", result };
    }
}

DynamicRenderGraphicsPipeline::Implementation::~Implementation() {
    vkDestroyPipeline( *_device, _pipeline, _device->getAllocationCallbacks() );
}

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// BindDynamicRenderGraphicsPipeline
//
BindDynamicRenderGraphicsPipeline::BindDynamicRenderGraphicsPipeline(
    ref_ptr<DynamicRenderGraphicsPipeline> in_pipeline )
    : Inherit{ 0 }
    , pipeline( in_pipeline ) {
}

BindDynamicRenderGraphicsPipeline::~BindDynamicRenderGraphicsPipeline() {
}

int BindDynamicRenderGraphicsPipeline::compare( const Object& rhs_object ) const {
    int result = StateCommand::compare( rhs_object );
    if ( result != 0 ) return result;

    const auto& rhs = static_cast<decltype( *this )>( rhs_object );
    return compare_pointer( pipeline, rhs.pipeline );
}

void BindDynamicRenderGraphicsPipeline::read( Input& input ) {
    StateCommand::read( input );

    input.readObject( "pipeline", pipeline );
}

void BindDynamicRenderGraphicsPipeline::write( Output& output ) const {
    StateCommand::write( output );

    output.writeObject( "pipeline", pipeline );
}

void BindDynamicRenderGraphicsPipeline::record( CommandBuffer& commandBuffer ) const {
    auto foo = pipeline->vk( commandBuffer.viewID );

    vkCmdBindPipeline( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, foo );
    commandBuffer.setCurrentPipelineLayout( BindDynamicRenderGraphicsPipeline::pipeline->layout );
}

void BindDynamicRenderGraphicsPipeline::compile( Context& context ) {
    if ( pipeline ) pipeline->compile( context );
}

}  // namespace vsg
