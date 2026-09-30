
#pragma once

#include <vsg/all.h>

#include "subgraph.h"

namespace tire {

// ======================================================================================
// ==================== BoundingRender ==================================================
// ======================================================================================

struct BoundingRender final : public Subgraph {
    BoundingRender( vsg::observer_ptr<vsg::Viewer> viewer );

    auto initPipeline() -> void;

private:
    vsg::ref_ptr<vsg::StateGroup> _stateGroup{};
    vsg::ref_ptr<vsg::Switch> _killSwitch{};
    vsg::ref_ptr<vsg::Group> _boundingsGroup{};

    float _scale{ 1.0f };
    float _lineLength{ 0.25f };
};

}  // namespace tire
