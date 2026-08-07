
#include <array>
#include <memory>
#include <filesystem>
#include <variant>

#include <vulkan/vulkan.h>
#include "vulkan/vulkan_core.h"
#include <vulkan/vk_enum_string_helper.h>

#include "vma/vk_mem_alloc.h"

#include "r/context/context.h"
#include "r/context/program.h"
#include "config/config.h"
#include "log/log.h"

#include "r/ui/billboard.h"
#include "r/ui/label.h"

#include "ui.h"
#include "pipeline_ui.h"

namespace tire {

using namespace algebra;

QuadDrawBuffer::QuadDrawBuffer( size_t quadsCount )
    : vBuf_{ VertexBuffer{ quadsCount * VERTICIES_PER_QUAD * 3 * sizeof( float ) } }
    , tBuf_{ VertexBuffer{ quadsCount * VERTICIES_PER_QUAD * 2 * sizeof( float ) } }
    , cBuf_{ VertexBuffer{ quadsCount * VERTICIES_PER_QUAD * 4 * sizeof( float ) } } {
}

// =====================================================================

UiComponentVisitor::UiComponentVisitor( VkCommandBuffer cb, QuadDrawBuffer &labelBuffer,
                                        QuadDrawBuffer &billboardBuffer )
    : cb_{ cb }
    , labelBuffer_{ labelBuffer }
    , billboardBuffer_{ billboardBuffer } {
}

auto UiComponentVisitor::operator()( const tire::Label &item ) -> void {
    //
    dispath( item, labelBuffer_ );
}

auto UiComponentVisitor::operator()( const tire::Billboard &item ) -> void {
    //
    dispath( item, billboardBuffer_ );
}

// =====================================================================

UiVK::UiVK() {
    //
    const auto basePath = Config::instance().getBasePath().string();
    const auto fontFile = Config::instance().get<std::string>( "ui_font" );

    try {
        testImage_ = std::make_shared<TextureImage>( basePath + "/img_fonts/" + fontFile );
    } catch ( std::exception &e ) {
        log::fatal()( "font image {}", e.what() );
    }

    pipeline_ = std::make_shared<PipelineUi>();

    auto program = Program{};
    program.fill( {
        //
        basePath + "/shaders/spirv/vk_ui_VERTEX.spv",
        basePath + "/shaders/spirv/vk_ui_FRAGMENT.spv",
    } );

    pipeline_->buildPipeline( program );

    initTextureSmpler();
    initDescriptorSets();
}

auto UiVK::upload( const VkCommandBuffer cb ) -> void {
    //
    for ( auto &&item : componentsList_ ) {
        std::visit( UiComponentVisitor{ cb, labelBuffer_, billboardBuffer_ }, item );
    }
}

auto UiVK::draw( const VkCommandBuffer cb ) -> void {
    vkCmdBindPipeline( cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->pipeline() );

    // =================================================================================

    std::array<VkDescriptorSet, 1> setsToBind{ fontDescSet_ };
    vkCmdBindDescriptorSets( cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->layout(), 0, 1, setsToBind.data(), 0,
                             nullptr );

    // =================================================================================

    // Pass viewport size.
    const auto v = std::array<float, 4>{ 48, 48, 48, 48 };
    vkCmdPushConstants( cb, pipeline_->layout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof( float ) * 4, &v );

    {
        // Pass disable texture flag.
        const auto f = std::array<uint32_t, 4>{ 0, 0, 0, 0 };
        vkCmdPushConstants( cb, pipeline_->layout(), VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( float ) * 4,
                            sizeof( uint32_t ) * 4, &f );

        auto vbo = billboardBuffer_.vBuf_.deviceBuffer();
        auto tbo = billboardBuffer_.tBuf_.deviceBuffer();
        auto cbo = billboardBuffer_.cBuf_.deviceBuffer();

        std::array<VkBuffer, 3> vertexBuffers = { vbo, tbo, cbo };
        std::array<VkDeviceSize, 3> offsets = { 0, 0, 0 };

        vkCmdBindVertexBuffers( cb, 0, vertexBuffers.size(), vertexBuffers.data(), offsets.data() );

        vkCmdDraw( cb, billboardBuffer_.primitievsCount_, 3, 0, 0 );
    }

    {
        // Pass enable texture flag.
        const auto f = std::array<uint32_t, 4>{ 1, 0, 0, 0 };
        vkCmdPushConstants( cb, pipeline_->layout(), VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( float ) * 4,
                            sizeof( uint32_t ) * 4, &f );

        auto vbo = labelBuffer_.vBuf_.deviceBuffer();
        auto tbo = labelBuffer_.tBuf_.deviceBuffer();
        auto cbo = labelBuffer_.cBuf_.deviceBuffer();

        std::array<VkBuffer, 3> vertexBuffers = { vbo, tbo, cbo };
        std::array<VkDeviceSize, 3> offsets = { 0, 0, 0 };

        vkCmdBindVertexBuffers( cb, 0, vertexBuffers.size(), vertexBuffers.data(), offsets.data() );

        vkCmdDraw( cb, labelBuffer_.primitievsCount_, 3, 0, 0 );
    }
}

auto UiVK::flush() -> void {
    //
    labelBuffer_.primitievsCount_ = 0;
    billboardBuffer_.primitievsCount_ = 0;
    componentsList_.clear();
}

void UiVK::initDescriptorSets() {
    auto pipelineDescSetLayouts = pipeline_->pipelineSescSetsLayout();

    // Write to the texture descriptor set.
    auto texDescSetLayout = pipelineDescSetLayouts[0];
    const auto texallocInfo = VkDescriptorSetAllocateInfo{
        //
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = Context::instance().descriptorPool(),
        .descriptorSetCount = 1,
        .pSetLayouts = &texDescSetLayout,
    };

    {
        const auto err = vkAllocateDescriptorSets( Context::instance().device(), &texallocInfo, &fontDescSet_ );
        if ( err != VK_SUCCESS ) {
            log::fatal()( "error while allocating descriptorSets {}", string_VkResult( err ) );
        }
    }

    const auto textureImageDescInfo = VkDescriptorImageInfo{
        //
        .sampler = fontSampler_,
        .imageView = testImage_->view(),
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    VkWriteDescriptorSet textureWrite = {
        //
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = fontDescSet_,
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = &textureImageDescInfo,
        .pBufferInfo = nullptr,
    };

    vkUpdateDescriptorSets( Context::instance().device(), 1, &textureWrite, 0, nullptr );
}

auto UiVK::initTextureSmpler() -> void {
    const auto info = VkSamplerCreateInfo{
        //
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias = 0.0f,
        .minLod = 0.0f,
        .maxLod = VK_LOD_CLAMP_NONE,
    };

    vkCreateSampler( Context::instance().device(), &info, nullptr, &fontSampler_ );
}

}  // namespace tire