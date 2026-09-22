
#pragma once

#include <vsg/all.h>

#include "subgraph.h"

namespace tire {

// ======================================================================================
// ==================== Gizmo ===========================================================
// ======================================================================================

struct GizmoSubgraph;

struct Gizmo final : public QObject {
    Q_OBJECT

public:
    Gizmo( const QObject* parent = nullptr );
};

// ======================================================================================
// ==================== GizmoSubgraph ===================================================
// ======================================================================================

struct GizmoSubgraph final : public Subgraph {
    GizmoSubgraph( vsg::Viewer* viewer );

    auto stateGroups() const -> std::vector<vsg::ref_ptr<vsg::StateGroup>> override;

    auto initPipeline() -> void override;

private:
};

}  // namespace tire