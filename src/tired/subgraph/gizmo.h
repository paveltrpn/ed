
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
    Gizmo( vsg::observer_ptr<vsg::Viewer> viewer, const QObject* parent = nullptr );

    auto node() const -> vsg::ref_ptr<GizmoSubgraph>;

private:
    vsg::ref_ptr<GizmoSubgraph> _gizmo{};
};

// ======================================================================================
// ==================== GizmoSubgraph ===================================================
// ======================================================================================

struct GizmoSubgraph final : public Subgraph {
    GizmoSubgraph( vsg::observer_ptr<vsg::Viewer> viewer );

    auto stateGroups() const -> std::vector<vsg::ref_ptr<vsg::StateGroup>> override;

    auto initPipeline() -> void;
    auto initDrawCmd() -> void;

private:
    vsg::ref_ptr<vsg::StateGroup> _stateGroup{};
};

}  // namespace tire