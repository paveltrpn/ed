
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

void Testbox::setBoxOrigin( QVector3D value ) {
    _node->_boxOrigin = vsg::vec3{ value.x(), value.y(), value.z() };
    boxOriginChanged();
}

void Testbox::setBoxAxis( QVector3D value ) {
    _node->_boxAxis = vsg::vec3{ value.x(), value.y(), value.z() };
    boxAxisChanged();
}

void Testbox::setBoxAngl( float value ) {
    _node->_boxAngl = value;
    boxAnglChanged();
}

void Testbox::setBoxSize( float value ) {
    _node->_boxSize = value;
    boxSizeChanged();
}

void Testbox::setBoxColor( QVector3D value ) {
    _node->_boxColor = vsg::vec3{ value.x(), value.y(), value.z() };
    boxColorChanged();
}

void Testbox::setLightOrigin( QVector3D value ) {
    _node->_lightOrigin = vsg::vec3{ value.x(), value.y(), value.z() };
}

void Testbox::setLightColor( QVector3D value ) {
    _node->_lightColor = vsg::vec3{ value.x(), value.y(), value.z() };
}

QVector3D Testbox::boxOrigin() {
    return QVector3D{ _node->_boxOrigin.x, _node->_boxOrigin.y, _node->_boxOrigin.z };
}

QVector3D Testbox::boxAxis() {
    return QVector3D{ _node->_boxAxis.x, _node->_boxAxis.y, _node->_boxAxis.z };
}

float Testbox::boxAngl() {
    return _node->_boxAngl;
}

float Testbox::boxSize() {
    return _node->_boxSize;
}

QVector3D Testbox::boxColor() {
    return QVector3D{ _node->_boxColor.x, _node->_boxColor.y, _node->_boxColor.z };
}

QVector3D Testbox::lightOrigin() {
    return QVector3D{ _node->_lightOrigin.x, _node->_lightOrigin.y, _node->_lightOrigin.z };
}

QVector3D Testbox::lightColor() {
    return QVector3D{ _node->_lightColor.x, _node->_lightColor.y, _node->_lightColor.z };
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
