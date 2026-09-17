
#include "marker.h"

namespace tire {

// ======================================================================================
// ==================== MarkerSubgraph ==================================================
// ======================================================================================

MarkerSubgraph::MarkerSubgraph( vsg::Viewer* viewer )
    : Subgraph{ viewer } {
}

auto MarkerSubgraph::initPipeline() -> void {
}

}  // namespace tire