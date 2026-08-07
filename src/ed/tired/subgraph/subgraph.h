
#pragma once

#include <memory.h>

#include <QObject>

#include <vsg/all.h>

#include "../scene_object/sceneobjectbase.h"
#include "mesh/mesh.h"
#include "mesh/obj.h"

// #define PREFIX_PATH_ONE "/home/pavel/code/ed"
#define PREFIX_PATH_ONE "/mnt/main/code/ed"

namespace tire {

struct Subgraph : vsg::Group {
    Subgraph( vsg::Viewer* viewer );

    virtual auto initPipeline() -> void = 0;

    virtual auto recompile() -> void;

    auto link( std::shared_ptr<SceneObjectBase> object ) -> void;

protected:
    vsg::Viewer* _viewer{};

    // StateGroup is the root of the scene/command graph to hold the GraphicsPipeline, and
    // binding of Descriptors to decorate the whole graph.
    vsg::ref_ptr<vsg::StateGroup> _stateGroup{};
};

}  // namespace tire
