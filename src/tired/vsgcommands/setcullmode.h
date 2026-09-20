
#pragma once

#include <vulkan/vulkan.h>

#include <vsg/commands/Command.h>

namespace vsg {

class VSG_DECLSPEC SetCullMode : public Inherit<Command, SetCullMode> {
public:
    explicit SetCullMode( VkCullModeFlagBits in_mode = VK_CULL_MODE_FRONT_BIT );

    VkCullModeFlagBits mode = VK_CULL_MODE_FRONT_BIT;

    void record( CommandBuffer& commandBuffer ) const override;
};

VSG_type_name( vsg::SetCullMode );

}  // namespace vsg

