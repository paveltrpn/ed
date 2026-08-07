#pragma once

#include <vsg/all.h>

#include "subgraph.h"

namespace tire {

struct MarkerSubgraph final : Subgraph {
    MarkerSubgraph( vsg::Viewer* viewer );

    auto initPipeline() -> void override;

private:
};

}  // namespace tire
