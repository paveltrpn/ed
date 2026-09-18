

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

namespace tire {

// ======================================================================================
// ==================== SceneObjects ====================================================
// ======================================================================================

Scene::Scene( vsg::Viewer* viewer, QObject* parent )
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

SceneObjectBase* Scene::findObject( const QString& uid ) const {
    auto obj = _objects->findObject( QUuid::fromString( uid ) );
    return obj.get();
}

void Scene::addBox( float px, float py, float pz, float rx, float ry, float rz, float sx, float sy, float sz ) {
    auto data = BoxObjectData{};
    data.setPosition( { px, py, pz } );
    data.setOrientation( { rx, ry, rz } );
    data.setScale( { sx, sy, sz } );

    auto box = std::make_shared<object::Box>( data );

    _node->link( box );

    _objects->addObject( std::move( box ) );

    emit objectsChanged();
}

void Scene::addSphere( float px, float py, float pz, float rx, float ry, float rz, float sx, float sy, float sz ) {
    auto data = SphereObjectData{};
    data.setPosition( { px, py, pz } );
    data.setOrientation( { rx, ry, rz } );
    data.setScale( { sx, sy, sz } );

    auto sphere = std::make_shared<object::Sphere>( data );

    _node->link( sphere );

    _objects->addObject( std::move( sphere ) );

    emit objectsChanged();
}

void Scene::addCylinder( float px, float py, float pz, float rx, float ry, float rz, float sx, float sy, float sz ) {
    auto data = CylinderObjectData{};
    data.setPosition( { px, py, pz } );
    data.setOrientation( { rx, ry, rz } );
    data.setScale( { sx, sy, sz } );

    auto cylinder = std::make_shared<object::Cylinder>( data );

    _node->link( cylinder );

    _objects->addObject( std::move( cylinder ) );

    emit objectsChanged();
}

void Scene::addCapsule( float px, float py, float pz, float rx, float ry, float rz, float sx, float sy, float sz ) {
    auto data = CapsuleObjectData{};
    data.setPosition( { px, py, pz } );
    data.setOrientation( { rx, ry, rz } );
    data.setScale( { sx, sy, sz } );

    auto capsule = std::make_shared<object::Capsule>( data );

    _node->link( capsule );

    _objects->addObject( std::move( capsule ) );

    emit objectsChanged();
}

// ======================================================================================
// ==================== SceneObjectSubgraph =============================================
// ======================================================================================

SceneSubgraph::SceneSubgraph( vsg::Viewer* viewer )
    : Subgraph{ viewer } {
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

    vsg::VertexInputState::Bindings vertexBindingsDescriptions{
        VkVertexInputBindingDescription{ 0, sizeof( vsg::vec3 ), VK_VERTEX_INPUT_RATE_VERTEX },  // vertex data
        VkVertexInputBindingDescription{ 1, sizeof( vsg::vec3 ), VK_VERTEX_INPUT_RATE_VERTEX },  // colour data
        VkVertexInputBindingDescription{ 2, sizeof( vsg::vec2 ), VK_VERTEX_INPUT_RATE_VERTEX }   // tex coord data
    };

    vsg::VertexInputState::Attributes vertexAttributeDescriptions{
        VkVertexInputAttributeDescription{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 },  // vertex data
        VkVertexInputAttributeDescription{ 1, 1, VK_FORMAT_R32G32B32_SFLOAT, 0 },  // colour data
        VkVertexInputAttributeDescription{ 2, 2, VK_FORMAT_R32G32_SFLOAT, 0 }      // tex coord data
    };

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

    // create texture image and associated DescriptorSets and binding
    auto sampler = vsg::Sampler::create();
    sampler->magFilter = VK_FILTER_LINEAR;
    sampler->minFilter = VK_FILTER_LINEAR;
    sampler->mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    auto imageInfo = vsg::ImageInfo::create( sampler, textureImageView );

    auto texture = vsg::DescriptorImage::create( imageInfo, 0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER );

    auto descriptorSet = vsg::DescriptorSet::create( descriptorSetLayout, vsg::Descriptors{ texture } );
    auto bindDescriptorSet =
        vsg::BindDescriptorSet::create( VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, descriptorSet );

    _stateGroup->add( bindGraphicsPipeline );
    _stateGroup->add( bindDescriptorSet );
}

}  // namespace tire
