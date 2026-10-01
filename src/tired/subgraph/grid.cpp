
#include <print>

#include "program/program.h"
#include "grid.h"

namespace tire {

// ======================================================================================
// ==================== GridUIProxy =====================================================
// ======================================================================================

GridUIProxy::GridUIProxy( vsg::observer_ptr<vsg::Viewer> viewer, QObject* parent )
    : QObject{ parent }
    , _grid{ new GridSubgraph{ viewer } } {
    //
    _grid->initPipeline();
    _grid->initDrawCommand();
}

auto GridUIProxy::gridNode() const -> vsg::ref_ptr<GridSubgraph> {
    return _grid;
}

auto GridUIProxy::gridSize() const -> float {
    return _grid->_gridSize;
};

auto GridUIProxy::lineThickness() const -> float {
    return _grid->_lineThickness;
};

auto GridUIProxy::maxRange() const -> float {
    return _grid->_maxRange;
};

auto GridUIProxy::zoomSensitivity() const -> float {
    return _grid->_zoomSensitivity;
};

auto GridUIProxy::majorDivisor() const -> float {
    return _grid->_majorDivisor;
};

auto GridUIProxy::gridScale() const -> float {
    return _grid->_gridScale;
};

auto GridUIProxy::gridZOffset() const -> float {
    return _grid->_gridZOffset;
};

auto GridUIProxy::setGridSize( float value ) -> void {
    _grid->_gridSize = value;
    _grid->updateGridBufUniformValue();
    emit gridSizeChanged();
}

auto GridUIProxy::setLineThickness( float value ) -> void {
    _grid->_lineThickness = value;
    _grid->updateGridBufUniformValue();
    emit lineThicknessChanged();
}

auto GridUIProxy::setMaxRange( float value ) -> void {
    _grid->_maxRange = value;
    _grid->updateGridBufUniformValue();
    emit maxRangeChanged();
}

auto GridUIProxy::setZoomSensitivity( float value ) -> void {
    _grid->_zoomSensitivity = value;
    _grid->updateGridBufUniformValue();
    emit zoomSensitivityChanged();
}

auto GridUIProxy::setColorMajor( float r, float g, float b ) -> void {
    _grid->_colorMajor.r = r;
    _grid->_colorMajor.g = g;
    _grid->_colorMajor.b = b;
    _grid->updateGridBufUniformValue();
}

auto GridUIProxy::setColorMinor( float r, float g, float b ) -> void {
    _grid->_colorMinor.r = r;
    _grid->_colorMinor.g = g;
    _grid->_colorMinor.b = b;
    _grid->updateGridBufUniformValue();
}

auto GridUIProxy::setMajorDivisor( float value ) -> void {
    _grid->_majorDivisor = value;
    _grid->updateGridBufUniformValue();
    emit majorDivisorChanged();
}

auto GridUIProxy::setGridScale( float value ) -> void {
    _grid->_gridScale = value;
    _grid->updatePlaneBufUniformValue();
    emit gridScaleChanged();
}

auto GridUIProxy::setGridZOffset( float value ) -> void {
    _grid->_gridZOffset = value;
    _grid->updatePlaneBufUniformValue();
    emit gridZOffsetChanged();
}

auto GridUIProxy::updateCameraPosition( const vsg::vec3& value ) -> void {
    _grid->_cameraPosition = value;
    _grid->updateGridBufUniformValue();
}

// ======================================================================================
// ==================== GridSubgraph ====================================================
// ======================================================================================

GridSubgraph::GridSubgraph( vsg::observer_ptr<vsg::Viewer> viewer )
    : Subgraph{ viewer }
    , _stateGroup{ vsg::StateGroup::create() } {
    //
    this->addChild( _stateGroup );
}

auto GridSubgraph::stateGroups() const -> std::vector<vsg::ref_ptr<vsg::StateGroup>> {
    return { _stateGroup };
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
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    auto blendStateInfo = vsg::ColorBlendState::create();
    blendStateInfo->attachments = { colorBlendAttachment };

    auto pipelineStates = vsg::GraphicsPipelineStates{ vsg::VertexInputState::create(),
                                                       vsg::InputAssemblyState::create(),
                                                       rasterizerInfo,
                                                       vsg::MultisampleState::create(),
                                                       blendStateInfo,
                                                       vsg::DepthStencilState::create() };

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
