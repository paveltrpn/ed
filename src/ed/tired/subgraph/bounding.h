

#pragma once

#include <vsg/all.h>

#include "subgraph.h"

namespace tire {

struct BoundingSubgraph;

struct Bounding final : public QObject {
    Q_OBJECT

public:
    Bounding( vsg::Viewer* viewer, QObject* parent = nullptr );

    auto bounding() const -> vsg::ref_ptr<BoundingSubgraph>;

    void setTransformMat( vsg::mat4 value );

private:
    vsg::ref_ptr<BoundingSubgraph> _bounding{};
};

// ===============================================================================

struct BoundingSubgraph final : public Subgraph {
    BoundingSubgraph( vsg::Viewer* viewer );

    auto initPipeline() -> void override;
    auto initDrawCommand() -> void;

    friend Bounding;

private:
    auto updateTransformMatUniform() -> void;

private:
    vsg::mat4 _transformMat{};
    vsg::mat4 _initialScale{ vsg::scale<float>( 1.1f, 1.1f, 1.1f ) };
    vsg::ref_ptr<vsg::mat4Value> _transformMatUniform{};
};

}  // namespace tire