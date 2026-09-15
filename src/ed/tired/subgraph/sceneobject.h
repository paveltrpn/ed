
#pragma once

#include "subgraph.h"

namespace tire {

// ======================================================================================
// ==================== SceneObjects ====================================================
// ======================================================================================

struct SceneObjectsSubgraph;

struct SceneObjects final : public QObject {
    Q_OBJECT

    // Q_PROPERTY( QVector3D boxOrigin READ boxOrigin WRITE setBoxOrigin NOTIFY boxOriginChanged FINAL )

public:
    SceneObjects( vsg::Viewer* viewer, QObject* parent = nullptr );

    auto node() const -> vsg::ref_ptr<SceneObjectsSubgraph>;

    // void setBoxOrigin( QVector3D value );

    // QVector3D boxOrigin();

signals:
    // void boxOriginChanged();

private:
    vsg::ref_ptr<SceneObjectsSubgraph> _node{};
};

// ======================================================================================
// ==================== SceneObjectsSubgraph ============================================
// ======================================================================================

struct SceneObjectsSubgraph final : Subgraph {
    SceneObjectsSubgraph( vsg::Viewer* viewer );

    auto initPipeline() -> void override;

private:
};

}  // namespace tire
