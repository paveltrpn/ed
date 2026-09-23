
#include "gizmo.h"

namespace tire {

// ======================================================================================
// ==================== Gizmo ===========================================================
// ======================================================================================

Gizmo::Gizmo( const QObject* parent ) {
    //
}

// ======================================================================================
// ==================== GizmoSubgraph ===================================================
// ======================================================================================

GizmoSubgraph::GizmoSubgraph( vsg::Viewer* viewer )
    : Subgraph{ viewer }
    , _stateGroup{ vsg::StateGroup::create() } {
    //
    this->addChild( _stateGroup );
}

auto GizmoSubgraph::stateGroups() const -> std::vector<vsg::ref_ptr<vsg::StateGroup>> {
    return { _stateGroup };
}

auto GizmoSubgraph::recompile() -> void {
    auto ct = vsg::CompileTraversal::create( *_viewer );
    ct->compile( _stateGroup );
}

auto GizmoSubgraph::initPipeline() -> void{
    //
};

}  // namespace tire