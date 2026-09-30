
#pragma once

#include <vsg/all.h>

#include "subgraph.h"

namespace tire {

enum class DraggerAxis { X, Y, Z };

// ======================================================================================
// ==================== Gizmo ===========================================================
// ======================================================================================

struct GizmoSubgraph;

struct Gizmo final : public QObject {
    Q_OBJECT

public:
    Gizmo( vsg::observer_ptr<vsg::Viewer> viewer, const QObject* parent = nullptr );

    [[nodiscard]]
    auto node() const -> vsg::ref_ptr<GizmoSubgraph>;

    auto beginDrag() -> void;
    auto update( vsg::dvec3 worldDisplacement ) -> void;

    [[nodiscard]]
    auto translate() const -> vsg::dvec3;

private:
    vsg::ref_ptr<GizmoSubgraph> _gizmo{};

    vsg::dvec3 _translation{};
    vsg::dvec3 _dragAnchor{};
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

    friend Gizmo;

private:
    vsg::ref_ptr<vsg::StateGroup> _stateGroup{};

    vsg::ref_ptr<vsg::Switch> _gizmoKillSwitch{};

    vsg::ref_ptr<vsg::floatArray> _dragerParamUniformValue{};

    vsg::dvec3 _dragersPos{};
    vsg::ref_ptr<vsg::MatrixTransform> _moveDraggersPivot{};

    vsg::ref_ptr<MoveDragger> _xMoveDg{};
    vsg::ref_ptr<MoveDragger> _yMoveDg{};
    vsg::ref_ptr<MoveDragger> _zMoveDg{};
};

// ======================================================================================
// ==================== Dragger =========================================================
// ======================================================================================

struct Dragger : public vsg::MatrixTransform {
    Dragger( DraggerAxis axis );

    auto node() const -> vsg::ref_ptr<vsg::Commands>;
    auto axis() const -> DraggerAxis;
    auto color() const -> vsg::vec3;

protected:
    vsg::ref_ptr<vsg::Commands> _dragger{};
    DraggerAxis _axis{};
    vsg::vec3 _color{};
};

// ======================================================================================
// ==================== MoveDragger =====================================================
// ======================================================================================

struct MoveDragger : public Dragger {
    MoveDragger( DraggerAxis axis );
};

}  // namespace tire