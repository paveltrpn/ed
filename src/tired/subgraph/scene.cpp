

#include <vsg/all.h>

#include "image/tga.h"
#include "log/log.h"

#include "scene.h"

#include "program/program.h"
#include "config/config.h"
#include "../scene_object/sceneobjectdata.h"
#include "../scene_object/box.h"
#include "../scene_object/sphere.h"
#include "../scene_object/cylinder.h"
#include "../scene_object/capsule.h"

#include "../vsgcommands/setpolygonmode.h"

namespace tire {

// ======================================================================================
// ==================== SceneObjects ====================================================
// ======================================================================================

Scene::Scene( vsg::observer_ptr<vsg::Viewer> viewer, QObject* parent )
    : QObject{ parent }
    , _node{ new SceneSubgraph{ viewer } }
    , _objects{ new ObjectsList{ this } } {
    //
    _node->initPipeline();
}

auto Scene::node() const -> vsg::ref_ptr<SceneSubgraph> {
    //
    return _node;
}

auto Scene::objects() const -> ObjectsList* {
    return _objects;
}

auto Scene::selectedObjectUid() const -> QString {
    return _selectedObjectUid.toString();
}

auto Scene::setSelectedObjectUid( const QString& value ) -> void {
    _selectedObjectUid = QUuid::fromString( value );

    emit selectedObjectUidChanged();

    auto obj = _objects->findObject( QUuid::fromString( value ) );

    emit selectedObjectChanged( obj.get() );
}

auto Scene::renderMode() const -> int {
    return static_cast<int>( _renderMode );
}

auto Scene::setRenderMode( int value ) -> void {
    _renderMode = static_cast<ObjectsRenderMode>( value );

    switch ( _renderMode ) {
        case ObjectsRenderMode::WIREFRAME: {
            _node->_polygonModeCmd->mode = VK_POLYGON_MODE_LINE;
            _node->_setCullModeCmd->mode = VK_CULL_MODE_NONE;
            break;
        }
        case ObjectsRenderMode::SOLID: {
            _node->_polygonModeCmd->mode = VK_POLYGON_MODE_FILL;
            _node->_setCullModeCmd->mode = VK_CULL_MODE_BACK_BIT;
            break;
        }
        case ObjectsRenderMode::SOLIDWIRE: {
            break;
        }
        default:
            break;
    }

    emit renderModeChanged();
}

auto Scene::appearnceMode() const -> int {
    return static_cast<int>( _node->_appearnceMode );
}

auto Scene::setAppearnceMode( int value ) -> void {
    _node->_appearnceMode = static_cast<ObjectsAppearenceMode>( value );
    _node->updateObjectParamsUniformValue();
    emit appearnceModeChanged();
}

auto Scene::lightMode() const -> int {
    return static_cast<int>( _node->_lightMode );
}

auto Scene::setLightMode( int value ) -> void {
    _node->_lightMode = static_cast<ObjectsLightMode>( value );
    _node->updateObjectParamsUniformValue();
    emit lightModeChanged();
}

auto Scene::showOuline() const -> bool {
    return _showOuline;
}

auto Scene::setShowOuline( bool value ) -> void {
    _showOuline = value;
    emit showOulineChanged();
}

SceneObjectBase* Scene::findObject( const QString& uid ) const {
    auto obj = _objects->findObject( QUuid::fromString( uid ) );
    return obj.get();
}

void Scene::addBox( const BoxObjectData& data ) {
    auto obj = std::make_shared<object::Box>( data );

    _node->attach( obj );

    _objects->addObject( std::move( obj ) );

    emit objectsChanged();
}

void Scene::addSphere( const SphereObjectData& data ) {
    auto obj = std::make_shared<object::Sphere>( data );

    _node->attach( obj );

    _objects->addObject( std::move( obj ) );

    emit objectsChanged();
}

void Scene::addCylinder( const CylinderObjectData& data ) {
    auto obj = std::make_shared<object::Cylinder>( data );

    _node->attach( obj );

    _objects->addObject( std::move( obj ) );

    emit objectsChanged();
}

void Scene::addCapsule( const CapsuleObjectData& data ) {
    auto obj = std::make_shared<object::Capsule>( data );

    _node->attach( obj );

    _objects->addObject( std::move( obj ) );

    emit objectsChanged();
}

void Scene::removeObject( const QUuid& uid ) {
    qDebug() << "=== " << uid;
    auto obj = _objects->findObject( uid );
    _node->detach( obj );

    // TODO: remove from list and model
}

// ======================================================================================
// ==================== SceneSubgraph ===================================================
// ======================================================================================

SceneSubgraph::SceneSubgraph( vsg::observer_ptr<vsg::Viewer> viewer )
    : Subgraph{ viewer }
    , _stateGroup{ vsg::StateGroup::create() } {
    //
    this->addChild( _stateGroup );
}

auto SceneSubgraph::stateGroups() const -> std::vector<vsg::ref_ptr<vsg::StateGroup>> {
    return { _stateGroup };
}

auto SceneSubgraph::attach( std::shared_ptr<SceneObjectBase> object ) -> void {
    vsg::CompileResult cr{};

    const auto op = tire::AttachAndCompileOp::create( _viewer, _stateGroup, object->node(), cr );

    _viewer.get()->addUpdateOperation( op );
}

auto SceneSubgraph::detach( std::shared_ptr<SceneObjectBase> object ) -> void {
    vsg::CompileResult cr{};

    const auto op = tire::DetachAndCompileOp::create( _viewer, _stateGroup, object->node() );

    _viewer.get()->addUpdateOperation( op );
}

auto SceneSubgraph::initPipeline() -> void {
    auto sceneobjectProgram = Program{ TextProgramSource{ "sceneobject" } };
    auto vertexShader = vsg::ShaderStage::create( VK_SHADER_STAGE_VERTEX_BIT, "main",
                                                  sceneobjectProgram.spirv( ShaderStageType::VERTEX ) );
    auto fragmentShader = vsg::ShaderStage::create( VK_SHADER_STAGE_FRAGMENT_BIT, "main",
                                                    sceneobjectProgram.spirv( ShaderStageType::FRAGMENT ) );

    if ( !vertexShader || !fragmentShader ) {
        log::fatal()( "Could not create shaders." );
    }

    const auto basePath = Config::instance().basePath().string();

    // Read texture image.
    const auto rawData = std::make_unique<Tga>( std::format( "{}{}", basePath, "/assets/textures/glitch.tga" ) );
    auto rawDataSize = rawData->width() * rawData->height() * rawData->components();

    auto textureData = vsg::ubvec4Array2D::create( rawData->width(), rawData->height() );
    textureData->properties.format = VK_FORMAT_R8G8B8A8_SRGB;
    textureData->properties.dataVariance = vsg::DYNAMIC_DATA;
    std::memcpy( textureData->dataPointer(), rawData->data(), rawDataSize );
    textureData->dirty();

    auto textureImage = vsg::ref_ptr<vsg::Image>{ new vsg::Image{ textureData } };
    // textureImage->imageType = VK_IMAGE_TYPE_2D;
    // textureImage->format = VK_FORMAT_R8G8B8A8_SRGB;
    // textureImage->extent = { width, height, 1 };
    // textureImage->mipLevels = 1;
    // textureImage->arrayLayers = 1;
    // textureImage->samples = VK_SAMPLE_COUNT_1_BIT;
    // textureImage->tiling = VK_IMAGE_TILING_OPTIMAL;
    // textureImage->usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    // textureImage->sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    auto textureImageView = vsg::ImageView::create( textureImage );

    // create texture image and associated DescriptorSets and binding
    auto sampler = vsg::Sampler::create();
    sampler->magFilter = VK_FILTER_LINEAR;
    sampler->minFilter = VK_FILTER_LINEAR;
    sampler->mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    auto imageInfo = vsg::ImageInfo::create( sampler, textureImageView );
    auto texture = vsg::DescriptorImage::create( imageInfo, 0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER );

    _objectParamsUniformValue =
        vsg::intArray::create( { static_cast<int>( _appearnceMode ), static_cast<int>( _lightMode ), 0, 0 } );
    _objectParamsUniformValue->properties.dataVariance = vsg::DYNAMIC_DATA;

    const auto objectParamsUniformDescriptor = vsg::DescriptorBuffer::create(
        _objectParamsUniformValue, /* dstBinding */ 1, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER );

    // set up graphics pipeline
    vsg::DescriptorSetLayoutBindings descriptorBindings{
        { /* binding */ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, /* count */ 1, VK_SHADER_STAGE_FRAGMENT_BIT,
          nullptr },
        { /* binding */ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, /* count */ 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr } };

    auto descriptorSetLayout = vsg::DescriptorSetLayout::create( descriptorBindings );

    vsg::PushConstantRanges pushConstantRanges{
        { VK_SHADER_STAGE_VERTEX_BIT, 0, 128 }
        // projection, view, and model matrices, actual push constant calls automatically provided by the VSG's RecordTraversal
    };

    vsg::VertexInputState::Bindings vertexBindingsDescriptions{
        VkVertexInputBindingDescription{ 0, sizeof( vsg::vec3 ), VK_VERTEX_INPUT_RATE_VERTEX },  // vertex data
        VkVertexInputBindingDescription{ 1, sizeof( vsg::vec3 ), VK_VERTEX_INPUT_RATE_VERTEX },  // colour data
        VkVertexInputBindingDescription{ 2, sizeof( vsg::vec2 ), VK_VERTEX_INPUT_RATE_VERTEX },  // tex coord data
        VkVertexInputBindingDescription{ 3, sizeof( vsg::vec3 ), VK_VERTEX_INPUT_RATE_VERTEX }   // normals coord data
    };

    vsg::VertexInputState::Attributes vertexAttributeDescriptions{
        VkVertexInputAttributeDescription{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 },  // vertex data
        VkVertexInputAttributeDescription{ 1, 1, VK_FORMAT_R32G32B32_SFLOAT, 0 },  // colour data
        VkVertexInputAttributeDescription{ 2, 2, VK_FORMAT_R32G32_SFLOAT, 0 },     // tex coord data
        VkVertexInputAttributeDescription{ 3, 3, VK_FORMAT_R32G32B32_SFLOAT, 0 }   // normals coord data
    };

    _rasterizationState = vsg::RasterizationState::create();
    _rasterizationState->depthClampEnable = VK_FALSE;
    _rasterizationState->rasterizerDiscardEnable = VK_FALSE;
    _rasterizationState->polygonMode = VK_POLYGON_MODE_FILL;
    _rasterizationState->cullMode = VK_CULL_MODE_BACK_BIT;
    _rasterizationState->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    _rasterizationState->depthBiasEnable = VK_FALSE;
    _rasterizationState->depthBiasConstantFactor = 1.0f;
    _rasterizationState->depthBiasClamp = 0.0f;
    _rasterizationState->depthBiasSlopeFactor = 1.0f;
    _rasterizationState->lineWidth = 1.0f;

    auto dynamicState = vsg::DynamicState::create();
    dynamicState->dynamicStates = { VK_DYNAMIC_STATE_LINE_WIDTH, VK_DYNAMIC_STATE_POLYGON_MODE_EXT,
                                    VK_DYNAMIC_STATE_CULL_MODE };

    _lineWidthCmd = vsg::SetLineWidth::create();
    _lineWidthCmd->lineWidth = 3.0f;

    _polygonModeCmd = vsg::SetPolygonMode::create();

    _setCullModeCmd = vsg::SetCullMode::create();
    _setCullModeCmd->mode = VK_CULL_MODE_BACK_BIT;

    vsg::GraphicsPipelineStates pipelineStates{
        vsg::VertexInputState::create( vertexBindingsDescriptions, vertexAttributeDescriptions ),
        vsg::InputAssemblyState::create(),
        _rasterizationState,
        vsg::MultisampleState::create(),
        vsg::ColorBlendState::create(),
        vsg::DepthStencilState::create() };

    auto pipelineLayout =
        vsg::PipelineLayout::create( vsg::DescriptorSetLayouts{ descriptorSetLayout }, pushConstantRanges );
    _graphicsPipeline = vsg::GraphicsPipeline::create(
        pipelineLayout, vsg::ShaderStages{ vertexShader, fragmentShader }, pipelineStates );
    auto bindGraphicsPipeline = vsg::BindGraphicsPipeline::create( _graphicsPipeline );

    auto descriptorSet =
        vsg::DescriptorSet::create( descriptorSetLayout, vsg::Descriptors{ texture, objectParamsUniformDescriptor } );
    auto bindDescriptorSet =
        vsg::BindDescriptorSet::create( VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, descriptorSet );

    _stateGroup->add( bindGraphicsPipeline );
    _stateGroup->add( bindDescriptorSet );

    _stateGroup->addChild( _polygonModeCmd );
    _stateGroup->addChild( _lineWidthCmd );
    _stateGroup->addChild( _setCullModeCmd );
}

auto SceneSubgraph::updateObjectParamsUniformValue() -> void {
    ( *_objectParamsUniformValue )[0] = static_cast<int>( _appearnceMode );
    ( *_objectParamsUniformValue )[1] = static_cast<int>( _lightMode );
    ( *_objectParamsUniformValue )[2] = 0;
    ( *_objectParamsUniformValue )[3] = 0;

    _objectParamsUniformValue->dirty();
}

}  // namespace tire
