
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
    _node->updateBoxUniformValue();
}

void Testbox::setBoxAxis( QVector3D value ) {
    _node->_boxAxis = vsg::vec3{ value.x(), value.y(), value.z() };
    _node->updateBoxUniformValue();
}

void Testbox::setBoxAngl( float value ) {
    _node->_boxAngl = value;
    _node->updateBoxUniformValue();
}

void Testbox::setBoxSize( float value ) {
    _node->_boxSize = value;
    _node->updateBoxUniformValue();
}

void Testbox::setBoxColor( QVector3D value ) {
    _node->_boxColor = vsg::vec3{ value.x(), value.y(), value.z() };
    _node->updateBoxUniformValue();
}

void Testbox::setLightOrigin( QVector3D value ) {
    _node->_lightOrigin = vsg::vec3{ value.x(), value.y(), value.z() };
    _node->updateLightBufUniformValue();
}

void Testbox::setLightColor( QVector3D value ) {
    _node->_lightColor = vsg::vec3{ value.x(), value.y(), value.z() };
    _node->updateLightBufUniformValue();
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

    // Set up graphics pipeline.
    const auto descriptorBindings = vsg::DescriptorSetLayoutBindings{
        { /* binding */ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, /* count */ 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr },
        { /* binding */ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, /* count */ 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr } };
    const auto descriptorSetLayout = vsg::DescriptorSetLayout::create( descriptorBindings );

    _boxUniformValue =
        vsg::floatArray::create( { _boxOrigin.x, _boxOrigin.y, _boxOrigin.z, _boxSize, _boxAxis.x, _boxAxis.y,
                                   _boxAxis.z, _boxAngl, _boxColor.x, _boxColor.y, _boxColor.z, 1.0f } );
    _boxUniformValue->properties.dataVariance = vsg::DYNAMIC_DATA;

    const auto boxUniformDescriptor =
        vsg::DescriptorBuffer::create( _boxUniformValue, /* dstBinding */ 0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER );

    _lightUniformValue = vsg::floatArray::create( {
        _lightOrigin.x,
        _lightOrigin.y,
        _lightOrigin.z,
        0.0f,
        _lightColor.x,
        _lightColor.y,
        _lightColor.z,
        0.0f,
    } );
    _lightUniformValue->properties.dataVariance = vsg::DYNAMIC_DATA;

    const auto lightUniformDescriptor =
        vsg::DescriptorBuffer::create( _lightUniformValue, /* dstBinding */ 1, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER );

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

    auto descriptorSet = vsg::DescriptorSet::create( descriptorSetLayout,
                                                     vsg::Descriptors{ boxUniformDescriptor, lightUniformDescriptor } );

    auto bindDescriptorSet = vsg::BindDescriptorSet::create( VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
                                                             /* in_firstSet */ 0, descriptorSet );

    _stateGroup->add( bindGraphicsPipeline );
    _stateGroup->add( bindDescriptorSet );
}

auto TestboxSubgraph::initDrawCommand() -> void {
    // Add draw command.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::Draw::create( 36, 1, 0, 0 ) );

    auto tr = vsg::MatrixTransform::create();
    tr->addChild( drawCommands );

    _stateGroup->addChild( tr );
}

auto TestboxSubgraph::updateBoxUniformValue() -> void {
    ( *_boxUniformValue )[0] = _boxOrigin.x;
    ( *_boxUniformValue )[1] = _boxOrigin.y;
    ( *_boxUniformValue )[2] = _boxOrigin.z;
    ( *_boxUniformValue )[3] = _boxSize;
    ( *_boxUniformValue )[4] = _boxAxis.x;
    ( *_boxUniformValue )[5] = _boxAxis.y;
    ( *_boxUniformValue )[6] = _boxAxis.z;
    ( *_boxUniformValue )[7] = _boxAngl;
    ( *_boxUniformValue )[8] = _boxColor.r;
    ( *_boxUniformValue )[9] = _boxColor.g;
    ( *_boxUniformValue )[10] = _boxColor.b;
    ( *_boxUniformValue )[11] = 1.0f;

    _boxUniformValue->dirty();
}

auto TestboxSubgraph::updateLightBufUniformValue() -> void {
    ( *_lightUniformValue )[0] = _lightOrigin.x;
    ( *_lightUniformValue )[1] = _lightOrigin.y;
    ( *_lightUniformValue )[2] = _lightOrigin.z;
    ( *_lightUniformValue )[3] = 0.0f;
    ( *_lightUniformValue )[4] = _lightColor.x;
    ( *_lightUniformValue )[5] = _lightColor.y;
    ( *_lightUniformValue )[6] = _lightColor.z;
    ( *_lightUniformValue )[7] = 0.0f;

    _lightUniformValue->dirty();
}

}  // namespace tire
