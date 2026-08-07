
#include "gizmo.h"

namespace tire {

Gizmo::Gizmo( const QObject* parent ) {
    //
}

// ===============================================================================

GizmoSubgraph::GizmoSubgraph( vsg::Viewer* viewer )
    : Subgraph{ viewer } {
    //
}

auto GizmoSubgraph::initPipeline() -> void{
    //
};

}  // namespace tire