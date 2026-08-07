
#pragma once

#include <vector>
#include <optional>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <vulkan/vulkan.h>

namespace tire {

struct VKInstance;
struct VKDevice;

// ============================================================================
// ============== VKSurface ===================================================
// ============================================================================

struct VKSurface {
    VKSurface( const VKInstance *instance, const VKDevice *device );

    VKSurface( const VKSurface &other ) = delete;
    VKSurface( VKSurface &&other ) = delete;

    auto operator=( const VKSurface &other ) -> VKSurface & = delete;
    auto operator=( VKSurface &&other ) -> VKSurface & = delete;

    virtual ~VKSurface();

    [[nodiscard]] auto get() const -> VkSurfaceKHR;
    [[nodiscard]] auto surfaceFormat() const -> const VkSurfaceFormatKHR &;
    [[nodiscard]] auto currentExtent() const -> const VkExtent2D &;
    [[nodiscard]] auto surfaceCapabilities() const -> const VkSurfaceCapabilitiesKHR &;
    [[nodiscard]] auto presentMode() const -> VkPresentModeKHR;

protected:
    auto physicalDeviceSurfaceCapabilities() -> void;

protected:
    const VKInstance *instance_{};
    const VKDevice *device_{};

    VkSurfaceKHR surface_{ VK_NULL_HANDLE };
    VkSurfaceFormatKHR surfaceFormat_{};
    VkSurfaceCapabilitiesKHR surfaceCapabilities_{};
    std::vector<VkSurfaceFormatKHR> surfaceFormats_{};
    std::vector<VkPresentModeKHR> presentModes_{};
    VkPresentModeKHR presentMode_{};
    VkExtent2D currentExtent_{};
};

// ============================================================================
// ============== VKSurfaceXLib ===============================================
// ============================================================================

struct VKSurfaceXLib final : public VKSurface {
    VKSurfaceXLib( const VKInstance *instance, const VKDevice *device, uint32_t width, uint32_t height,
                   Display *display, Window window );
};

// ============================================================================
// ============== VKSurfaceWayland ============================================
// ============================================================================

/*
#include <wayland-client.h>

struct VKSurfaceWayland final {
    VKSurfaceWayland( const VKInstance *instance, const VKDevice *device, wl_display *display, wl_surface *surface );
};

*/

}  // namespace tire