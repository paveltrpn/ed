
#pragma once

#include <QObject>

#include <vsg/all.h>

#include "subgraph/grid.h"
#include "subgraph/navbox.h"
#include "subgraph/scene.h"
#include "subgraph/marker.h"
#include "subgraph/bounding.h"
#include "subgraph/gizmo.h"

namespace tire {

enum class GizmoModes {
    //
    LOCAL,
    GLOBAL
};

struct Scenegraph final : public QObject {
    Q_OBJECT

    Q_PROPERTY( QObject* navbox READ navbox NOTIFY navboxChanged FINAL )
    Q_PROPERTY( QObject* grid READ grid NOTIFY gridChanged FINAL )
    Q_PROPERTY( QObject* bounding READ bounding NOTIFY boundingChanged FINAL )
    Q_PROPERTY( QObject* gizmo READ gizmo NOTIFY gizmoChanged FINAL )
    Q_PROPERTY( QObject* scene READ scene NOTIFY sceneChanged FINAL )

    Q_PROPERTY( int gizmoMode READ gizmoMode WRITE setGizmoMode NOTIFY gizmoModeChanged FINAL )

public:
    Scenegraph( vsg::Viewer* viewer, QObject* parent = nullptr );

    Q_INVOKABLE void addExBox( float px, float py, float pz, float rx, float ry, float rz, float sx, float sy,
                               float sz );

    auto root() const -> vsg::ref_ptr<vsg::Group>;

    Navbox* navbox() const;
    Grid* grid() const;
    Bounding* bounding() const;
    Gizmo* gizmo() const;
    Scene* scene() const;

    void setGizmoMode( int value );
    int gizmoMode();

signals:
    void navboxChanged();
    void gridChanged();
    void boundingChanged();
    void gizmoChanged();
    void sceneChanged();

    void gizmoModeChanged();

public slots:
    void lookChanged( const vsg::dvec3& eye, const vsg::dvec3& cnt, const vsg::dvec3& up );

private:
    vsg::Viewer* _viewer;

    // Must be Group of StateGroups.
    vsg::ref_ptr<vsg::Group> _root{};

    Navbox* _navbox{};
    Grid* _grid{};
    Bounding* _bounding{};

    Gizmo* _gizmo{};
    GizmoModes _gizmoMode{};

    Scene* _scene{};

    vsg::ref_ptr<MarkerSubgraph> _markerSubgraph{};
};

}  // namespace tire