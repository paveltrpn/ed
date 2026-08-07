
#include <print>

#include "grid.h"

namespace tire {

Grid::Grid( vsg::Viewer* viewer, QObject* parent )
    : QObject{ parent }
    , _grid{ new GridSubgraph{ viewer } } {
    //
    _grid->initPipeline();
    _grid->initDrawCommand();
}

auto Grid::grid() const -> vsg::ref_ptr<GridSubgraph> {
    return _grid;
}

float Grid::gridSize() {
    return _grid->_gridSize;
};

float Grid::lineThickness() {
    return _grid->_lineThickness;
};

float Grid::maxRange() {
    return _grid->_maxRange;
};

float Grid::zoomSensitivity() {
    return _grid->_zoomSensitivity;
};

float Grid::majorDivisor() {
    return _grid->_majorDivisor;
};

float Grid::gridScale() {
    return _grid->_gridScale;
};

float Grid::gridZOffset() {
    return _grid->_gridZOffset;
};

void Grid::setGridSize( float value ) {
    _grid->_gridSize = value;
    _grid->updateGridBufUniformValue();
}

void Grid::setLineThickness( float value ) {
    _grid->_lineThickness = value;
    _grid->updateGridBufUniformValue();
}

void Grid::setMaxRange( float value ) {
    _grid->_maxRange = value;
    _grid->updateGridBufUniformValue();
}

void Grid::setZoomSensitivity( float value ) {
    _grid->_zoomSensitivity = value;
    _grid->updateGridBufUniformValue();
}

void Grid::setColorMajor( float r, float g, float b ) {
    _grid->_colorMajor.r = r;
    _grid->_colorMajor.g = g;
    _grid->_colorMajor.b = b;
    _grid->updateGridBufUniformValue();
}

void Grid::setColorMinor( float r, float g, float b ) {
    _grid->_colorMinor.r = r;
    _grid->_colorMinor.g = g;
    _grid->_colorMinor.b = b;
    _grid->updateGridBufUniformValue();
}

void Grid::setMajorDivisor( float value ) {
    _grid->_majorDivisor = value;
    _grid->updateGridBufUniformValue();
}

void Grid::setGridScale( float value ) {
    _grid->_gridScale = value;
    _grid->updatePlaneBufUniformValue();
}

void Grid::setGridZOffset( float value ) {
    _grid->_gridZOffset = value;
    _grid->updatePlaneBufUniformValue();
}

// ===============================================================================

GridSubgraph::GridSubgraph( vsg::Viewer* viewer )
    : Subgraph{ viewer } {
}

auto GridSubgraph::initPipeline() -> void {
    // load shaders
    vsg::Paths searchPaths = std::vector<vsg::Path>{ std::format( "{}{}", PREFIX_PATH_ONE, "/shaders/spirv" ),
                                                     std::format( "{}{}", PREFIX_PATH_ONE, "/assets" ) };

    vsg::ref_ptr<vsg::ShaderStage> vertexShader =
        vsg::ShaderStage::read( VK_SHADER_STAGE_VERTEX_BIT, "main", vsg::findFile( "vert_grid.spv", searchPaths ) );
    vsg::ref_ptr<vsg::ShaderStage> fragmentShader =
        vsg::ShaderStage::read( VK_SHADER_STAGE_FRAGMENT_BIT, "main", vsg::findFile( "frag_grid.spv", searchPaths ) );
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
