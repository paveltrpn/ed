
#pragma once

#include <QVector3D>

#include <vsg/all.h>

#include "subgraph.h"

namespace tire {

struct TestboxSubgraph;

// ======================================================================================
// ==================== Testbox =========================================================
// ======================================================================================

struct Testbox final : public QObject {
    Q_OBJECT

    Q_PROPERTY( QVector3D origin READ origin WRITE setOrigin NOTIFY originChanged FINAL )
    Q_PROPERTY( QVector3D axis READ axis WRITE setAxis NOTIFY axisChanged FINAL )
    Q_PROPERTY( float angl READ angl WRITE setAngl NOTIFY anglChanged FINAL )
    Q_PROPERTY( float size READ size WRITE setSize NOTIFY sizeChanged FINAL )
    Q_PROPERTY( QVector3D color READ color WRITE setColor NOTIFY colorChanged FINAL )

public:
    Testbox( vsg::Viewer* viewer, QObject* parent = nullptr );

    auto node() const -> vsg::ref_ptr<TestboxSubgraph>;

    void setOrigin( QVector3D value );
    void setAxis( QVector3D value );
    void setAngl( float value );
    void setSize( float value );
    void setColor( QVector3D value );

    QVector3D origin();
    QVector3D axis();
    float angl();
    float size();
    QVector3D color();

signals:
    void originChanged();
    void axisChanged();
    void anglChanged();
    void sizeChanged();
    void colorChanged();

private:
    vsg::ref_ptr<TestboxSubgraph> _node{};
};

// ======================================================================================
// ==================== TestboxSubgraph =================================================
// ======================================================================================

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

    vsg::vec3 _color{ 0.0f, 0.0f, 0.0f };
};

}  // namespace tire
