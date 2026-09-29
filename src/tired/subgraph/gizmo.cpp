
#include "gizmo.h"
#include "program/program.h"
#include "../generatorutils.h"

namespace tire {

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

// ======================================================================================
// ==================== GizmoSubgraph ===================================================
// ======================================================================================

GizmoSubgraph::GizmoSubgraph( vsg::observer_ptr<vsg::Viewer> viewer )
    : Subgraph{ viewer }
    , _stateGroup{ vsg::StateGroup::create() }
    , _gizmoKillSwitch{ vsg::Switch::create() } {
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

    // set up graphics pipeline
    const auto descriptorBindings = vsg::DescriptorSetLayoutBindings{
        { /* binding */ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, /* count */ 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr } };

    const auto descriptorSetLayout = vsg::DescriptorSetLayout::create( descriptorBindings );

    // Projection, view, and model matrices plus dragger color.
    vsg::PushConstantRanges pushConstantRanges{ { VK_SHADER_STAGE_VERTEX_BIT, 0, 128 + sizeof( float ) * 4 } };

    const auto vertexBindingsDescriptions = vsg::VertexInputState::Bindings{
        VkVertexInputBindingDescription{ 0, sizeof( vsg::vec3 ), VK_VERTEX_INPUT_RATE_VERTEX },  // vertex data
        VkVertexInputBindingDescription{ 1, sizeof( vsg::vec3 ), VK_VERTEX_INPUT_RATE_VERTEX },  // colour data
    };

    const auto vertexAttributeDescriptions = vsg::VertexInputState::Attributes{
        VkVertexInputAttributeDescription{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 },  // vertex data
        VkVertexInputAttributeDescription{ 1, 1, VK_FORMAT_R32G32B32_SFLOAT, 0 },  // colour data
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

    const auto pipelineStates = vsg::GraphicsPipelineStates{
        vsg::VertexInputState::create( vertexBindingsDescriptions, vertexAttributeDescriptions ),
        vsg::InputAssemblyState::create(),
        rasterizationState,
        vsg::MultisampleState::create(),
        vsg::ColorBlendState::create(),
        vsg::DepthStencilState::create() };

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
    _xMoveDg = vsg::ref_ptr<MoveDragger>{ new MoveDragger{ tire::DraggerAxis::X } };
    _yMoveDg = vsg::ref_ptr<MoveDragger>{ new MoveDragger{ tire::DraggerAxis::Y } };
    _zMoveDg = vsg::ref_ptr<MoveDragger>{ new MoveDragger{ tire::DraggerAxis::Z } };

    _gizmoKillSwitch->addChild( vsg::MASK_ALL, _xMoveDg );
    _gizmoKillSwitch->addChild( vsg::MASK_ALL, _yMoveDg );
    _gizmoKillSwitch->addChild( vsg::MASK_ALL, _zMoveDg );

    // vkCmdPushConstants( cb, _pipeline->layout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof( float ) * 4, &v );
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
    auto data = VsgMeshDataGenerator::cylinder( /* radius */ 0.15,
                                                /* size */ 2.0,
                                                /* slices */ 8,
                                                /* segments */ 4,
                                                /* rings */ 4,
                                                /* start */ 0.0,
                                                /* sweep */ gml::radians( 360.0 ) );

    const auto offst = vsg::translate( vsg::dvec3{ 0.0, 0.0, 2.0 } );

    switch ( _axis ) {
        case tire::DraggerAxis::X: {
            const auto rm = vsg::rotate( vsg::radians( 90.0 ), vsg::dvec3{ 1.0, 0.0, 0.0 } );
            this->matrix = rm * offst;

            _color = vsg::vec3{ 1.0f, 0.0f, 0.0f };

            break;
        }
        case tire::DraggerAxis::Y: {
            const auto rm = vsg::rotate( vsg::radians( 90.0 ), vsg::dvec3{ 0.0, 1.0, 0.0 } );
            this->matrix = rm * offst;

            _color = vsg::vec3{ 0.0f, 1.0f, 0.0f };

            break;
        }
        case tire::DraggerAxis::Z: {
            const auto rm = vsg::rotate( vsg::radians( 90.0 ), vsg::dvec3{ 0.0, 0.0, 1.0 } );
            this->matrix = rm * offst;

            _color = vsg::vec3{ 0.0f, 0.0f, 1.0f };

            break;
        }
    }

    const auto colorPC = vsg::PushConstants::create(
        VK_SHADER_STAGE_VERTEX_BIT, 128, vsg::floatArray::create( { _color.r, _color.g, _color.b, -1.0 } ) );

    _dragger->addChild( colorPC );
    _dragger->addChild( vsg::BindVertexBuffers::create( 0, vsg::DataList{ data._vertices, data._colors } ) );
    _dragger->addChild( vsg::BindIndexBuffer::create( data._indices ) );
    _dragger->addChild( vsg::DrawIndexed::create( data._indicesCount, 1, 0, 0, 0 ) );

    this->addChild( _dragger );
}

}  // namespace tire