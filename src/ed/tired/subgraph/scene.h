
#pragma once

#include "subgraph.h"

namespace tire {

// ======================================================================================
// ==================== Scene ===========================================================
// ======================================================================================

struct SceneSubgraph;

struct Scene final : public QObject {
    Q_OBJECT

    // Q_PROPERTY( QVector3D boxOrigin READ boxOrigin WRITE setBoxOrigin NOTIFY boxOriginChanged FINAL )

public:
    Scene( vsg::Viewer* viewer, QObject* parent = nullptr );

    auto node() const -> vsg::ref_ptr<SceneSubgraph>;

    // void setBoxOrigin( QVector3D value );

    // QVector3D boxOrigin();

signals:
    // void boxOriginChanged();

private:
    vsg::ref_ptr<SceneSubgraph> _node{};
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
