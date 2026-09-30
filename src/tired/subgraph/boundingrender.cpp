
#include <print>

#include "boundingrender.h"
#include "program/program.h"

namespace tire {

// ======================================================================================
// ==================== BoundingRender ==================================================
// ======================================================================================

BoundingRender::BoundingRender( vsg::observer_ptr<vsg::Viewer> viewer )
    : Subgraph{ viewer }
    , _stateGroup{ vsg::StateGroup::create() }
    , _killSwitch{ vsg::Switch::create() }
    , _boundingsGroup{ vsg::Group::create() } {
    //
    this->addChild( _killSwitch );

    _killSwitch->addChild( vsg::MASK_ALL, _stateGroup );

    _stateGroup->addChild( _boundingsGroup );

    initPipeline();
}

auto BoundingRender::initPipeline() -> void {
    auto bboxcornerSource = TextProgramSource{ "bboxcorner" };
    auto vSource = bboxcornerSource.stageSource( ShaderStageType::VERTEX );
    auto fSource = bboxcornerSource.stageSource( ShaderStageType::FRAGMENT );
    auto vertexShader = vsg::ShaderStage::create( VK_SHADER_STAGE_VERTEX_BIT, "main", vSource );
    auto fragmentShader = vsg::ShaderStage::create( VK_SHADER_STAGE_FRAGMENT_BIT, "main", fSource );

    if ( !vertexShader || !fragmentShader ) {
        std::println( "Could not create shaders." );
        std::terminate();
    }

    auto descriptorBindings = vsg::DescriptorSetLayoutBindings{};
    auto descriptorSetLayout = vsg::DescriptorSetLayout::create( descriptorBindings );

    // Push constants - projection + modelview + bounding box transform.
    auto pushConstantRanges = vsg::PushConstantRanges{ { VK_SHADER_STAGE_VERTEX_BIT, 0, 64 + 64 + 64 } };

    auto inputAssemblyState = vsg::InputAssemblyState::create();
    inputAssemblyState->topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

    auto rasterizationState = vsg::RasterizationState::create();
    rasterizationState->lineWidth = 12.0f;

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

    auto descriptorSet = vsg::DescriptorSet::create( descriptorSetLayout, vsg::Descriptors{} );

    auto bindDescriptorSet = vsg::BindDescriptorSet::create( VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
                                                             /* in_firstSet */ 0, descriptorSet );

    _stateGroup->add( bindGraphicsPipeline );
    _stateGroup->add( bindDescriptorSet );
};

auto BoundingRender::stateGroups() const -> std::vector<vsg::ref_ptr<vsg::StateGroup>> {
    return { _stateGroup };
}

auto BoundingRender::attach( vsg::ref_ptr<BoundingDraw> bounding ) -> void {
    _boundingsGroup->addChild( bounding );
}

}  // namespace tire