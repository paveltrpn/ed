
#pragma once

#include <vsg/all.h>

#include "subgraph.h"

namespace tire {

struct GridSubgraph;

struct Grid final : public QObject {
    Q_OBJECT

    Q_PROPERTY( float gridSize READ gridSize WRITE setGridSize NOTIFY gridSizeChanged FINAL )
    Q_PROPERTY( float lineThickness READ lineThickness WRITE setLineThickness NOTIFY lineThicknessChanged FINAL )
    Q_PROPERTY( float maxRange READ maxRange WRITE setMaxRange NOTIFY maxRangeChanged FINAL )
    Q_PROPERTY(
        float zoomSensitivity READ zoomSensitivity WRITE setZoomSensitivity NOTIFY zoomSensitivityChanged FINAL )

    Q_PROPERTY( float majorDivisor READ majorDivisor WRITE setMajorDivisor NOTIFY majorDivisorChanged FINAL )
    Q_PROPERTY( float gridScale READ gridScale WRITE setGridScale NOTIFY gridScaleChanged FINAL )
    Q_PROPERTY( float gridZOffset READ gridZOffset WRITE setGridZOffset NOTIFY gridZOffsetChanged FINAL )

public:
    Grid( vsg::Viewer* viewer, QObject* parent = nullptr );

    auto grid() const -> vsg::ref_ptr<GridSubgraph>;

    void setGridSize( float value );
    void setLineThickness( float value );
    void setMaxRange( float value );
    void setZoomSensitivity( float value );

    Q_INVOKABLE void setColorMajor( float r, float g, float b );
    Q_INVOKABLE void setColorMinor( float r, float g, float b );

    void setMajorDivisor( float value );
    void setGridScale( float value );
    void setGridZOffset( float value );

    float gridSize();
    float lineThickness();
    float maxRange();
    float zoomSensitivity();
    float majorDivisor();
    float gridScale();
    float gridZOffset();

signals:
    void gridSizeChanged();
    void lineThicknessChanged();
    void maxRangeChanged();
    void zoomSensitivityChanged();

    void majorDivisorChanged();
    void gridScaleChanged();
    void gridZOffsetChanged();

private:
    vsg::ref_ptr<GridSubgraph> _grid{};
};

// ===============================================================================

struct GridSubgraph final : public Subgraph {
    GridSubgraph( vsg::Viewer* viewer );

    auto initPipeline() -> void override;
    auto initDrawCommand() -> void;

    auto recompile() -> void override;

    friend Grid;

private:
    auto updateGridBufUniformValue() -> void;
    auto updatePlaneBufUniformValue() -> void;

private:
    float _gridSize{ 0.8f };
    float _lineThickness{ 0.015f };
    float _maxRange{ 100.0f };
    float _zoomSensitivity{ 0.05f };

    vsg::vec3 _colorMajor{ 0.7f, 0.2f, 0.2f };
    vsg::vec3 _colorMinor{ 0.2f, 0.2f, 0.5f };

    float _majorDivisor{ 5.0f };
    float _gridScale{ 10.0 };
    float _gridZOffset{ 0.0 };

    vsg::ref_ptr<vsg::floatArray> _gridBufUniformValue{};
    vsg::ref_ptr<vsg::floatArray> _planeBufUniformValue{};
};

}  // namespace tire
