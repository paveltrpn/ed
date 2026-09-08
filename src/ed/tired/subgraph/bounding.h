

#pragma once

#include <vsg/all.h>

#include "subgraph.h"

namespace tire {

// ======================================================================================
// ==================== Bounding ========================================================
// ======================================================================================

struct BoundingSubgraph;

struct Bounding final : public QObject {
    Q_OBJECT

    Q_PROPERTY( float scale READ scale WRITE setScale NOTIFY scaleChanged FINAL )
    Q_PROPERTY( float lineLength READ lineLength WRITE setLineLength NOTIFY lineLengthChanged FINAL )

public:
    Bounding( vsg::Viewer* viewer, QObject* parent = nullptr );

    auto node() const -> vsg::ref_ptr<BoundingSubgraph>;

    void setScale( float value );
    void setLineLength( float value );

    float scale();
    float lineLength();

    void setTransformMat( vsg::mat4 value );

signals:
    void scaleChanged();
    void lineLengthChanged();

private:
    vsg::ref_ptr<BoundingSubgraph> _node{};
};

// ======================================================================================
// ==================== BoundingSubgraph ================================================
// ======================================================================================

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

    float _scale{ 1.0f };
    float _lineLength{ 0.25f };

    vsg::ref_ptr<vsg::mat4Value> _transformMatUniform{};
};

}  // namespace tire