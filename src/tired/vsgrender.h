#pragma once

#include <vsg/all.h>

#include "viewer.h"

namespace tire {

// ======================================================================================
// ==================== InputHandler ====================================================
// ======================================================================================

struct InputHandler final : vsg::Visitor {
    vsg::KeySymbol closeKey = vsg::KEY_Escape;

    InputHandler();

    void apply( vsg::KeyPressEvent& keyPress ) override;
    void apply( vsg::KeyReleaseEvent& keyRelease ) override;
    void apply( vsg::FocusInEvent& focusIn ) override;
    void apply( vsg::FocusOutEvent& focusOut ) override;
    void apply( vsg::ButtonPressEvent& buttonPress ) override;
    void apply( vsg::ButtonReleaseEvent& buttonRelease ) override;
    void apply( vsg::MoveEvent& moveEvent ) override;
    void apply( vsg::ScrollWheelEvent& scrollWheel ) override;
    void apply( vsg::TouchDownEvent& touchDown ) override;
    void apply( vsg::TouchUpEvent& touchUp ) override;
    void apply( vsg::TouchMoveEvent& touchMove ) override;
    void apply( vsg::FrameEvent& frame ) override;
    void apply( vsg::CloseWindowEvent& ) override;
    void apply( vsg::TerminateEvent& ) override;

private:
};

// ======================================================================================
// ==================== VsgRender =======================================================
// ======================================================================================

struct VsgRender final {
public:
    VsgRender( int argc, char** argv );

    VsgRender( const VsgRender& other ) = delete;
    VsgRender( VsgRender&& other ) = delete;

    auto operator=( const VsgRender& other ) -> VsgRender& = delete;
    auto operator=( VsgRender&& other ) -> VsgRender& = delete;

    auto render() -> std::optional<vsg::ref_ptr<vsg::Data>>;
    auto renderNative() -> std::optional<std::tuple<VkImage, VkExtent2D, VkImageLayout>>;

    auto needResize() const -> bool;
    auto setNeedResize( bool value ) -> void;

    auto extent() const -> VkExtent2D;
    auto setExtent( VkExtent2D newExtent ) -> void;

    auto instance() const -> VkInstance;
    auto queueFamily() const -> int;
    auto physicalDevice() const -> VkPhysicalDevice;
    auto logicalDevice() const -> VkDevice;
    auto queueFamilyIndex() const -> uint32_t;

    auto viewer() const -> vsg::ref_ptr<tire::Viewer>;
    auto inputHandler() const -> vsg::ref_ptr<InputHandler>;

public:
    /// events buffered since the last pollEvents.
    vsg::UIEvents _bufferedEvents;

private:
    auto handleResize() -> void;
    auto fetchImageData() -> std::optional<vsg::ref_ptr<vsg::Data>>;
    auto passEventsToViewer() -> void;

private:
    vsg::ref_ptr<tire::Viewer> _viewer{};
    vsg::ref_ptr<InputHandler> _inputHandler{};

    vsg::ref_ptr<vsg::Instance> _instance{};

    vsg::ref_ptr<vsg::PhysicalDevice> _physicalDevice{};
    int _queueFamily;
    vsg::ref_ptr<vsg::Device> _device{};

    VkExtent2D _extent{ 512, 512 };
    VkFormat _imageFormat{ VK_FORMAT_R8G8B8A8_UNORM };
    VkFormat _depthFormat{ VK_FORMAT_D32_SFLOAT };

    vsg::ref_ptr<vsg::Framebuffer> _framebuffer{};
    vsg::ref_ptr<vsg::ImageView> _colorImageView{};
    vsg::ref_ptr<vsg::ImageView> _depthImageView{};
    vsg::ref_ptr<vsg::Commands> _colorBufferCapture{};
    vsg::ref_ptr<vsg::Image> _copiedColorBuffer{};
    vsg::ref_ptr<vsg::Commands> _depthBufferCapture{};
    vsg::ref_ptr<vsg::Buffer> _copiedDepthBuffer{};

    bool _useDepthBuffer{ true };

    VkSampleCountFlagBits _samples{ VK_SAMPLE_COUNT_1_BIT };

    vsg::ref_ptr<vsg::CommandGraph> _commandGraph;

    vsg::ref_ptr<vsg::RenderGraph> _renderGraph;

    vsg::ref_ptr<vsg::MatrixTransform> _sceneTransform{};
    float _angl{};

    bool _needResize{ false };

    VkExtent2D _newExtent{};
};

}  // namespace tire
