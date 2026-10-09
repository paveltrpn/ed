#pragma once

#include <vsg/nodes/Group.h>

#include <vsg/app/Camera.h>
#include <vsg/state/ResourceHints.h>
#include <vsg/app/WindowResizeHandler.h>

namespace vsg {

class VSG_DECLSPEC DynamicRenderGraph : public Inherit<Group, DynamicRenderGraph> {
public:
    ~DynamicRenderGraph() override{};

    DynamicRenderGraph( Device* in_device, vsg::ref_ptr<ImageView> colorAttachment,
                        vsg::ref_ptr<ImageView> depthAttachment, uint32_t width, uint32_t height );

    using Group::accept;

    void accept( RecordTraversal& recordTraversal ) const override;

    VkExtent2D getExtent() const { return extent2D(); };

    VkRect2D renderArea;

    // Default ViewportState to use for graphics pipelines under this RenderGraph, this be will synced with the renderArea.
    ref_ptr<ViewportState> viewportState;

    using ClearValues = std::vector<VkClearValue>;
    ClearValues clearValues;  // initialize window colour and depth/stencil

    void setClearValues( VkClearColorValue clearColor = { { 0.2f, 0.2f, 0.4f, 1.0f } },
                         VkClearDepthStencilValue clearDepthStencil = { 0.0f, 0 } );

    uint32_t viewportStateHint = DYNAMIC_VIEWPORTSTATE;

    /// Callback used to automatically update viewports, scissors, renderArea and clears when the window is resized.
    /// By default resize handling is done.
    ref_ptr<WindowResizeHandler> windowResizeHandler;

    /// invoke the WindowResizeHandler, called automatically when a window dimension change is detected.
    void resized();

    VkExtent2D extent2D() const { return VkExtent2D{ _width, _height }; }

    /// window extent at previous frame, used to track window resizes
    constexpr static uint32_t invalid_dimension = std::numeric_limits<uint32_t>::max();
    mutable VkExtent2D previous_extent = VkExtent2D{ invalid_dimension, invalid_dimension };

protected:
    const ref_ptr<Device> _device;

    vsg::ref_ptr<ImageView> _colorAttachment;
    vsg::ref_ptr<ImageView> _depthAttachment;

    const uint32_t _width;
    const uint32_t _height;
};

VSG_type_name( vsg::DynamicRenderGraph );

}  // namespace vsg
