
#pragma once

#include <vsg/all.h>

#include "subgraph.h"

namespace tire {

struct TestboxSubgraph;

struct Testbox final : public QObject {
    Q_OBJECT

    Q_PROPERTY( float origin READ gridSize WRITE setSize NOTIFY sizeChanged FINAL )
    Q_PROPERTY( float axis READ gridSize WRITE setSize NOTIFY sizeChanged FINAL )
    Q_PROPERTY( float angl READ gridSize WRITE setSize NOTIFY sizeChanged FINAL )
    Q_PROPERTY( float size READ gridSize WRITE setSize NOTIFY sizeChanged FINAL )

public:
    Testbox( vsg::Viewer* viewer, QObject* parent = nullptr );

    auto node() const -> vsg::ref_ptr<TestboxSubgraph>;

    void setSize( float value );

    float gridSize();

signals:
    void sizeChanged();

private:
    vsg::ref_ptr<TestboxSubgraph> _node{};
};

// ===============================================================================

struct TestboxSubgraph final : Subgraph {
    TestboxSubgraph( vsg::Viewer* viewer );

    auto initPipeline() -> void override;
    auto initDrawCommand() -> void;

    friend Testbox;

private:
    vsg::vec3 _origin{ 0.0f, 0.0f, 0.0f };
    vsg::vec3 _axis{ 0.0f, 0.0f, 0.0f };
    float _angl{ 0.0f };
    float _size{ 1.0f };
};

}  // namespace tire
