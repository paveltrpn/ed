
#pragma once

#include <vsg/all.h>

#include "subgraph.h"

namespace tire {

struct GizmoSubgraph;

struct Gizmo final : public QObject {
    Q_OBJECT

public:
    Gizmo( const QObject* parent = nullptr );
};

// ===============================================================================

struct GizmoSubgraph final : public Subgraph {
    GizmoSubgraph( vsg::Viewer* viewer );

    auto initPipeline() -> void override;

private:
};

}  // namespace tire