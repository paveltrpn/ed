
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

    Q_PROPERTY( QVector3D boxOrigin READ boxOrigin WRITE setBoxOrigin NOTIFY boxOriginChanged FINAL )
    Q_PROPERTY( QVector3D boxAxis READ boxAxis WRITE setBoxAxis NOTIFY boxAxisChanged FINAL )
    Q_PROPERTY( float boxAngl READ boxAngl WRITE setBoxAngl NOTIFY boxAnglChanged FINAL )
    Q_PROPERTY( float boxSize READ boxSize WRITE setBoxSize NOTIFY boxSizeChanged FINAL )
    Q_PROPERTY( QVector3D boxColor READ boxColor WRITE setBoxColor NOTIFY boxColorChanged FINAL )

    Q_PROPERTY( QVector3D lightOrigin READ lightOrigin WRITE setLightOrigin NOTIFY lightOriginChanged FINAL )
    Q_PROPERTY( QVector3D lightColor READ lightColor WRITE setLightColor NOTIFY lightColorChanged FINAL )

public:
    Testbox( vsg::Viewer* viewer, QObject* parent = nullptr );

    auto node() const -> vsg::ref_ptr<TestboxSubgraph>;

    void setBoxOrigin( QVector3D value );
    void setBoxAxis( QVector3D value );
    void setBoxAngl( float value );
    void setBoxSize( float value );
    void setBoxColor( QVector3D value );

    void setLightOrigin( QVector3D value );
    void setLightColor( QVector3D value );

    QVector3D boxOrigin();
    QVector3D boxAxis();
    float boxAngl();
    float boxSize();
    QVector3D boxColor();

    QVector3D lightOrigin();
    QVector3D lightColor();

signals:
    void boxOriginChanged();
    void boxAxisChanged();
    void boxAnglChanged();
    void boxSizeChanged();
    void boxColorChanged();

    void lightOriginChanged();
    void lightColorChanged();

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
    auto updateBoxUniformValue() -> void;
    auto updateLightBufUniformValue() -> void;

private:
    vsg::vec3 _boxOrigin{ 0.0f, 0.0f, 0.0f };
    float _boxSize{ 1.0f };
    vsg::vec3 _boxAxis{ 0.0f, 0.0f, 0.0f };
    float _boxAngl{ 0.0f };
    vsg::vec3 _boxColor{ 0.0f, 0.0f, 0.0f };

    vsg::vec3 _lightOrigin{ 0.0f, 0.0f, 0.0f };
    vsg::vec3 _lightColor{ 0.0f, 0.0f, 0.0f };

    vsg::ref_ptr<vsg::floatArray> _boxUniformValue{};
    vsg::ref_ptr<vsg::floatArray> _lightUniformValue{};
};

}  // namespace tire
