
#pragma once

#include <QObject>

#include <vsg/all.h>

#include "subgraph/grid.h"
#include "subgraph/testbox.h"
#include "subgraph/sceneobject.h"
#include "subgraph/marker.h"
#include "subgraph/bounding.h"
#include "subgraph/gizmo.h"

namespace tire {

struct Scenegraph final : public QObject {
    Q_OBJECT

    Q_PROPERTY( QObject* testbox READ testbox NOTIFY testboxChanged FINAL )
    Q_PROPERTY( QObject* grid READ grid NOTIFY gridChanged FINAL )
    Q_PROPERTY( QObject* bounding READ bounding NOTIFY boundingChanged FINAL )

public:
    Scenegraph( vsg::Viewer* viewer, QObject* parent = nullptr );

    Q_INVOKABLE void addExBox( float px, float py, float pz, float rx, float ry, float rz, float sx, float sy,
                               float sz );

    auto initSubgraphs() -> void;

    auto root() const -> vsg::ref_ptr<vsg::Group>;

    Testbox* testbox() const;
    Grid* grid() const;
    Bounding* bounding() const;

signals:
    void testboxChanged();
    void gridChanged();
    void boundingChanged();

private:
    vsg::ref_ptr<vsg::Group> _root{};
    vsg::Viewer* _viewer;

    Testbox* _testbox{};
    Grid* _grid{};
    Bounding* _bounding{};

    Gizmo* _gizmo{};

    vsg::ref_ptr<SceneObjectSubgraph> _sceneObjectSubgraph{};
    std::vector<std::shared_ptr<SceneObjectBase>> _objectsList{};

    vsg::ref_ptr<MarkerSubgraph> _markerSubgraph{};
};

}  // namespace tire