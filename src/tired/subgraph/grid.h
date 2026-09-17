
#pragma once

#include <vsg/all.h>

#include "subgraph.h"

namespace tire {

struct GridSubgraph;

// ======================================================================================
// ==================== Grid ============================================================
// ======================================================================================

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

    auto node() const -> vsg::ref_ptr<GridSubgraph>;

    auto setGridSize( float value ) -> void;
    auto setLineThickness( float value ) -> void;
    auto setMaxRange( float value ) -> void;
    auto setZoomSensitivity( float value ) -> void;

    Q_INVOKABLE void setColorMajor( float r, float g, float b );
    Q_INVOKABLE void setColorMinor( float r, float g, float b );

    auto setMajorDivisor( float value ) -> void;
    auto setGridScale( float value ) -> void;
    auto setGridZOffset( float value ) -> void;

    auto updateCameraPosition( const vsg::vec3& value ) -> void;

    auto gridSize() const -> float;
    auto lineThickness() const -> float;
    auto maxRange() const -> float;
    auto zoomSensitivity() const -> float;
    auto majorDivisor() const -> float;
    auto gridScale() const -> float;
    auto gridZOffset() const -> float;

signals:
    void gridSizeChanged();
    void lineThicknessChanged();
    void maxRangeChanged();
    void zoomSensitivityChanged();

    void majorDivisorChanged();
    void gridScaleChanged();
    void gridZOffsetChanged();

private:
    vsg::ref_ptr<GridSubgraph> _node{};
};

// ======================================================================================
// ==================== GridSubgraph ====================================================
// ======================================================================================

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
    float _gridSize{ 1.0f };
    float _lineThickness{ 0.0256f };
    float _maxRange{ 256.0f };
    float _zoomSensitivity{ 0.5f };

    vsg::vec3 _colorMajor{ 19.0f / 255.0f, 19.0f / 255.0f, 17.0f / 255.0f };
    vsg::vec3 _colorMinor{ 53.0f / 255.0f, 53.0f / 255.0f, 51.0f / 255.0f };

    float _majorDivisor{ 5.0f };
    float _gridScale{ 512.0f };
    float _gridZOffset{ 0.0f };

    vsg::vec3 _cameraPosition{ 0.0f, -16.0f, 8.0f };

    vsg::ref_ptr<vsg::floatArray> _gridBufUniformValue{};
    vsg::ref_ptr<vsg::floatArray> _planeBufUniformValue{};
};

}  // namespace tire
