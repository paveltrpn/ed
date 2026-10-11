
#pragma once

#include <vulkan/vulkan.h>

#include <vsg/commands/Command.h>
#include <vsg/core/observer_ptr.h>

namespace vsg {

class DynamicRenderGraph;

class VSG_DECLSPEC ClearDepthAttachment : public Inherit<Command, ClearDepthAttachment> {
public:
    explicit ClearDepthAttachment( observer_ptr<DynamicRenderGraph> renderGraph );

    void record( CommandBuffer& commandBuffer ) const override;

private:
    observer_ptr<DynamicRenderGraph> _renderGraph;
};

VSG_type_name( vsg::ClearDepthAttachment );

}  // namespace vsg
