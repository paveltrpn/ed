
#pragma once

#include <memory.h>
#include <vector>

#include <QObject>

#include <vsg/all.h>

namespace tire {

// ======================================================================================
// ==================== Subgraph ========================================================
// ======================================================================================

// "StateGroup is the root of the scene/command graph to hold the GraphicsPipeline, and
// binding of Descriptors to decorate the whole graph.
// For example, a vsg::Geometry node alone only defines what to draw. To define
// how it is drawn (shaders, pipeline state), it must be a child of a vsg::StateGroup
// configured with the appropriate ShaderSet and pipeline states."
struct Subgraph : vsg::Group {
    Subgraph( vsg::Viewer* viewer );

    virtual auto stateGroups() const -> std::vector<vsg::ref_ptr<vsg::StateGroup>> = 0;
    virtual auto recompile() -> void = 0;

protected:
    vsg::Viewer* _viewer{};
};

}  // namespace tire
