
#pragma once

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

namespace tire {

struct VKInstance final {
    VKInstance( const std::string &platformSurfaceExtension );

    VKInstance( const VKInstance &other ) = delete;
    VKInstance( VKInstance &&other ) = delete;

    auto operator=( const VKInstance &other ) -> VKInstance & = delete;
    auto operator=( VKInstance &&other ) -> VKInstance & = delete;

    ~VKInstance();

    [[nodiscard]] auto get() const -> VkInstance;
    [[nodiscard]] auto desiredValidationLayerList() const -> const std::vector<const char *> &;

private:
    VkInstance vkInstance_{ VK_NULL_HANDLE };
    VkDebugUtilsMessengerEXT debugMessenger_{ VK_NULL_HANDLE };
    std::vector<const char *> desiredValidationLayerList_{};
    std::vector<VkExtensionProperties> extensionProperties_{};
    std::vector<VkLayerProperties> layerProperties_{};
};

}  // namespace tire
