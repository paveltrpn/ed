#pragma once

#include <vsg/all.h>

#include "viewer.h"

namespace tire {

// ======================================================================================
// ==================== VsgRender =======================================================
// ======================================================================================

struct VsgRender final {
public:
    VsgRender();

    VsgRender( const VsgRender& other ) = delete;
    VsgRender( VsgRender&& other ) = delete;

    auto operator=( const VsgRender& other ) -> VsgRender& = delete;
    auto operator=( VsgRender&& other ) -> VsgRender& = delete;

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

    auto vsgInstance() const -> vsg::ref_ptr<vsg::Instance>;
    auto vsgDevice() const -> vsg::ref_ptr<vsg::Device>;
    auto camera() const -> vsg::ref_ptr<vsg::Camera>;
    auto sceneTransform() const -> vsg::ref_ptr<vsg::MatrixTransform>;

    auto viewer() const -> vsg::ref_ptr<tire::Viewer>;

public:
    /// events buffered since the last pollEvents.
    vsg::UIEvents _bufferedEvents;

private:
    auto _initInstance() -> void;
    auto _initDevice() -> void;

    auto handleResize() -> void;
    auto passEventsToViewer() -> void;

private:
    vsg::ref_ptr<tire::Viewer> _viewer{};
    vsg::ref_ptr<vsg::Camera> _camera{};

    vsg::ref_ptr<vsg::Instance> _instance{};

    vsg::ref_ptr<vsg::PhysicalDevice> _physicalDevice{};
    int _queueFamily;
    vsg::ref_ptr<vsg::Device> _device{};

    VkExtent2D _extent{ 1150, 870 };
    VkFormat _imageFormat{ VK_FORMAT_R8G8B8A8_UNORM };
    VkFormat _depthFormat{ VK_FORMAT_D32_SFLOAT };

    vsg::ref_ptr<vsg::Framebuffer> _framebuffer{};
    vsg::ref_ptr<vsg::ImageView> _colorImageView{};
    vsg::ref_ptr<vsg::ImageView> _depthImageView{};
    vsg::ref_ptr<vsg::Commands> _colorBufferCapture{};
    vsg::ref_ptr<vsg::Image> _copiedColorBuffer{};
    vsg::ref_ptr<vsg::Commands> _depthBufferCapture{};
    vsg::ref_ptr<vsg::Buffer> _copiedDepthBuffer{};

    VkSampleCountFlagBits _samples{ VK_SAMPLE_COUNT_1_BIT };

    vsg::ref_ptr<vsg::CommandGraph> _commandGraph;

    vsg::ref_ptr<vsg::RenderGraph> _renderGraph;

    vsg::ref_ptr<vsg::MatrixTransform> _sceneTransform{};

    bool _needResize{ false };

    VkExtent2D _newExtent{};
};

}  // namespace tire
