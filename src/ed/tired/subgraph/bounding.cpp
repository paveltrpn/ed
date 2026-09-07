
#include "program/program.h"
#include "bounding.h"

namespace tire {

// ======================================================================================
// ==================== Bounding ========================================================
// ======================================================================================

Bounding::Bounding( vsg::Viewer* viewer, QObject* parent )
    : QObject{ parent }
    , _node{ new BoundingSubgraph{ viewer } } {
    //
    _node->initPipeline();
    _node->initDrawCommand();
}

auto Bounding::node() const -> vsg::ref_ptr<BoundingSubgraph> {
    return _node;
}

void Bounding::setTransformMat( vsg::mat4 value ) {
    _node->_transformMat = value * _node->_initialScale;
    _node->updateTransformMatUniform();
}

// ======================================================================================
// ==================== BoundingSubgraph ================================================
// ======================================================================================

BoundingSubgraph::BoundingSubgraph( vsg::Viewer* viewer )
    : Subgraph{ viewer } {
    //
}

auto BoundingSubgraph::initPipeline() -> void {
    auto bboxcornerSource = TextProgramSource{ "bboxcorner" };
    auto vSource = bboxcornerSource.stageSource( ShaderStageType::VERTEX );
    auto fSource = bboxcornerSource.stageSource( ShaderStageType::FRAGMENT );
    auto vertexShader = vsg::ShaderStage::create( VK_SHADER_STAGE_VERTEX_BIT, "main", vSource );
    auto fragmentShader = vsg::ShaderStage::create( VK_SHADER_STAGE_FRAGMENT_BIT, "main", fSource );

    if ( !vertexShader || !fragmentShader ) {
        std::println( "Could not create shaders." );
        std::terminate();
    }

    // set up graphics pipeline
    auto descriptorBindings = vsg::DescriptorSetLayoutBindings{
        { /* binding */ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, /* count */ 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr } };
    auto descriptorSetLayout = vsg::DescriptorSetLayout::create( descriptorBindings );

    _transformMatUniform = vsg::mat4Value::create( _transformMat * _initialScale );
    _transformMatUniform->properties.dataVariance = vsg::DYNAMIC_DATA;

    auto transformMatBufUniformDescriptor =
        vsg::DescriptorBuffer::create( _transformMatUniform, /* dstBinding */ 0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER );

    // projection, view, and model matrices, actual push constant calls automatically provided by the VSG's RecordTraversal
    auto pushConstantRanges = vsg::PushConstantRanges{ { VK_SHADER_STAGE_VERTEX_BIT, 0, 128 } };

    auto inputAssemblyState = vsg::InputAssemblyState::create();
    inputAssemblyState->topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

    auto rasterizationState = vsg::RasterizationState::create();
    rasterizationState->lineWidth = 4.0f;

    auto pipelineStates = vsg::GraphicsPipelineStates{ vsg::VertexInputState::create(),
                                                       inputAssemblyState,
                                                       rasterizationState,
                                                       vsg::MultisampleState::create(),
                                                       vsg::ColorBlendState::create(),
                                                       vsg::DepthStencilState::create() };

    auto pipelineLayout =
        vsg::PipelineLayout::create( vsg::DescriptorSetLayouts{ descriptorSetLayout }, pushConstantRanges );
    auto graphicsPipeline = vsg::GraphicsPipeline::create(
        pipelineLayout, vsg::ShaderStages{ vertexShader, fragmentShader }, pipelineStates );
    auto bindGraphicsPipeline = vsg::BindGraphicsPipeline::create( graphicsPipeline );

    auto descriptorSet =
        vsg::DescriptorSet::create( descriptorSetLayout, vsg::Descriptors{ transformMatBufUniformDescriptor } );

    auto bindDescriptorSet = vsg::BindDescriptorSet::create( VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
                                                             /* in_firstSet */ 0, descriptorSet );

    _stateGroup->add( bindGraphicsPipeline );
    _stateGroup->add( bindDescriptorSet );
};

auto BoundingSubgraph::initDrawCommand() -> void {
    auto drawCommands = vsg::Commands::create();

    const auto drawCmd = vsg::Draw::create( 48, 1, 0, 0 );
    drawCommands->addChild( drawCmd );

    _stateGroup->addChild( drawCommands );
}

auto BoundingSubgraph::updateTransformMatUniform() -> void {
    _transformMatUniform->set( _transformMat );

    _transformMatUniform->dirty();
}

}  // namespace tire
