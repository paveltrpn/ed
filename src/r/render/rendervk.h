
#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#include "r/event/event.h"
#include "r/event/eventobserver.h"

#include "timer.h"
#include "test_box/test_box.h"
#include "ui/ui.h"

namespace tire {

struct RenderVK final : public EventObserver {
    explicit RenderVK();

    RenderVK( const RenderVK &other ) = delete;
    RenderVK( RenderVK &&other ) = delete;

    auto operator=( const RenderVK &other ) -> RenderVK & = delete;
    auto operator=( RenderVK &&other ) -> RenderVK & = delete;

    ~RenderVK() = default;

    [[nodiscard]] auto isRun() -> bool;

    auto displayRenderInfo() -> void;

    auto preLoop() -> void;
    auto frame() -> void;
    auto postLoop() -> void;

    auto handleEvent( std::shared_ptr<EventBase> event ) -> void override;

private:
    bool run_{};

    uint32_t currentFrame_{ 0 };

    // Ui.
    std::shared_ptr<UiVK> ui_{};

    // Test box.
    std::shared_ptr<TestBox> testBox_{};

    Timer timer_{};

    bool holdMouse_{ false };
};

}  // namespace tire
