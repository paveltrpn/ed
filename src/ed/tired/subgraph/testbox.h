
#pragma once

#include <vsg/all.h>

#include "subgraph.h"

namespace tire {

struct TestboxSubgraph;

struct Testbox final : public QObject {
    Q_OBJECT

    Q_PROPERTY( float size READ gridSize WRITE setSize NOTIFY sizeChanged FINAL )

public:
    Testbox( vsg::Viewer* viewer, QObject* parent = nullptr );

    auto testbox() const -> vsg::ref_ptr<TestboxSubgraph>;

    void setSize( float value );

    float gridSize();

signals:
    void sizeChanged();

private:
    vsg::ref_ptr<TestboxSubgraph> _testbox{};
};

// ===============================================================================

struct TestboxSubgraph final : Subgraph {
    TestboxSubgraph( vsg::Viewer* viewer );

    auto initPipeline() -> void override;
    auto initDrawCommand() -> void;

    friend Testbox;

private:
    float _size{ 1.0f };
};

}  // namespace tire
