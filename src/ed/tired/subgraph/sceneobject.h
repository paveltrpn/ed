
#pragma once

#include "subgraph.h"

namespace tire {

// ======================================================================================
// ==================== SceneObjectSubgraph =============================================
// ======================================================================================

struct SceneObjectSubgraph final : Subgraph {
    SceneObjectSubgraph( vsg::Viewer* viewer );

    auto initPipeline() -> void override;

private:
};

}  // namespace tire
