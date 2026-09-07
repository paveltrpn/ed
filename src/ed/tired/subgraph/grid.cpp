
#include <print>

#include "program/program.h"
#include "grid.h"

namespace tire {

Grid::Grid( vsg::Viewer* viewer, QObject* parent )
    : QObject{ parent }
    , _node{ new GridSubgraph{ viewer } } {
    //
    _node->initPipeline();
    _node->initDrawCommand();
}

auto Grid::node() const -> vsg::ref_ptr<GridSubgraph> {
    return _node;
}

float Grid::gridSize() {
    return _node->_gridSize;
};

float Grid::lineThickness() {
    return _node->_lineThickness;
};

float Grid::maxRange() {
    return _node->_maxRange;
};

float Grid::zoomSensitivity() {
    return _node->_zoomSensitivity;
};

float Grid::majorDivisor() {
    return _node->_majorDivisor;
};

float Grid::gridScale() {
    return _node->_gridScale;
};

float Grid::gridZOffset() {
    return _node->_gridZOffset;
};

void Grid::setGridSize( float value ) {
    _node->_gridSize = value;
    _node->updateGridBufUniformValue();
}

void Grid::setLineThickness( float value ) {
    _node->_lineThickness = value;
    _node->updateGridBufUniformValue();
}

void Grid::setMaxRange( float value ) {
    _node->_maxRange = value;
    _node->updateGridBufUniformValue();
}

void Grid::setZoomSensitivity( float value ) {
    _node->_zoomSensitivity = value;
    _node->updateGridBufUniformValue();
}

void Grid::setColorMajor( float r, float g, float b ) {
    _node->_colorMajor.r = r;
    _node->_colorMajor.g = g;
    _node->_colorMajor.b = b;
    _node->updateGridBufUniformValue();
}

void Grid::setColorMinor( float r, float g, float b ) {
    _node->_colorMinor.r = r;
    _node->_colorMinor.g = g;
    _node->_colorMinor.b = b;
    _node->updateGridBufUniformValue();
}

void Grid::setMajorDivisor( float value ) {
    _node->_majorDivisor = value;
    _node->updateGridBufUniformValue();
}

void Grid::setGridScale( float value ) {
    _node->_gridScale = value;
    _node->updatePlaneBufUniformValue();
}

void Grid::setGridZOffset( float value ) {
    _node->_gridZOffset = value;
    _node->updatePlaneBufUniformValue();
}

// ===============================================================================

GridSubgraph::GridSubgraph( vsg::Viewer* viewer )
    : Subgraph{ viewer } {
}

auto GridSubgraph::initPipeline() -> void {
    auto gridSource = TextProgramSource{ "grid" };
    auto vSource = gridSource.stageSource( ShaderStageType::VERTEX );
    auto fSource = gridSource.stageSource( ShaderStageType::FRAGMENT );
    auto vertexShader = vsg::ShaderStage::create( VK_SHADER_STAGE_VERTEX_BIT, "main", vSource );
    auto fragmentShader = vsg::ShaderStage::create( VK_SHADER_STAGE_FRAGMENT_BIT, "main", fSource );

    if ( !vertexShader || !fragmentShader ) {
        std::println( "Could not create shaders." );
        std::terminate();
    }

    // set up graphics pipeline
    auto descriptorBindings = vsg::DescriptorSetLayoutBindings{
        { /* binding */ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, /* count */ 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr },
        { /* binding */ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, /* count */ 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr } };
    auto descriptorSetLayout = vsg::DescriptorSetLayout::create( descriptorBindings );

    _planeBufUniformValue = vsg::floatArray::create( { _gridScale, _gridZOffset, 1.0f, 1.0f } );
    _planeBufUniformValue->properties.dataVariance = vsg::DYNAMIC_DATA;

    auto planeBufUniformDescriptor = vsg::DescriptorBuffer::create( _planeBufUniformValue, /* dstBinding */ 0, 0,
                                                                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER );

    _gridBufUniformValue =
        vsg::floatArray::create( { _gridSize, _lineThickness, _maxRange, _zoomSensitivity, _colorMajor.r, _colorMajor.g,
                                   _colorMajor.b, 1.0, _colorMinor.r, _colorMinor.g, _colorMinor.b, _majorDivisor } );
    _gridBufUniformValue->properties.dataVariance = vsg::DYNAMIC_DATA;

    auto gridBufUniformDescriptor =
        vsg::DescriptorBuffer::create( _gridBufUniformValue, /* dstBinding */ 1, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER );

    auto rasterizerInfo = vsg::RasterizationState::create();
    rasterizerInfo->depthClampEnable = VK_FALSE;
    rasterizerInfo->rasterizerDiscardEnable = VK_FALSE;
    rasterizerInfo->cullMode = VK_CULL_MODE_NONE;
    rasterizerInfo->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizerInfo->depthBiasEnable = VK_FALSE;
    rasterizerInfo->lineWidth = 1.0f;

    auto pipelineStates = vsg::GraphicsPipelineStates{
        vsg::VertexInputState::create(), vsg::InputAssemblyState::create(), rasterizerInfo,
        vsg::MultisampleState::create(), vsg::ColorBlendState::create(),    vsg::DepthStencilState::create() };

    // projection, view, and model matrices, actual push constant calls automatically provided by the VSG's RecordTraversal
    auto pushConstantRanges = vsg::PushConstantRanges{ { VK_SHADER_STAGE_VERTEX_BIT, 0, 128 } };

    auto pipelineLayout =
        vsg::PipelineLayout::create( vsg::DescriptorSetLayouts{ descriptorSetLayout }, pushConstantRanges );
    auto graphicsPipeline = vsg::GraphicsPipeline::create(
        pipelineLayout, vsg::ShaderStages{ vertexShader, fragmentShader }, pipelineStates );
    auto bindGraphicsPipeline = vsg::BindGraphicsPipeline::create( graphicsPipeline );

    auto descriptorSet = vsg::DescriptorSet::create(
        descriptorSetLayout, vsg::Descriptors{ planeBufUniformDescriptor, gridBufUniformDescriptor } );

    auto bindDescriptorSet = vsg::BindDescriptorSet::create( VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
                                                             /* in_firstSet */ 0, descriptorSet );

    _stateGroup->add( bindGraphicsPipeline );
    _stateGroup->add( bindDescriptorSet );
}

auto GridSubgraph::initDrawCommand() -> void {
    auto commands = vsg::Commands::create();

    const auto drawCmd = vsg::Draw::create( 6, 1, 0, 0 );
    commands->addChild( drawCmd );

    _stateGroup->addChild( commands );
}

auto GridSubgraph::recompile() -> void {
    Subgraph::recompile();
}

auto GridSubgraph::updateGridBufUniformValue() -> void {
    ( *_gridBufUniformValue )[0] = _gridSize;
    ( *_gridBufUniformValue )[1] = _lineThickness;
    ( *_gridBufUniformValue )[2] = _maxRange;
    ( *_gridBufUniformValue )[3] = _zoomSensitivity;
    ( *_gridBufUniformValue )[4] = _colorMajor.r;
    ( *_gridBufUniformValue )[5] = _colorMajor.g;
    ( *_gridBufUniformValue )[6] = _colorMajor.b;
    ( *_gridBufUniformValue )[7] = 1.0;
    ( *_gridBufUniformValue )[8] = _colorMinor.r;
    ( *_gridBufUniformValue )[9] = _colorMinor.g;
    ( *_gridBufUniformValue )[10] = _colorMinor.b;
    ( *_gridBufUniformValue )[11] = _majorDivisor;

    _gridBufUniformValue->dirty();
}

auto GridSubgraph::updatePlaneBufUniformValue() -> void {
    ( *_gridBufUniformValue )[0] = _gridScale;
    ( *_gridBufUniformValue )[1] = _gridZOffset;
    ( *_gridBufUniformValue )[2] = 1.0f;
    ( *_gridBufUniformValue )[3] = 1.0f;

    _gridBufUniformValue->dirty();
}

}  // namespace tire
