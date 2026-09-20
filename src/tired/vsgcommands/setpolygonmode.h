
#pragma once

#include <vulkan/vulkan.h>

#include <vsg/commands/Command.h>

namespace vsg {

class VSG_DECLSPEC SetPolygonMode : public Inherit<Command, SetPolygonMode> {
public:
    explicit SetPolygonMode( VkPolygonMode in_mode = VK_POLYGON_MODE_FILL );

    VkPolygonMode mode = VK_POLYGON_MODE_FILL;

    void record( CommandBuffer& commandBuffer ) const override;
};

VSG_type_name( vsg::SetPolygonMode );

}  // namespace vsg
