
#pragma once

#include "subgraph.h"
#include "objectslist.h"

namespace tire {

// ======================================================================================
// ==================== Scene ===========================================================
// ======================================================================================

struct SceneSubgraph;

struct Scene final : public QObject {
    Q_OBJECT

    Q_PROPERTY( ObjectsList* objects READ objects NOTIFY objectsChanged FINAL )

public:
    Scene( vsg::Viewer* viewer, QObject* parent = nullptr );

    auto node() const -> vsg::ref_ptr<SceneSubgraph>;

    Q_INVOKABLE void addBox( float px, float py, float pz, float rx, float ry, float rz, float sx, float sy, float sz );

    auto objects() const -> ObjectsList*;

signals:
    void objectsChanged();

private:
    vsg::ref_ptr<SceneSubgraph> _node{};
    ObjectsList* _objects{};
};

// ======================================================================================
// ==================== SceneSubgraph ===================================================
// ======================================================================================

struct SceneSubgraph final : Subgraph {
    SceneSubgraph( vsg::Viewer* viewer );

    auto initPipeline() -> void override;

private:
};

}  // namespace tire
