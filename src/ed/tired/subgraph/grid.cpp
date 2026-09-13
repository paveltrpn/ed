
#include <print>

#include "program/program.h"
#include "grid.h"

namespace tire {

// ======================================================================================
// ==================== Grid ============================================================
// ======================================================================================

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

auto Grid::gridSize() const -> float {
    return _node->_gridSize;
};

auto Grid::lineThickness() const -> float {
    return _node->_lineThickness;
};

auto Grid::maxRange() const -> float {
    return _node->_maxRange;
};

auto Grid::zoomSensitivity() const -> float {
    return _node->_zoomSensitivity;
};

auto Grid::majorDivisor() const -> float {
    return _node->_majorDivisor;
};

auto Grid::gridScale() const -> float {
    return _node->_gridScale;
};

auto Grid::gridZOffset() const -> float {
    return _node->_gridZOffset;
};

auto Grid::setGridSize( float value ) -> void {
    _node->_gridSize = value;
    _node->updateGridBufUniformValue();
}

auto Grid::setLineThickness( float value ) -> void {
    _node->_lineThickness = value;
    _node->updateGridBufUniformValue();
}

auto Grid::setMaxRange( float value ) -> void {
    _node->_maxRange = value;
    _node->updateGridBufUniformValue();
}

auto Grid::setZoomSensitivity( float value ) -> void {
    _node->_zoomSensitivity = value;
    _node->updateGridBufUniformValue();
}

auto Grid::setColorMajor( float r, float g, float b ) -> void {
    _node->_colorMajor.r = r;
    _node->_colorMajor.g = g;
    _node->_colorMajor.b = b;
    _node->updateGridBufUniformValue();
}

auto Grid::setColorMinor( float r, float g, float b ) -> void {
    _node->_colorMinor.r = r;
    _node->_colorMinor.g = g;
    _node->_colorMinor.b = b;
    _node->updateGridBufUniformValue();
}

auto Grid::setMajorDivisor( float value ) -> void {
    _node->_majorDivisor = value;
    _node->updateGridBufUniformValue();
}

auto Grid::setGridScale( float value ) -> void {
    _node->_gridScale = value;
    _node->updatePlaneBufUniformValue();
}

auto Grid::setGridZOffset( float value ) -> void {
    _node->_gridZOffset = value;
    _node->updatePlaneBufUniformValue();
}

auto Grid::updateCameraPosition( const vsg::vec3& value ) -> void {
    _node->_cameraPosition = value;
    _node->updateGridBufUniformValue();
}

// ======================================================================================
// ==================== GridSubgraph ====================================================
// ======================================================================================

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

    // Set up graphics pipeline.
    const auto descriptorBindings = vsg::DescriptorSetLayoutBindings{
        { /* binding */ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, /* count */ 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr },
        { /* binding */ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, /* count */ 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr } };
    const auto descriptorSetLayout = vsg::DescriptorSetLayout::create( descriptorBindings );

    _planeBufUniformValue = vsg::floatArray::create( { _gridScale, _gridZOffset, -0.0f, -0.0f } );
    _planeBufUniformValue->properties.dataVariance = vsg::DYNAMIC_DATA;

    const auto planeBufUniformDescriptor = vsg::DescriptorBuffer::create( _planeBufUniformValue, /* dstBinding */ 0, 0,
                                                                          VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER );

    _gridBufUniformValue =
        vsg::floatArray::create( { _gridSize, _lineThickness, _maxRange, _zoomSensitivity, _colorMajor.r, _colorMajor.g,
                                   _colorMajor.b, -0.0f, _colorMinor.r, _colorMinor.g, _colorMinor.b, _majorDivisor,
                                   _cameraPosition.x, _cameraPosition.y, _cameraPosition.z, -0.0f } );
    _gridBufUniformValue->properties.dataVariance = vsg::DYNAMIC_DATA;

    const auto gridBufUniformDescriptor =
        vsg::DescriptorBuffer::create( _gridBufUniformValue, /* dstBinding */ 1, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER );

    auto rasterizerInfo = vsg::RasterizationState::create();
    rasterizerInfo->depthClampEnable = VK_FALSE;
    rasterizerInfo->rasterizerDiscardEnable = VK_FALSE;
    rasterizerInfo->cullMode = VK_CULL_MODE_NONE;
    rasterizerInfo->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizerInfo->depthBiasEnable = VK_FALSE;
    rasterizerInfo->lineWidth = 1.0f;

    const VkPipelineColorBlendAttachmentState colorBlendAttachment{
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    auto blendStateInfo = vsg::ColorBlendState::create();
    blendStateInfo->logicOpEnable = VK_TRUE;
    blendStateInfo->logicOp = VK_LOGIC_OP_COPY;
    blendStateInfo->attachments = { colorBlendAttachment };

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
    ( *_gridBufUniformValue )[7] = -0.0f;
    ( *_gridBufUniformValue )[8] = _colorMinor.r;
    ( *_gridBufUniformValue )[9] = _colorMinor.g;
    ( *_gridBufUniformValue )[10] = _colorMinor.b;
    ( *_gridBufUniformValue )[11] = _majorDivisor;
    ( *_gridBufUniformValue )[12] = _cameraPosition.x;
    ( *_gridBufUniformValue )[13] = _cameraPosition.y;
    ( *_gridBufUniformValue )[14] = _cameraPosition.z;
    ( *_gridBufUniformValue )[15] = -0.0f;

    _gridBufUniformValue->dirty();
}

auto GridSubgraph::updatePlaneBufUniformValue() -> void {
    ( *_planeBufUniformValue )[0] = _gridScale;
    ( *_planeBufUniformValue )[1] = _gridZOffset;
    ( *_planeBufUniformValue )[2] = -0.0f;
    ( *_planeBufUniformValue )[3] = -0.0f;

    _planeBufUniformValue->dirty();
}

}  // namespace tire
