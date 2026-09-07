
#include <print>

#include "program/program.h"
#include "testbox.h"

namespace tire {

// ======================================================================================
// ==================== Testbox =========================================================
// ======================================================================================

Testbox::Testbox( vsg::Viewer* viewer, QObject* parent )
    : QObject{ parent }
    , _node{ new TestboxSubgraph{ viewer } } {
    //
    _node->initPipeline();
    _node->initDrawCommand();
}

auto Testbox::node() const -> vsg::ref_ptr<TestboxSubgraph> {
    return _node;
}

void Testbox::setOrigin( QVector3D value ) {
    _node->_origin = vsg::vec3{ value.x(), value.y(), value.z() };
    originChanged();
}

void Testbox::setAxis( QVector3D value ) {
    _node->_axis = vsg::vec3{ value.x(), value.y(), value.z() };
    axisChanged();
}

void Testbox::setAngl( float value ) {
    _node->_angl = value;
    anglChanged();
}

void Testbox::setSize( float value ) {
    _node->_size = value;
    sizeChanged();
}

void Testbox::setColor( QVector3D value ) {
    _node->_color = vsg::vec3{ value.x(), value.y(), value.z() };
    colorChanged();
}

QVector3D Testbox::origin() {
    return QVector3D{ _node->_origin.x, _node->_origin.y, _node->_origin.z };
}

QVector3D Testbox::axis() {
    return QVector3D{ _node->_axis.x, _node->_axis.y, _node->_axis.z };
}

float Testbox::angl() {
    return _node->_angl;
}

float Testbox::size() {
    return _node->_size;
}

QVector3D Testbox::color() {
    return QVector3D{ _node->_color.x, _node->_color.y, _node->_color.z };
}

// ======================================================================================
// ==================== TestboxSubgraph =================================================
// ======================================================================================

TestboxSubgraph::TestboxSubgraph( vsg::Viewer* viewer )
    : Subgraph{ viewer } {
}

auto TestboxSubgraph::initPipeline() -> void {
    auto testboxSource = TextProgramSource{ "testbox" };
    auto vSource = testboxSource.stageSource( ShaderStageType::VERTEX );
    auto fSource = testboxSource.stageSource( ShaderStageType::FRAGMENT );
    auto vertexShader = vsg::ShaderStage::create( VK_SHADER_STAGE_VERTEX_BIT, "main", vSource );
    auto fragmentShader = vsg::ShaderStage::create( VK_SHADER_STAGE_FRAGMENT_BIT, "main", fSource );

    if ( !vertexShader || !fragmentShader ) {
        std::println( "Could not create shaders." );
        std::terminate();
    }

    // set up graphics pipeline
    vsg::DescriptorSetLayoutBindings descriptorBindings{
        { 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT,
          nullptr }  // { binding, descriptorType, descriptorCount, stageFlags, pImmutableSamplers}
    };

    auto descriptorSetLayout = vsg::DescriptorSetLayout::create( descriptorBindings );

    vsg::PushConstantRanges pushConstantRanges{
        { VK_SHADER_STAGE_VERTEX_BIT, 0,
          128 }  // projection, view, and model matrices, actual push constant calls automatically provided by the VSG's RecordTraversal
    };

    vsg::VertexInputState::Bindings vertexBindingsDescriptions{};

    vsg::VertexInputState::Attributes vertexAttributeDescriptions{};

    vsg::GraphicsPipelineStates pipelineStates{
        vsg::VertexInputState::create( vertexBindingsDescriptions, vertexAttributeDescriptions ),
        vsg::InputAssemblyState::create(),
        vsg::RasterizationState::create(),
        vsg::MultisampleState::create(),
        vsg::ColorBlendState::create(),
        vsg::DepthStencilState::create() };

    auto pipelineLayout =
        vsg::PipelineLayout::create( vsg::DescriptorSetLayouts{ descriptorSetLayout }, pushConstantRanges );
    auto graphicsPipeline = vsg::GraphicsPipeline::create(
        pipelineLayout, vsg::ShaderStages{ vertexShader, fragmentShader }, pipelineStates );
    auto bindGraphicsPipeline = vsg::BindGraphicsPipeline::create( graphicsPipeline );

    _stateGroup->add( bindGraphicsPipeline );
}

auto TestboxSubgraph::initDrawCommand() -> void {
    // Add draw command.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::Draw::create( 36, 1, 0, 0 ) );

    auto tr = vsg::MatrixTransform::create();
    tr->addChild( drawCommands );

    _stateGroup->addChild( tr );
}

}  // namespace tire
