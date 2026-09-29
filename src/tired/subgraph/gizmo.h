
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

struct Dragger;
struct MoveDragger;

struct GizmoSubgraph final : public Subgraph {
    GizmoSubgraph( vsg::observer_ptr<vsg::Viewer> viewer );

    auto stateGroups() const -> std::vector<vsg::ref_ptr<vsg::StateGroup>> override;

    auto initPipeline() -> void;
    auto initDraggers() -> void;

private:
    vsg::ref_ptr<vsg::StateGroup> _stateGroup{};

    vsg::ref_ptr<MoveDragger> _xMoveDg{};
    vsg::ref_ptr<MoveDragger> _yMoveDg{};
    vsg::ref_ptr<MoveDragger> _zMoveDg{};
};

// ======================================================================================
// ==================== Dragger =========================================================
// ======================================================================================

struct Dragger : public vsg::MatrixTransform {
    Dragger();

    auto node() const -> vsg::ref_ptr<vsg::Commands>;

protected:
    vsg::ref_ptr<vsg::Commands> _dragger{};
};

// ======================================================================================
// ==================== MoveDragger =====================================================
// ======================================================================================

struct MoveDragger : public Dragger {
    MoveDragger();
};

}  // namespace tire