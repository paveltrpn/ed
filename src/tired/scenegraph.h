
#pragma once

#include <QObject>

#include <vsg/all.h>

#include "subgraph/grid.h"
#include "subgraph/navbox.h"
#include "subgraph/scene.h"
#include "subgraph/gizmo.h"
#include "subgraph/boundingrender.h"
#include "vsg/core/ref_ptr.h"

namespace tire {

struct Scenegraph final : public QObject {
    Q_OBJECT

    Q_PROPERTY( QObject* navbox READ navbox NOTIFY navboxChanged FINAL )
    Q_PROPERTY( QObject* grid READ grid NOTIFY gridChanged FINAL )
    Q_PROPERTY( QObject* gizmo READ gizmo NOTIFY gizmoChanged FINAL )
    Q_PROPERTY( QObject* scene READ scene NOTIFY sceneChanged FINAL )

public:
    Scenegraph( vsg::observer_ptr<vsg::Viewer>, QObject* parent = nullptr );

    [[nodiscard]]
    auto root() const -> vsg::ref_ptr<vsg::Group>;

    [[nodiscard]]
    auto scenegraphViewer() const -> vsg::observer_ptr<vsg::Viewer>;

    [[nodiscard]]
    auto navbox() const -> Navbox*;

    [[nodiscard]]
    auto grid() const -> Grid*;

    [[nodiscard]]
    auto gizmo() const -> GizmoUIProxy*;

    [[nodiscard]]
    auto scene() const -> Scene*;

signals:
    void navboxChanged();
    void gridChanged();
    void boundingChanged();
    void gizmoChanged();
    void sceneChanged();

public slots:
    void lookChanged( const vsg::dvec3& eye, const vsg::dvec3& cnt, const vsg::dvec3& up );

private:
    vsg::observer_ptr<vsg::Viewer> _viewer;

    vsg::ref_ptr<vsg::Group> _root{};

    vsg::ref_ptr<BoundingRender> _boundingRender{};

    Navbox* _navbox{};
    Grid* _grid{};
    GizmoUIProxy* _gizmo{};
    Scene* _scene{};
};

}  // namespace tire