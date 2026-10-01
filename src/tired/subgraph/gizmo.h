
#pragma once

#include <vsg/all.h>

#include "subgraph.h"
#include "../scene_object/sceneobjectbase.h"

namespace tire {

enum class DraggerAxis {
    //
    X,
    Y,
    Z
};

enum class GizmoMode {
    //
    LOCAL,
    GLOBAL
};

enum class GizmoType {
    //
    MOVE,
    ROTATE,
    SCALE
};

// ======================================================================================
// ==================== GizmoUIProxy ====================================================
// ======================================================================================

struct GizmoSubgraph;

struct GizmoUIProxy final : public QObject {
    Q_OBJECT

    Q_PROPERTY( int gizmoMode READ gizmoMode WRITE setGizmoMode NOTIFY gizmoModeChanged FINAL )
    Q_PROPERTY( int gizmoType READ gizmoType WRITE setGizmoType NOTIFY gizmoTypeChanged FINAL )

public:
    GizmoUIProxy( vsg::observer_ptr<GizmoSubgraph> gizmo, const QObject* parent = nullptr );

    auto beginDrag() -> void;
    auto update( vsg::dvec3 worldDisplacement ) -> void;

    [[nodiscard]] auto translation() const -> vsg::dvec3;
    auto setTranslation( const vsg::dvec3& value ) -> void;

    [[nodiscard]] auto gizmoMode() const -> int;
    auto setGizmoMode( int value ) -> void;

    [[nodiscard]] auto gizmoType() const -> int;
    auto setGizmoType( int value ) -> void;

    auto moveObject( SceneObjectBase* object ) -> void;

signals:
    void gizmoModeChanged();
    void gizmoTypeChanged();

private:
    auto updateGizmoVisibility() -> void;

private:
    vsg::observer_ptr<GizmoSubgraph> _gizmo{};

    vsg::dvec3 _translation{};
    vsg::dvec3 _dragAnchor{};
};

// ======================================================================================
// ==================== GizmoSubgraph ===================================================
// ======================================================================================

struct Dragger;
struct MoveDragger;
struct RotationDragger;
struct ScaleDragger;

struct GizmoSubgraph final : public Subgraph {
    friend GizmoUIProxy;

    GizmoSubgraph( vsg::observer_ptr<vsg::Viewer> viewer );

    auto stateGroups() const -> std::vector<vsg::ref_ptr<vsg::StateGroup>> override;

    auto initPipeline() -> void;
    auto initDraggers() -> void;

private:
    auto showMoveGizmo() -> void;
    auto showRotateGizmo() -> void;
    auto showScaleGizmo() -> void;

private:
    vsg::ref_ptr<vsg::StateGroup> _stateGroup{};

    vsg::ref_ptr<vsg::Switch> _gizmoKillSwitch{};

    vsg::ref_ptr<vsg::floatArray> _dragerParamUniformValue{};

    vsg::dvec3 _dragersPos{};

    vsg::ref_ptr<vsg::MatrixTransform> _gizmoPivot{};

    vsg::ref_ptr<vsg::Switch> _moveGizmoSwitch{};
    vsg::ref_ptr<MoveDragger> _xMoveDg{};
    vsg::ref_ptr<MoveDragger> _yMoveDg{};
    vsg::ref_ptr<MoveDragger> _zMoveDg{};

    vsg::ref_ptr<vsg::Switch> _rotateGizmoSwitch{};
    vsg::ref_ptr<RotationDragger> _xRotateDg{};
    vsg::ref_ptr<RotationDragger> _yRotateDg{};
    vsg::ref_ptr<RotationDragger> _zRotateDg{};

    vsg::ref_ptr<vsg::Switch> _scaleGizmoSwitch{};
    vsg::ref_ptr<ScaleDragger> _xScaleDg{};
    vsg::ref_ptr<ScaleDragger> _yScaleDg{};
    vsg::ref_ptr<ScaleDragger> _zScaleDg{};

    GizmoMode _gizmoMode{ GizmoMode::GLOBAL };
    GizmoType _gizmoType{ GizmoType::MOVE };
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

// ======================================================================================
// ==================== RotationDragger =================================================
// ======================================================================================

struct RotationDragger : public Dragger {
    RotationDragger( DraggerAxis axis );
};

// ======================================================================================
// ==================== ScaleDragger ====================================================
// ======================================================================================

struct ScaleDragger : public Dragger {
    ScaleDragger( DraggerAxis axis );
};

}  // namespace tire