
#include <format>
#include <memory>

#include "context.h"

#include "config/config.h"
#include "log/log.h"
#include "image/color.h"

namespace tire {

struct DepthImage;

void Context::init( uint32_t width, uint32_t height, Display *display, Window window ) {
    if ( _initSuccess ) {
        log::error()( "Warning: Singleton already initialized. Ignoring new arguments." );
    }

    std::call_once( _initFlag, [&]() {
        // We deliberately use 'new' and do not delete.
        // This is intentional. It solves the Static Destruction Order Fiasco.
        // If your Singleton is destroyed during program shutdown, and another static
        // object's destructor tries to use it, your program crashes. By leaking
        // the pointer, the Singleton survives past the end of the program, and the OS automatically
        // reclaims the memory when the process exits anyway.
        _instance.store( new Context( width, height, display, window ) );
        _initSuccess = true;
    } );
}

Context &Context::instance() {
    // memory_order_acquire ensures we see the fully constructed object
    Context *ptr = _instance.load();

    if ( !ptr ) {
        throw std::logic_error( "Singleton must be initialized via init() before calling getInstance()." );
    }
    return *ptr;
}

#ifdef SURFACE_X11
Context::Context( uint32_t width, uint32_t height, Display *display, Window window )
    : width_{ width }
    , height_{ height } {
    //
    vkInstance_ = std::make_unique<VKInstance>( "VK_KHR_xlib_surface" );
    vkDevice_ = std::make_unique<VKDevice>( vkInstance_.get() );
    vkSurface_ = std::make_unique<VKSurfaceXLib>( vkInstance_.get(), vkDevice_.get(), width, height, display, window );
    allocator_ = std::make_unique<VMAllocator>( vkInstance_.get(), vkDevice_.get() );
    presentation_ = std::make_unique<Presentation>( vkInstance_.get(), vkDevice_.get() );
    contextPools_ = std::make_unique<ContextPools>( vkInstance_.get(), vkDevice_.get() );

    makeSwapchain();
    initRenderPass();
    makeFrames();
    initCopyCommandBuffer();

    // Note that the order of clearValues should be identical to the order of your
    // attachments
    const auto colorString = tire::Config::instance().get<std::string>( "background_color" );
    const auto backgroundColor = Colorf( colorString );
    clearValues_[0].color = { { backgroundColor.r(), backgroundColor.g(), backgroundColor.b(), 1.0f } };
    clearValues_[1].depthStencil = { .depth = 1.0f, .stencil = 0 };
}
#elifdef SURFACE_WAYLAND
Context::Context( uint32_t width, uint32_t height, wl_display *display, wl_surface *surface )
    : width_{ width }
    , height_{ height } {
    //
    makeInstance( "VK_KHR_wayland_surface" );
    makeWaylandSurface( display, surface );
    initRest();
}
#endif

auto Context::releaseContext() -> void {
    log::info()( "Release vulkan context..." );

    vkDestroyFence( device(), copyCommandFence_, nullptr );

    for ( auto i = 0; i < framesCount_; i++ ) {
        vkDestroySemaphore( device(), frames_[i].imageAvailableSemaphore_, nullptr );
        vkDestroySemaphore( device(), frames_[i].renderFinishedSemaphore_, nullptr );
        vkDestroyFence( device(), frames_[i].inFlightFence_, nullptr );
        vkDestroyFramebuffer( device(), frames_[i].framebuffer_, nullptr );
        vkDestroyImageView( device(), frames_[i].view_, nullptr );
    }

    vkDestroySwapchainKHR( device(), swapchain_, nullptr );

    presentation_.reset();
    allocator_.reset();
    vkSurface_.reset();
    vkDevice_.reset();
    vkInstance_.reset();
};

}  // namespace tire
