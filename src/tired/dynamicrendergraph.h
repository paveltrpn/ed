#pragma once

#include <vsg/nodes/Group.h>

#include <vsg/app/Camera.h>
#include <vsg/state/ResourceHints.h>
#include <vsg/app/WindowResizeHandler.h>

namespace vsg {

// ======================================================================================
// ==================== DynamicRenderGraph ==============================================
// ======================================================================================

class VSG_DECLSPEC DynamicRenderGraph : public Inherit<Group, DynamicRenderGraph> {
public:
    ~DynamicRenderGraph() override{};

    DynamicRenderGraph( Device* in_device, vsg::ref_ptr<ImageView> colorAttachment,
                        vsg::ref_ptr<ImageView> depthAttachment, VkExtent2D extent );

    using Group::accept;

    void accept( RecordTraversal& recordTraversal ) const override;

    VkExtent2D getExtent() const { return _extent; };

    void setClearValues( VkClearColorValue clearColor = { { 0.0f, 0.2f, 0.4f, 1.0f } },
                         VkClearDepthStencilValue clearDepthStencil = { 0.0f, 0 } );

    /// invoke the WindowResizeHandler, called automatically when a window dimension change is detected.
    void resized();

    /// update the attachments and extent after a resize.
    void setAttachments( vsg::ref_ptr<ImageView> colorAttachment, vsg::ref_ptr<ImageView> depthAttachment,
                         VkExtent2D extent );

public:
    uint32_t viewportStateHint{ DYNAMIC_VIEWPORTSTATE };

    // Default ViewportState to use for graphics pipelines under this RenderGraph, this be will synced with the renderArea.
    ref_ptr<ViewportState> viewportState{};

    /// window extent at previous frame, used to track window resizes
    constexpr static uint32_t invalid_dimension = std::numeric_limits<uint32_t>::max();
    mutable VkExtent2D previous_extent = VkExtent2D{ invalid_dimension, invalid_dimension };

    /// Callback used to automatically update viewports, scissors, renderArea and clears when the window is resized.
    /// By default resize handling is done.
    ref_ptr<WindowResizeHandler> windowResizeHandler;

    VkRect2D renderArea{};

    VkExtent2D _extent{};

protected:
    ref_ptr<Device> _device;

    vsg::ref_ptr<ImageView> _colorAttachment;
    vsg::ref_ptr<ImageView> _depthAttachment;

    VkClearValue _clearValue{};
};

VSG_type_name( vsg::DynamicRenderGraph );

}  // namespace vsg
