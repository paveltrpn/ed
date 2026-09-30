
#include "gizmo.h"
#include "program/program.h"
#include "../generatorutils.h"

namespace tire {

// // Создаём узел, который будет отображаться поверх (например, иконка)
// auto iconGeometry = vsg::VertexIndexDraw::create();
// // ... (настройка вершин, пайплайна и шейдеров) ...

// auto absoluteTransform = vsg::AbsoluteTransform::create();

// // В цикле обновления (viewer->update() или кастомный UpdateVisitor):
// // Вычисляем расстояние от камеры до точки привязки (worldPos)
// vsg::dvec3 cameraPos = view->camera->viewMatrix->transform(vsg::dvec3(0,0,0)); // позиция камеры
// double distance = vsg::length(worldPos - cameraPos);

// // Масштаб, чтобы размер в пикселях оставался постоянным:
// // scale = distance * (2 * tan(fov/2) / viewportHeight)
// double fovY = view->camera->projectionMatrix->getFovY(); // условный метод
// double viewportHeight = view->camera->viewportState->getViewport().height;
// double scale = distance * (2.0 * std::tan(fovY * 0.5) / viewportHeight);

// // Матрица: перемещаем в точку + масштабируем + компенсируем масштаб родителя
// vsg::dmat4 matrix = vsg::translate(worldPos) * vsg::scale(scale);
// absoluteTransform->matrix = matrix;

// ======================================================================================
// ==================== Gizmo ===========================================================
// ======================================================================================

Gizmo::Gizmo( vsg::observer_ptr<vsg::Viewer> viewer, const QObject* parent )
    : _gizmo{ new GizmoSubgraph{ viewer } } {
    //
}

auto Gizmo::node() const -> vsg::ref_ptr<GizmoSubgraph> {
    return _gizmo;
}

auto Gizmo::beginDrag() -> void {
    _dragAnchor = _translation;
}

auto Gizmo::update( vsg::dvec3 worldDisplacement ) -> void {
    setTranslation( _dragAnchor + worldDisplacement );
}

auto Gizmo::translation() const -> vsg::dvec3 {
    return _translation;
}

auto Gizmo::setTranslation( const vsg::dvec3& value ) -> void {
    _translation = value;
    _gizmo->_gizmoPivot->matrix = vsg::translate( _translation );
}

auto Gizmo::gizmoMode() const -> int {
    return static_cast<int>( _gizmoMode );
}

void Gizmo::setGizmoMode( int value ) {
    _gizmoMode = static_cast<GizmoMode>( value );
    emit gizmoModeChanged();
}

auto Gizmo::gizmoType() const -> int {
    return static_cast<int>( _gizmoType );
}

auto Gizmo::setGizmoType( int value ) -> void {
    _gizmoType = static_cast<GizmoType>( value );
    emit gizmoTypeChanged();
}

auto Gizmo::moveObject( SceneObjectBase* object ) -> void {
}

// ======================================================================================
// ==================== GizmoSubgraph ===================================================
// ======================================================================================

GizmoSubgraph::GizmoSubgraph( vsg::observer_ptr<vsg::Viewer> viewer )
    : Subgraph{ viewer }
    , _stateGroup{ vsg::StateGroup::create() }
    , _gizmoKillSwitch{ vsg::Switch::create() }
    , _gizmoPivot{ vsg::MatrixTransform::create() } {
    //
    this->addChild( _stateGroup );

    _stateGroup->addChild( _gizmoKillSwitch );

    initPipeline();
    initDraggers();
}

auto GizmoSubgraph::stateGroups() const -> std::vector<vsg::ref_ptr<vsg::StateGroup>> {
    return { _stateGroup };
}

auto GizmoSubgraph::initPipeline() -> void {
    const auto gizmoProgram = Program{ TextProgramSource{ "gizmo" } };
    const auto vertexShader =
        vsg::ShaderStage::create( VK_SHADER_STAGE_VERTEX_BIT, "main", gizmoProgram.spirv( ShaderStageType::VERTEX ) );
    const auto fragmentShader = vsg::ShaderStage::create( VK_SHADER_STAGE_FRAGMENT_BIT, "main",
                                                          gizmoProgram.spirv( ShaderStageType::FRAGMENT ) );

    if ( !vertexShader || !fragmentShader ) {
        log::fatal()( "Could not create shaders." );
    }

    _dragerParamUniformValue = vsg::floatArray::create( { 1.0, 0.0, 0.0, -1.0 } );
    _dragerParamUniformValue->properties.dataVariance = vsg::DYNAMIC_DATA;

    const auto dragerParamUniformDescriptor = vsg::DescriptorBuffer::create(
        _dragerParamUniformValue, /* dstBinding */ 0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER );

    const auto descriptorBindings = vsg::DescriptorSetLayoutBindings{
        { /* binding */ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, /* count */ 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr } };

    const auto descriptorSetLayout = vsg::DescriptorSetLayout::create( descriptorBindings );

    // Projection, view, and model matrices plus dragger color.
    vsg::PushConstantRanges pushConstantRanges{ { VK_SHADER_STAGE_VERTEX_BIT, 0, 128 + sizeof( float ) * 4 } };

    const auto vertexBindingsDescriptions = vsg::VertexInputState::Bindings{
        VkVertexInputBindingDescription{ 0, sizeof( vsg::vec3 ), VK_VERTEX_INPUT_RATE_VERTEX },  // vertex data
    };

    const auto vertexAttributeDescriptions = vsg::VertexInputState::Attributes{
        VkVertexInputAttributeDescription{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 },  // vertex data
    };

    const auto rasterizationState = vsg::RasterizationState::create();
    rasterizationState->depthClampEnable = VK_FALSE;
    rasterizationState->rasterizerDiscardEnable = VK_FALSE;
    rasterizationState->polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationState->cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationState->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationState->depthBiasEnable = VK_FALSE;
    rasterizationState->depthBiasConstantFactor = 1.0f;
    rasterizationState->depthBiasClamp = 0.0f;
    rasterizationState->depthBiasSlopeFactor = 1.0f;
    rasterizationState->lineWidth = 1.0f;

    const auto depthStencilState = vsg::DepthStencilState::create();
    depthStencilState->depthTestEnable = VK_TRUE;
    depthStencilState->depthWriteEnable = VK_TRUE;
    depthStencilState->depthCompareOp = VK_COMPARE_OP_GREATER;
    depthStencilState->depthBoundsTestEnable = VK_FALSE;
    depthStencilState->stencilTestEnable = VK_FALSE;
    depthStencilState->front = {};
    depthStencilState->back = {};
    depthStencilState->minDepthBounds = 0.0f;
    depthStencilState->maxDepthBounds = 1.0f;

    const auto pipelineStates = vsg::GraphicsPipelineStates{
        vsg::VertexInputState::create( vertexBindingsDescriptions, vertexAttributeDescriptions ),
        vsg::InputAssemblyState::create(),
        rasterizationState,
        vsg::MultisampleState::create(),
        vsg::ColorBlendState::create(),
        depthStencilState };

    const auto pipelineLayout =
        vsg::PipelineLayout::create( vsg::DescriptorSetLayouts{ descriptorSetLayout }, pushConstantRanges );
    const auto graphicsPipeline = vsg::GraphicsPipeline::create(
        pipelineLayout, vsg::ShaderStages{ vertexShader, fragmentShader }, pipelineStates );
    const auto bindGraphicsPipeline = vsg::BindGraphicsPipeline::create( graphicsPipeline );

    const auto descriptorSet =
        vsg::DescriptorSet::create( descriptorSetLayout, vsg::Descriptors{ dragerParamUniformDescriptor } );
    const auto bindDescriptorSet =
        vsg::BindDescriptorSet::create( VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, descriptorSet );

    _stateGroup->add( bindGraphicsPipeline );
    _stateGroup->add( bindDescriptorSet );
};

auto GizmoSubgraph::initDraggers() -> void {
    _moveGizmoSwitch = vsg::Switch::create();
    _xMoveDg = vsg::ref_ptr<MoveDragger>{ new MoveDragger{ tire::DraggerAxis::X } };
    _yMoveDg = vsg::ref_ptr<MoveDragger>{ new MoveDragger{ tire::DraggerAxis::Y } };
    _zMoveDg = vsg::ref_ptr<MoveDragger>{ new MoveDragger{ tire::DraggerAxis::Z } };

    _moveGizmoSwitch->addChild( vsg::MASK_ALL, _xMoveDg );
    _moveGizmoSwitch->addChild( vsg::MASK_ALL, _yMoveDg );
    _moveGizmoSwitch->addChild( vsg::MASK_ALL, _zMoveDg );

    _rotateGizmoSwitch = vsg::Switch::create();
    _xRotateDg = vsg::ref_ptr<RotationDragger>{ new RotationDragger{ tire::DraggerAxis::X } };
    _yRotateDg = vsg::ref_ptr<RotationDragger>{ new RotationDragger{ tire::DraggerAxis::Y } };
    _zRotateDg = vsg::ref_ptr<RotationDragger>{ new RotationDragger{ tire::DraggerAxis::Z } };

    _rotateGizmoSwitch->addChild( vsg::MASK_ALL, _xRotateDg );
    _rotateGizmoSwitch->addChild( vsg::MASK_ALL, _yRotateDg );
    _rotateGizmoSwitch->addChild( vsg::MASK_ALL, _zRotateDg );

    _scaleGizmoSwitch = vsg::Switch::create();
    _xScaleDg = vsg::ref_ptr<ScaleDragger>{ new ScaleDragger{ tire::DraggerAxis::X } };
    _yScaleDg = vsg::ref_ptr<ScaleDragger>{ new ScaleDragger{ tire::DraggerAxis::Y } };
    _zScaleDg = vsg::ref_ptr<ScaleDragger>{ new ScaleDragger{ tire::DraggerAxis::Z } };

    _scaleGizmoSwitch->addChild( vsg::MASK_ALL, _xScaleDg );
    _scaleGizmoSwitch->addChild( vsg::MASK_ALL, _yScaleDg );
    _scaleGizmoSwitch->addChild( vsg::MASK_ALL, _zScaleDg );

    _gizmoPivot->addChild( _moveGizmoSwitch );
    _gizmoPivot->addChild( _rotateGizmoSwitch );
    _gizmoPivot->addChild( _scaleGizmoSwitch );

    _gizmoKillSwitch->addChild( vsg::MASK_ALL, _gizmoPivot );
}

// ======================================================================================
// ==================== Dragger =========================================================
// ======================================================================================

Dragger::Dragger( DraggerAxis axis )
    : _dragger{ vsg::Commands::create() }
    , _axis{ axis } {
}

auto Dragger::node() const -> vsg::ref_ptr<vsg::Commands> {
    return _dragger;
}

auto Dragger::axis() const -> DraggerAxis {
    return _axis;
}

auto Dragger::color() const -> vsg::vec3 {
    return _color;
}

// ======================================================================================
// ==================== MoveDragger =====================================================
// ======================================================================================

MoveDragger::MoveDragger( DraggerAxis axis )
    : Dragger{ axis } {
    auto shaft = VsgMeshDataGenerator::cylinder( /* radius */ 0.05,
                                                 /* size */ 2.0,
                                                 /* slices */ 8,
                                                 /* segments */ 4,
                                                 /* rings */ 4,
                                                 /* start */ 0.0,
                                                 /* sweep */ gml::radians( 360.0 ) );

    auto tip = VsgMeshDataGenerator::cone( /* radius */ 0.30,
                                           /* size */ 0.5,
                                           /* slices */ 8,
                                           /* segments */ 4,
                                           /* rings */ 4,
                                           /* start */ 0.0,
                                           /* sweep */ gml::radians( 360.0 ) );

    for ( size_t i{}; i < tip._vertices->size(); ++i ) {
        ( *tip._vertices )[i] += vsg::vec3( 0.0f, 0.0f, 2.0f );
    }

    const auto offst = vsg::translate( vsg::dvec3{ 0.0, 0.0, 2.0 } );

    switch ( _axis ) {
        case tire::DraggerAxis::X: {
            const auto rm = vsg::rotate( vsg::radians( 90.0 ), vsg::dvec3{ 0.0, 1.0, 0.0 } );
            this->matrix = rm * offst;

            _color = vsg::vec3{ 1.0f, 0.0f, 0.0f };

            break;
        }
        case tire::DraggerAxis::Y: {
            const auto rm = vsg::rotate( vsg::radians( 90.0 ), vsg::dvec3{ 1.0, 0.0, 0.0 } );
            this->matrix = rm * offst;

            _color = vsg::vec3{ 0.0f, 1.0f, 0.0f };

            break;
        }
        case tire::DraggerAxis::Z: {
            const auto rm = vsg::rotate( vsg::radians( 0.0 ), vsg::dvec3{ 0.0, 0.0, 1.0 } );
            this->matrix = rm * offst;

            _color = vsg::vec3{ 0.0f, 0.0f, 1.0f };

            break;
        }
    }

    _dragger->addChild( vsg::PushConstants::create(
        VK_SHADER_STAGE_VERTEX_BIT, 128, vsg::floatArray::create( { _color.r, _color.g, _color.b, -1.0 } ) ) );

    _dragger->addChild( vsg::BindVertexBuffers::create( 0, vsg::DataList{ shaft._vertices } ) );
    _dragger->addChild( vsg::BindIndexBuffer::create( shaft._indices ) );
    _dragger->addChild( vsg::DrawIndexed::create( shaft._indicesCount, 1, 0, 0, 0 ) );

    _dragger->addChild( vsg::BindVertexBuffers::create( 0, vsg::DataList{ tip._vertices } ) );
    _dragger->addChild( vsg::BindIndexBuffer::create( tip._indices ) );
    _dragger->addChild( vsg::DrawIndexed::create( tip._indicesCount, 1, 0, 0, 0 ) );

    this->addChild( _dragger );
}

// ======================================================================================
// ==================== RotationDragger =================================================
// ======================================================================================

RotationDragger::RotationDragger( DraggerAxis axis )
    : Dragger{ axis } {
}

// ======================================================================================
// ==================== ScaleDragger ====================================================
// ======================================================================================

ScaleDragger::ScaleDragger( DraggerAxis axis )
    : Dragger{ axis } {
}

}  // namespace tire