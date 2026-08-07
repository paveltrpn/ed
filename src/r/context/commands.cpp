
#include <coroutine>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#include "context.h"
#include "command_routine.h"
#include "log/log.h"

namespace tire {

auto Context::immediateCommand() const -> CommandRoutine {
    const auto allocInfo = VkCommandBufferAllocateInfo{
        //
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool(),
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer cb{};
    vkAllocateCommandBuffers( device(), &allocInfo, &cb );

    VkCommandBufferUsageFlags usageFlags = {
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    const VkCommandBufferBeginInfo beginInfo{
        //
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = usageFlags,
        .pInheritanceInfo = nullptr,
    };

    vkBeginCommandBuffer( cb, &beginInfo );

    // ==============================================
    // Suspend and record commands from other places.
    co_yield cb;
    // Resume, finish record command and submit.
    // ==============================================

    vkEndCommandBuffer( cb );

    std::array<VkPipelineStageFlags, 0> waitStages{};
    std::array<VkSemaphore, 0> waitsems{};
    std::array<VkSemaphore, 0> sgnlsems{};
    std::array<VkCommandBuffer, 1> commands{
        //
        cb,
    };

    const VkSubmitInfo submitInfo{ .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                   .waitSemaphoreCount = waitsems.size(),
                                   .pWaitSemaphores = waitsems.data(),
                                   .pWaitDstStageMask = waitStages.data(),
                                   .commandBufferCount = static_cast<uint32_t>( commands.size() ),
                                   .pCommandBuffers = commands.data(),
                                   .signalSemaphoreCount = sgnlsems.size(),
                                   .pSignalSemaphores = sgnlsems.data() };

    {
        const auto err = vkQueueSubmit( graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE );
        if ( err != VK_SUCCESS ) {
            log::fatal()( "error while command coroutine {}", string_VkResult( err ) );
        }
    }

    vkQueueWaitIdle( graphicsQueue() );

    vkFreeCommandBuffers( device(), commandPool(), 1, &cb );
}

auto Context::copyBufferCommand() const -> CommandRoutine {
    // Record command to transfer data from CPU to GPU side.
    VkCommandBufferUsageFlags usageFlags = {
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    const VkCommandBufferBeginInfo beginInfo{
        //
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = usageFlags,
        .pInheritanceInfo = nullptr,
    };

    std::array<VkFence, 1> fences = {
        copyCommandFence_,
    };

    vkResetCommandBuffer( copyCommandBuffer_, 0 );

    vkBeginCommandBuffer( copyCommandBuffer_, &beginInfo );

    // ==============================================
    // Suspend and record commands from other places.
    co_yield copyCommandBuffer_;
    // Resume, finish record command and submit.
    // ==============================================

    vkEndCommandBuffer( copyCommandBuffer_ );

    std::array<VkPipelineStageFlags, 1> waitStages{
        //
        VK_PIPELINE_STAGE_TRANSFER_BIT,
    };

    std::array<VkSemaphore, 0> waitsems{};
    std::array<VkSemaphore, 0> sgnlsems{};
    std::array<VkCommandBuffer, 1> commands{
        //
        copyCommandBuffer_,
    };

    const VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = waitsems.size(),
        .pWaitSemaphores = waitsems.data(),
        .pWaitDstStageMask = waitStages.data(),
        .commandBufferCount = static_cast<uint32_t>( commands.size() ),
        .pCommandBuffers = commands.data(),
        .signalSemaphoreCount = sgnlsems.size(),
        .pSignalSemaphores = sgnlsems.data(),
    };

    vkQueueSubmit( graphicsQueue(), 1, &submitInfo, copyCommandFence_ );

    vkWaitForFences( device(), fences.size(), fences.data(), VK_TRUE, UINT64_MAX );
    vkResetFences( device(), fences.size(), fences.data() );
}

auto Context::renderCommand( uint32_t frameId ) -> CommandRoutine {
    const auto [iaSem, rfSem, ifFnc, cb] = getFrameSyncSet( frameId );

    std::array<VkFence, 1> fences = { ifFnc };

    // NOTE: omit return code check
    vkWaitForFences( device(), fences.size(), fences.data(), VK_TRUE, UINT64_MAX );

    // NOTE: omit return code check
    vkResetFences( device(), fences.size(), fences.data() );

    vkResetCommandBuffer( cb, 0 );

    // NOTE: omit return code check
    // May return VK_SUBOPTIMAL_KHR or even VK_ERROR_OUT_OF_DATE_KHR
    // if current surface properties are no longer matched
    // exactly or swap chain has become incompatible
    // with the surface and can no longer be used for rendering
    uint32_t imageIndex{};
    vkAcquireNextImageKHR( device(), swapchain(), UINT64_MAX, iaSem, VK_NULL_HANDLE, &imageIndex );

    const VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, .flags = 0, .pInheritanceInfo = nullptr };

    vkBeginCommandBuffer( cb, &beginInfo );

    const auto currentFramebuffer = framebuffer( frameId );

    const auto [viewportWidth, viewportHeight] = viewportSize();

    const VkRenderPassBeginInfo renderPassInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = renderPass(),
        .framebuffer = currentFramebuffer,
        .renderArea = { .offset = { .x = 0, .y = 0 }, .extent = { viewportWidth, viewportHeight } },
        .clearValueCount = static_cast<uint32_t>( clearValues_.size() ),
        .pClearValues = clearValues_.data() };

    vkCmdBeginRenderPass( cb, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );

    const auto [width, height] = viewportSize();

    // Dynamic viewport. No performance penalty.
    // Take out work from pipeline creation.
    // NOTE: Define negative viewport size to use same projection matrix as
    // for OpenGL pipeline.
    const VkViewport viewport{
        .x = 0.0f,
        .y = static_cast<float>( height ),
        .width = static_cast<float>( width ),
        .height = -static_cast<float>( height ),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    // const VkViewport viewport{ .x = 0.0f,
    //    .y = 0.0f,
    //    .width = static_cast<float>( width ),
    //    .height = static_cast<float>( height ),
    //    .minDepth = 0.0f,
    //                         .maxDepth = 1.0f };
    vkCmdSetViewport( cb, 0, 1, &viewport );

    const VkRect2D scissor{ { .x = 0, .y = 0 }, { .width = width, .height = height } };
    vkCmdSetScissor( cb, 0, 1, &scissor );

    // ==============================================
    // Suspend and record commands from other places.
    co_yield cb;
    // Resume, finish record command and submit.
    // ==============================================

    vkCmdEndRenderPass( cb );
    // NOTE: omit return code check
    vkEndCommandBuffer( cb );

    std::array<VkPipelineStageFlags, 1> waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    std::array<VkSemaphore, 1> waitsems{ iaSem };
    std::array<VkSemaphore, 1> sgnlsems{ rfSem };
    std::array<VkCommandBuffer, 1> commands{ cb };

    const VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = static_cast<uint32_t>( waitsems.size() ),
        .pWaitSemaphores = waitsems.data(),
        .pWaitDstStageMask = waitStages.data(),
        .commandBufferCount = static_cast<uint32_t>( commands.size() ),
        .pCommandBuffers = commands.data(),
        .signalSemaphoreCount = static_cast<uint32_t>( sgnlsems.size() ),
        .pSignalSemaphores = sgnlsems.data(),
    };

    // NOTE: PRESET!
    // NOTE: omit return code check
    vkQueueSubmit( graphicsQueue(), static_cast<uint32_t>( commands.size() ), &submitInfo, ifFnc );

    std::array<VkSwapchainKHR, 1> swapChains = { swapchain_ };
    std::array<VkSemaphore, 1> signalSemaphores = { rfSem };
    std::array<uint32_t, 1> imageIds = { frameId };

    const VkPresentInfoKHR presentInfo{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = static_cast<uint32_t>( signalSemaphores.size() ),
        .pWaitSemaphores = signalSemaphores.data(),
        .swapchainCount = static_cast<uint32_t>( swapChains.size() ),
        .pSwapchains = swapChains.data(),
        .pImageIndices = imageIds.data(),
        .pResults = nullptr,
    };

    // May return VK_SUBOPTIMAL_KHR or even VK_ERROR_OUT_OF_DATE_KHR
    // if current surface properties are no longer matched
    // exactly or swap chain has become incompatible
    // with the surface and can no longer be used for rendering
    if ( const auto res = vkQueuePresentKHR( presentQueue(), &presentInfo ); res != VK_SUCCESS ) {
        log::warning()( "queue present result {}, is it normal?", string_VkResult( res ) );
    }
}

}  // namespace tire
