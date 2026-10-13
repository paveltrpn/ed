
#include <algorithm>
#include <cmath>

#include "gizmo.h"
#include "program/program.h"
#include "../generatorutils.h"
#include "../dynamicrendergraphicspipeline.h"

namespace tire {

namespace {

constexpr double kGizmoScreenFraction{ 0.18 };
constexpr double kGizmoGeometryExtent{ 1.75 };

}  // namespace

// ======================================================================================
// ==================== GizmoUIProxy ====================================================
// ======================================================================================

GizmoUIProxy::GizmoUIProxy( vsg::observer_ptr<GizmoSubgraph> gizmo, const QObject* parent )
    : _gizmo{ gizmo } {
    //
    updateGizmoVisibility();
}

auto GizmoUIProxy::beginDrag() -> void {
    _dragAnchor = _translation;

    if ( _dragTarget ) {
        _objectAnchor = _dragTarget->node()->_position;
        _objectOrientationAnchor = _dragTarget->node()->_orientation;
        _objectScaleAnchor = _dragTarget->node()->_scale;
    }
}

auto GizmoUIProxy::update( vsg::dvec3 worldDisplacement ) -> void {
    setTranslation( _dragAnchor + worldDisplacement );

    if ( _dragTarget ) {
        const auto position = _objectAnchor + worldDisplacement;
        _dragTarget->setPosition( QVector3D{ static_cast<float>( position.x ), static_cast<float>( position.y ),
                                             static_cast<float>( position.z ) } );
    }
}

auto GizmoUIProxy::updateRotate( DraggerAxis axis, double degrees ) -> void {
    if ( _dragTarget ) {
        auto orientation = _objectOrientationAnchor;

        switch ( axis ) {
            case DraggerAxis::X: {
                orientation.x += degrees;
                break;
            }
            case DraggerAxis::Y: {
                orientation.y += degrees;
                break;
            }
            case DraggerAxis::Z: {
                orientation.z += degrees;
                break;
            }
        }

        _dragTarget->setOrientation( QVector3D{ static_cast<float>( orientation.x ),
                                                static_cast<float>( orientation.y ),
                                                static_cast<float>( orientation.z ) } );
    }
}

auto GizmoUIProxy::updateScale( DraggerAxis axis, double delta ) -> void {
    if ( _dragTarget ) {
        auto scale = _objectScaleAnchor;

        switch ( axis ) {
            case DraggerAxis::X: {
                scale.x += delta;
                break;
            }
            case DraggerAxis::Y: {
                scale.y += delta;
                break;
            }
            case DraggerAxis::Z: {
                scale.z += delta;
                break;
            }
        }

        _dragTarget->setScale( QVector3D{ static_cast<float>( scale.x ), static_cast<float>( scale.y ),
                                          static_cast<float>( scale.z ) } );
    }
}

auto GizmoUIProxy::setDragTarget( SceneObjectBase* object ) -> void {
    _dragTarget = object;

    if ( _dragTarget ) {
        setTranslation( _dragTarget->node()->_position );
    }
}

auto GizmoUIProxy::translation() const -> vsg::dvec3 {
    return _translation;
}

auto GizmoUIProxy::setTranslation( const vsg::dvec3& value ) -> void {
    _translation = value;
    _gizmo.get()->setPivotPosition( value );
}

auto GizmoUIProxy::gizmoMode() const -> int {
    return static_cast<int>( _gizmo.get()->_gizmoMode );
}

void GizmoUIProxy::setGizmoMode( int value ) {
    _gizmo.get()->_gizmoMode = static_cast<GizmoMode>( value );
    emit gizmoModeChanged();
}

auto GizmoUIProxy::gizmoType() const -> int {
    return static_cast<int>( _gizmo.get()->_gizmoType );
}

auto GizmoUIProxy::setGizmoType( int value ) -> void {
    _gizmo.get()->_gizmoType = static_cast<GizmoType>( value );
    updateGizmoVisibility();
    emit gizmoTypeChanged();
}

auto GizmoUIProxy::updateGizmoVisibility() -> void {
    switch ( _gizmo.get()->_gizmoType ) {
        case tire::GizmoType::MOVE: {
            _gizmo.get()->showMoveGizmo();
            break;
        }

        case tire::GizmoType::ROTATE: {
            _gizmo.get()->showRotateGizmo();
            break;
        }

        case tire::GizmoType::SCALE: {
            _gizmo.get()->showScaleGizmo();
            break;
        }
    }
}

// ======================================================================================
// ==================== GizmoSubgraph ===================================================
// ======================================================================================

GizmoSubgraph::GizmoSubgraph( vsg::observer_ptr<vsg::Viewer> viewer,
                              vsg::observer_ptr<vsg::DynamicRenderGraph> renderGraph )
    : Subgraph{ viewer }
    , _depthClear{ vsg::ClearDepthAttachment::create( renderGraph ) }
    , _stateGroup{ vsg::StateGroup::create() }
    , _gizmoKillSwitch{ vsg::Switch::create() }
    , _gizmoPivot{ vsg::AbsoluteTransform::create() } {
    //
    // Depth clear must record before any gizmo geometry so the gizmo draws
    // on top of everything recorded earlier in the pass.
    this->addChild( _depthClear );
    this->addChild( _stateGroup );

    _stateGroup->addChild( _gizmoKillSwitch );

    initPipeline();
    initDraggers();
}

auto GizmoSubgraph::stateGroups() const -> std::vector<vsg::ref_ptr<vsg::StateGroup>> {
    return { _stateGroup };
}

auto GizmoSubgraph::setPivotPosition( const vsg::dvec3& position ) -> void {
    _pivotPosition = position;

    _gizmoPivot->matrix = vsg::translate( _pivotPosition );
}

auto GizmoSubgraph::updateScreenTransform( const vsg::Camera* camera ) -> void {
    const auto perspective = camera->projectionMatrix->cast<vsg::Perspective>();
    if ( !perspective ) {
        return;
    }

    const auto viewportHeight = static_cast<double>( camera->getViewport().height );
    if ( viewportHeight <= 0.0 ) {
        return;
    }

    const auto view = camera->viewMatrix->transform();
    const auto eyeHom = vsg::inverse( view ) * vsg::dvec4{ 0.0, 0.0, 0.0, 1.0 };
    const auto eye = vsg::dvec3{ eyeHom.x, eyeHom.y, eyeHom.z };

    const auto distance = std::max( vsg::length( eye - _pivotPosition ), 1e-6 );
    const auto worldPerHeight = 2.0 * distance * std::tan( vsg::radians( perspective->fieldOfViewY ) * 0.5 );
    const auto s = kGizmoScreenFraction * worldPerHeight / kGizmoGeometryExtent;

    _gizmoPivot->matrix = view * vsg::translate( _pivotPosition ) * vsg::scale( s );
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

    const auto graphicsPipeline =
        vsg::ref_ptr<vsg::DynamicRenderGraphicsPipeline>{ new vsg::DynamicRenderGraphicsPipeline{
            pipelineLayout, vsg::ShaderStages{ vertexShader, fragmentShader }, pipelineStates } };

    const auto bindGraphicsPipeline = vsg::BindDynamicRenderGraphicsPipeline::create( graphicsPipeline );

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

auto GizmoSubgraph::showMoveGizmo() -> void {
    _moveGizmoSwitch->setAllChildren( true );
    _rotateGizmoSwitch->setAllChildren( false );
    _scaleGizmoSwitch->setAllChildren( false );
}

auto GizmoSubgraph::showRotateGizmo() -> void {
    _moveGizmoSwitch->setAllChildren( false );
    _rotateGizmoSwitch->setAllChildren( true );
    _scaleGizmoSwitch->setAllChildren( false );
}

auto GizmoSubgraph::showScaleGizmo() -> void {
    _moveGizmoSwitch->setAllChildren( false );
    _rotateGizmoSwitch->setAllChildren( false );
    _scaleGizmoSwitch->setAllChildren( true );
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
                                                 /* size */ 1.25,
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
        ( *tip._vertices )[i] += vsg::vec3( 0.0f, 0.0f, 1.0f );
    }

    const auto offst = vsg::translate( vsg::dvec3{ 0.0, 0.0, 1.0 } );

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
    auto torus = VsgMeshDataGenerator::torus( /* minor */ 0.08,
                                              /* major */ 1.0,
                                              /* slices */ 12,
                                              /* segments */ 24,
                                              /* minorStart */ 0.0,
                                              /* minorSweep */ gml::radians( 360.0 ),
                                              /* majorStart */ 0.0,
                                              /* majorSweep */ gml::radians( 360.0 ) );

    switch ( _axis ) {
        case tire::DraggerAxis::X: {
            const auto rm = vsg::rotate( vsg::radians( 90.0 ), vsg::dvec3{ 0.0, 1.0, 0.0 } );
            this->matrix = rm;

            _color = vsg::vec3{ 1.0f, 0.0f, 0.0f };

            break;
        }
        case tire::DraggerAxis::Y: {
            const auto rm = vsg::rotate( vsg::radians( 90.0 ), vsg::dvec3{ 1.0, 0.0, 0.0 } );
            this->matrix = rm;

            _color = vsg::vec3{ 0.0f, 1.0f, 0.0f };

            break;
        }
        case tire::DraggerAxis::Z: {
            const auto rm = vsg::rotate( vsg::radians( 0.0 ), vsg::dvec3{ 0.0, 0.0, 1.0 } );
            this->matrix = rm;

            _color = vsg::vec3{ 0.0f, 0.0f, 1.0f };

            break;
        }
    }

    _dragger->addChild( vsg::PushConstants::create(
        VK_SHADER_STAGE_VERTEX_BIT, 128, vsg::floatArray::create( { _color.r, _color.g, _color.b, -1.0 } ) ) );

    _dragger->addChild( vsg::BindVertexBuffers::create( 0, vsg::DataList{ torus._vertices } ) );
    _dragger->addChild( vsg::BindIndexBuffer::create( torus._indices ) );
    _dragger->addChild( vsg::DrawIndexed::create( torus._indicesCount, 1, 0, 0, 0 ) );

    this->addChild( _dragger );
}

// ======================================================================================
// ==================== ScaleDragger ====================================================
// ======================================================================================

ScaleDragger::ScaleDragger( DraggerAxis axis )
    : Dragger{ axis } {
    auto shaft = VsgMeshDataGenerator::cylinder( /* radius */ 0.05,
                                                 /* size */ 1.25,
                                                 /* slices */ 8,
                                                 /* segments */ 4,
                                                 /* rings */ 4,
                                                 /* start */ 0.0,
                                                 /* sweep */ gml::radians( 360.0 ) );

    auto tip = VsgMeshDataGenerator::cylinder( /* radius */ 0.25,
                                               /* size */ 0.45,
                                               /* slices */ 8,
                                               /* segments */ 4,
                                               /* rings */ 4,
                                               /* start */ 0.0,
                                               /* sweep */ gml::radians( 360.0 ) );

    for ( size_t i{}; i < tip._vertices->size(); ++i ) {
        ( *tip._vertices )[i] += vsg::vec3( 0.0f, 0.0f, 1.0f );
    }

    const auto offst = vsg::translate( vsg::dvec3{ 0.0, 0.0, 1.0 } );

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

}  // namespace tire