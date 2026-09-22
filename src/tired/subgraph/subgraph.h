
#pragma once

#include <memory.h>

#include <QObject>

#include <vsg/all.h>

#include "../scene_object/sceneobjectbase.h"

namespace tire {

// ======================================================================================
// ==================== Subgraph ========================================================
// ======================================================================================

struct Subgraph : vsg::Group {
    Subgraph( vsg::Viewer* viewer );

    virtual auto initPipeline() -> void = 0;

    virtual auto recompile() -> void;

    auto link( std::shared_ptr<SceneObjectBase> object ) -> void;

protected:
    vsg::Viewer* _viewer{};

    // "StateGroup is the root of the scene/command graph to hold the GraphicsPipeline, and
    // binding of Descriptors to decorate the whole graph.
    // For example, a vsg::Geometry node alone only defines what to draw. To define
    // how it is drawn (shaders, pipeline state), it must be a child of a vsg::StateGroup
    // configured with the appropriate ShaderSet and pipeline states."
    vsg::ref_ptr<vsg::StateGroup> _stateGroup{};
    vsg::ref_ptr<vsg::StateGroup> _outlineStateGroup{};
};

}  // namespace tire
