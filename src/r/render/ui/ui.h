#pragma once

#include <array>
#include <memory>
#include <filesystem>
#include <variant>
#include <iostream>

#include <vulkan/vulkan.h>
#include "vulkan/vulkan_core.h"
#include <vulkan/vk_enum_string_helper.h>

#include "vma/vk_mem_alloc.h"

#include "r/context/context.h"
#include "r/context/program.h"
#include "config/config.h"
#include "log/log.h"
#include "image/image.h"
#include "algebra/vector3.h"

#include "pipeline_ui.h"

#include "../vertex_buffer.h"
#include "../texture_image.h"

#include "r/ui/ui.h"
#include "r/ui/billboard.h"
#include "r/ui/label.h"

namespace tire {

using namespace algebra;

#define VERTICIES_PER_QUAD 6
#define OUTPUT_QUADS_COUNT 128 * 2

struct QuadDrawBuffer final {
    QuadDrawBuffer( size_t quadsCount );

    VertexBuffer vBuf_;
    VertexBuffer tBuf_;
    VertexBuffer cBuf_;

    uint32_t primitievsCount_{};
};

struct UiComponentVisitor final {
    UiComponentVisitor( VkCommandBuffer cb, QuadDrawBuffer &labelBuffer, QuadDrawBuffer &billboardBuffer );

    auto operator()( const tire::Label &item ) -> void;

    auto operator()( const tire::Billboard &item ) -> void;

    template <typename T>
    auto dispath( const T &item, QuadDrawBuffer &buffer ) -> void {
        const auto vOffset = buffer.primitievsCount_ * 3 * sizeof( float );
        const auto tOffset = buffer.primitievsCount_ * 2 * sizeof( float );
        const auto cOffset = buffer.primitievsCount_ * 4 * sizeof( float );

        const auto vDataPtr = reinterpret_cast<const void *>( item.verteciesData() );
        buffer.vBuf_.memcpy( vDataPtr, item.bufferVerticesSize(), vOffset );

        const auto tDataPtr = reinterpret_cast<const void *>( item.texcrdsData() );
        buffer.tBuf_.memcpy( tDataPtr, item.bufferTexcrdsSize(), tOffset );

        const auto cDataPtr = reinterpret_cast<const void *>( item.clrsData() );
        buffer.cBuf_.memcpy( cDataPtr, item.bufferVertclrsSize(), cOffset );

        VkBufferCopy copyVrt{
            //
            .srcOffset = vOffset,
            .dstOffset = vOffset,
            .size = item.bufferVerticesSize(),
        };

        vkCmdCopyBuffer( cb_, buffer.vBuf_.stagingBuffer(), buffer.vBuf_.deviceBuffer(), 1, &copyVrt );

        VkBufferCopy copyTxc{
            //
            .srcOffset = tOffset,
            .dstOffset = tOffset,
            .size = item.bufferTexcrdsSize(),
        };

        vkCmdCopyBuffer( cb_, buffer.tBuf_.stagingBuffer(), buffer.tBuf_.deviceBuffer(), 1, &copyTxc );

        VkBufferCopy copyClrs{
            //
            .srcOffset = cOffset,
            .dstOffset = cOffset,
            .size = item.bufferVertclrsSize(),
        };

        vkCmdCopyBuffer( cb_, buffer.cBuf_.stagingBuffer(), buffer.cBuf_.deviceBuffer(), 1, &copyClrs );

        buffer.primitievsCount_ += item.lettersCount() * VERTICIES_PER_QUAD;
    }

    VkCommandBuffer cb_;

    QuadDrawBuffer &labelBuffer_;
    QuadDrawBuffer &billboardBuffer_;
};

struct UiVK final : tire::Ui {
    UiVK();

    auto upload( const VkCommandBuffer cb ) -> void;

    auto draw( const VkCommandBuffer cb ) -> void;

    auto flush() -> void override;

private:
    void initDescriptorSets();
    auto initTextureSmpler() -> void;

private:
    std::shared_ptr<PipelineUi> pipeline_{};

    QuadDrawBuffer labelBuffer_{ OUTPUT_QUADS_COUNT };
    QuadDrawBuffer billboardBuffer_{ OUTPUT_QUADS_COUNT };

    std::shared_ptr<tire::TextureImage> testImage_;
    VkSampler fontSampler_{};
    VkDescriptorSet fontDescSet_{};
};

}  // namespace tire