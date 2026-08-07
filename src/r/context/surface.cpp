
#include <exception>

#define SURFACE_X11

#ifdef SURFACE_X11
#define VK_USE_PLATFORM_XLIB_KHR
#elifdef SURFACE_WAYLAND
#define VK_USE_PLATFORM_WAYLAND_KHR
#endif

#include <vulkan/vk_enum_string_helper.h>

#include "vkinstance.h"
#include "device.h"
#include "surface.h"
#include "log/log.h"

namespace tire {

// ============================================================================
// ============== VKSurface ===================================================
// ============================================================================

VKSurface::VKSurface( const VKInstance *instance, const VKDevice *device )
    : instance_{ instance }
    , device_{ device } {
}

auto VKSurface::get() const -> VkSurfaceKHR {
    return surface_;
}

auto VKSurface::surfaceFormat() const -> const VkSurfaceFormatKHR & {
    return surfaceFormat_;
}

auto VKSurface::currentExtent() const -> const VkExtent2D & {
    return currentExtent_;
}

auto VKSurface::surfaceCapabilities() const -> const VkSurfaceCapabilitiesKHR & {
    return surfaceCapabilities_;
}

auto VKSurface::presentMode() const -> VkPresentModeKHR {
    return presentMode_;
}

VKSurface::~VKSurface() {
    vkDestroySurfaceKHR( instance_->get(), surface_, nullptr );
};

auto VKSurface::physicalDeviceSurfaceCapabilities() -> void {
    // Check if picked queue family (with VK_QUEUE_GRAPHICS_BIT) support present.
    // May exist other queue family that not support VK_QUEUE_GRAPHICS_BIT but support
    // present and we can use it to present, but now we just check "can present" property
    // on coosed one and if not we terminate.
    VkBool32 presentSupport = false;
    if ( const auto err = vkGetPhysicalDeviceSurfaceSupportKHR(
             device_->physicalDevice(), device_->graphicsFamilyQueueId(), surface_, &presentSupport );
         err != VK_SUCCESS ) {
        log::fatal()(
            "failed to get device surface support for "
            "presentation with code {}!\n",
            string_VkResult( err ) );
    }

    //
    if ( !presentSupport ) {
        log::fatal()(
            "queue with VK_QUEUE_GRAPHICS_BIT not support present! Maybe check "
            "another queue family?" );
    }

    // Physical device surface capabilities
    if ( const auto err =
             vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device_->physicalDevice(), surface_, &surfaceCapabilities_ );
         err != VK_SUCCESS ) {
        log::fatal()( "failed to obtain surface capabilities with code {}!\n", string_VkResult( err ) );
    } else {
        log::debug()(
            "physical device surface capabilities acquire "
            "success!" );
    }

    // Application window size (extent) givet at startup.
    // NOTE: What i have to do with this when application window
    // have ben resized??
    currentExtent_ = surfaceCapabilities_.currentExtent;

    // Physical device surface formats
    uint32_t formatCount{};
    if ( const auto err =
             vkGetPhysicalDeviceSurfaceFormatsKHR( device_->physicalDevice(), surface_, &formatCount, nullptr );
         err != VK_SUCCESS ) {
        log::fatal()(
            "failed to obtain physical device surface formats "
            "count with code {}!\n",
            string_VkResult( err ) );
    } else {
        log::debug()(
            "physical device surface formats count obtain "
            "success, "
            "count: "
            "{}",
            formatCount );
    }

    surfaceFormats_.resize( formatCount );

    if ( const auto err = vkGetPhysicalDeviceSurfaceFormatsKHR( device_->physicalDevice(), surface_, &formatCount,
                                                                surfaceFormats_.data() );
         err != VK_SUCCESS ) {
        log::fatal()(
            "failed to acquire physical device surface formats "
            "count with code {}!\n",
            string_VkResult( err ) );
    } else {
        log::debug()(
            "physical device surface formats acquire "
            "success!" );
    }

    for ( const auto &item : surfaceFormats_ ) {
        log::notice()( "format: {}", string_VkFormat( item.format ) );
    }

    // Physical device present modes
    uint32_t presentModeCount{};
    if ( const auto err = vkGetPhysicalDeviceSurfacePresentModesKHR( device_->physicalDevice(), surface_,
                                                                     &presentModeCount, nullptr );
         err != VK_SUCCESS ) {
        log::fatal()(
            "failed to obtain physical device present modes "
            "count with code {}!\n",
            string_VkResult( err ) );
    } else {
        log::debug()(
            "physical device present modes count obtain "
            "success, "
            "count: "
            "{}",
            presentModeCount );
    }

    presentModes_.resize( presentModeCount );

    if ( const auto err = vkGetPhysicalDeviceSurfacePresentModesKHR( device_->physicalDevice(), surface_,
                                                                     &presentModeCount, presentModes_.data() );
         err != VK_SUCCESS ) {
        log::fatal()(
            "failed to acquire physical device present modes "
            "with code {}!\n",
            string_VkResult( err ) );
    } else {
        log::debug()( "physical device present modes acquire success!" );
    }

    for ( const auto &item : presentModes_ ) {
        log::notice()( "mode: {}", string_VkPresentModeKHR( item ) );
    }

#define CHOSEN_SURFACE_FORMAT 0
    // This format will be used across application, in
    // swapchain in particular.
    surfaceFormat_ = surfaceFormats_[CHOSEN_SURFACE_FORMAT];

    log::debug()( "format is {}", string_VkFormat( surfaceFormat_.format ) );

#define CHOSEN_PRESENT_MODE VK_PRESENT_MODE_FIFO_KHR
    // This present mode will be used across application, in
    // swapchain in particular.
    presentMode_ = CHOSEN_PRESENT_MODE;  // presentModes_[0];

    log::debug()( "present mode is {}", string_VkPresentModeKHR( presentMode_ ) );
}

// ============================================================================
// ============== VKSurfaceXLib ===============================================
// ============================================================================

VKSurfaceXLib::VKSurfaceXLib( const VKInstance *instance, const VKDevice *device, uint32_t width, uint32_t height,
                              Display *display, Window window )
    : VKSurface{ instance, device } {
    VkXlibSurfaceCreateInfoKHR xlibSurfInfo{};
    xlibSurfInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    xlibSurfInfo.dpy = display;
    xlibSurfInfo.window = window;

    if ( const auto err = vkCreateXlibSurfaceKHR( instance_->get(), &xlibSurfInfo, nullptr, &surface_ );
         err != VK_SUCCESS ) {
        throw std::runtime_error(
            std::format( "failed to create xlib surface with code {}\n!", string_VkResult( err ) ) );
    } else {
        log::info()( "xlib surface created!" );
    }

    physicalDeviceSurfaceCapabilities();
}

// ============================================================================
// ============== VKSurfaceWayland ============================================
// ============================================================================

/*
VKSurfaceWayland::VKSurfaceWayland( const VKInstance *instance, const VKDevice *device, wl_display *display, wl_surface *surface ) {
    VkWaylandSurfaceCreateInfoKHR wlSurfInfo{};
    wlSurfInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    wlSurfInfo.display = display;
    wlSurfInfo.surface = surface;

    if ( const auto err = vkCreateWaylandSurfaceKHR( instance_, &wlSurfInfo, nullptr, &surface_ ); err != VK_SUCCESS ) {
        throw std::runtime_error(
            std::format( "failed to create wayland surface with code {}\n!", string_VkResult( err ) ) );
    } else {
        log::info()( "wayland surface created!" );
    }
}
*/

}  // namespace tire
