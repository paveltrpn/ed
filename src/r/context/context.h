
#pragma once

#include <atomic>
#include <mutex>
#include <memory>
#include <vector>
#include <array>

#define SURFACE_X11
// #define SURFACE_WAYLAND

#ifdef SURFACE_X11
#define VK_USE_PLATFORM_XLIB_KHR
#elifdef SURFACE_WAYLAND
#define VK_USE_PLATFORM_WAYLAND_KHR
#endif

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#ifdef SURFACE_X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#elifdef SURFACE_WAYLAND
#include <wayland-client.h>
#endif

#include "vkinstance.h"
#include "surface.h"
#include "device.h"
#include "allocator.h"
#include "presentation.h"
#include "contextpools.h"

#include "command_routine.h"

namespace tire {

struct DepthImage;

struct Context final {
    Context( const Context &other ) = delete;
    Context( Context &&other ) = delete;

    auto operator=( const Context &other ) -> Context & = delete;
    auto operator=( Context &&other ) -> Context & = delete;

    static void init( uint32_t width, uint32_t height, Display *display, Window window );
    [[nodiscard]] static Context &instance();

    [[nodiscard]] auto vkInstance() const -> VkInstance {
        //
        return vkInstance_->get();
    }

    [[nodiscard]] auto surface() const -> VkSurfaceKHR {
        //
        return vkSurface_->get();
    }

    [[nodiscard]] auto device() const -> VkDevice {
        //
        return vkDevice_->get();
    }

    [[nodiscard]] auto swapchain() const -> VkSwapchainKHR {
        //
        return swapchain_;
    }

    [[nodiscard]] auto surfaceFormat() const -> const VkSurfaceFormatKHR & {
        //
        return vkSurface_->surfaceFormat();
    };

    [[nodiscard]] auto graphicsQueue() const -> VkQueue {
        //
        return vkDevice_->graphicsQueue();
    }

    [[nodiscard]] auto presentQueue() const -> VkQueue {
        //
        return vkDevice_->presentQueue();
    }

    [[nodiscard]] auto graphicsFamilyQueueId() const -> uint32_t {
        //
        return vkDevice_->graphicsFamilyQueueId();
    }

    [[nodiscard]] auto presentSupportQueueId() const -> uint32_t {
        //
        return vkDevice_->presentSupportQueueId();
    }

    [[nodiscard]] auto physicalDevice() const -> VkPhysicalDevice {
        //
        return vkDevice_->physicalDevice();
    }

    [[nodiscard]] auto currentExtent() const -> const VkExtent2D & {
        //
        return vkSurface_->currentExtent();
    };

    [[nodiscard]] auto viewportSize() -> std::tuple<uint32_t, uint32_t> const {
        //
        return { width_, height_ };
    }

    [[nodiscard]] auto presentMode() const -> VkPresentModeKHR {
        //
        return vkSurface_->presentMode();
    }

    auto surfaceCapabilities() const -> const VkSurfaceCapabilitiesKHR & {
        //
        return vkSurface_->surfaceCapabilities();
    }

    [[nodiscard]] auto memoryRequirements( uint32_t typeFilter, VkMemoryPropertyFlags properties ) const
        -> std::optional<uint32_t>;
    [[nodiscard]] auto findSupportedFormat( const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                            VkFormatFeatureFlags features ) const -> std::optional<VkFormat>;

    [[nodiscard]] auto renderPass() const -> VkRenderPass {
        //
        return renderPass_;
    };

    [[nodiscard]] auto allocator() const -> VmaAllocator {
        //
        return allocator_->get();
    }

    [[nodiscard]] auto getFrameSyncSet( size_t id ) -> std::tuple<VkSemaphore, VkSemaphore, VkFence, VkCommandBuffer> {
        return { frames_[id].imageAvailableSemaphore_, frames_[id].renderFinishedSemaphore_, frames_[id].inFlightFence_,
                 frames_[id].cbPrimary_ };
    }

    [[nodiscard]] auto framebuffer( size_t id ) const -> VkFramebuffer {
        //
        return frames_[id].framebuffer_;
    };

    auto renderCommandBegin( uint32_t frameId ) -> void;
    auto renderCommandEnd( uint32_t frameId ) -> void;

    [[nodiscard]] auto getDrawCommandBuffer( size_t id ) const -> VkCommandBuffer {
        //
        return frames_[id].cbPrimary_;
    }

    [[nodiscard]] auto framesCount() const -> uint32_t {
        //
        return framesCount_;
    };

    [[nodiscard]] auto commandPool() const -> VkCommandPool {
        //
        return contextPools_->commandPool();
    };

    [[nodiscard]] auto descriptorPool() const -> VkDescriptorPool {
        //
        return contextPools_->descriptorPool();
    };

    [[nodiscard]] auto renderCommand( uint32_t frameId ) -> CommandRoutine;
    [[nodiscard]] auto copyBufferCommand() const -> CommandRoutine;
    [[nodiscard]] auto immediateCommand() const -> CommandRoutine;

    // Destroy all Vulkan context here.
    auto releaseContext() -> void;

private:
#ifdef SURFACE_X11
    Context( uint32_t width, uint32_t height, Display *display, Window window );
#elifdef SURFACE_WAYLAND
    Context( uint32_t width, uint32_t height, wl_display *display, wl_surface *surface );
#endif

    ~Context() = default;

    inline static std::atomic<Context *> _instance{ nullptr };
    inline static std::once_flag _initFlag;
    inline static bool _initSuccess{ false };

private:
    struct Frame final {
        VkImage image_{};
        VkImageView view_{};
        VkFramebuffer framebuffer_{};
        VkSemaphore imageAvailableSemaphore_{};
        VkSemaphore renderFinishedSemaphore_{};
        VkFence inFlightFence_{};
        VkCommandBuffer cbPrimary_{ VK_NULL_HANDLE };
        VkCommandBuffer cbSecondary_{ VK_NULL_HANDLE };
    };

private:
    auto makeSwapchain() -> void;
    auto initRenderPass() -> void;
    auto makeFrames() -> void;
    auto initCopyCommandBuffer() -> void;

protected:
    std::unique_ptr<VKInstance> vkInstance_{};
    std::unique_ptr<VKDevice> vkDevice_{};
    std::unique_ptr<VKSurface> vkSurface_{};
    std::unique_ptr<VMAllocator> allocator_{};
    std::unique_ptr<Presentation> presentation_{};
    std::unique_ptr<ContextPools> contextPools_{};

    // Swapchain
    VkSwapchainKHR swapchain_{ VK_NULL_HANDLE };
    uint32_t framesCount_{};
    uint32_t swapchainImageCount_{};
    std::vector<Frame> frames_{};
    std::shared_ptr<DepthImage> depthImage_;

    VkRenderPass renderPass_{ VK_NULL_HANDLE };

    uint32_t width_{};
    uint32_t height_{};

    // Background color value
    std::array<VkClearValue, 2> clearValues_{};

    // Reusable command buffer with fence.
    VkFence copyCommandFence_{};
    VkCommandBuffer copyCommandBuffer_{};
};

}  // namespace tire
