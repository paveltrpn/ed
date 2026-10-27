
#pragma once

#include <vsg/all.h>

#include "sceneobjectbase.h"

namespace tire {

struct TubeObject;

// ======================================================================================
// ==================== TubeNode ========================================================
// ======================================================================================

struct TubeNode final : public SceneObjectNode {
    friend TubeObject;
    friend JsonSerializationVisitor;

    TubeNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
              vsg::vec4 color, double radius, double innerRadius, double size, int slices, int segments, double start,
              double sweep );

    auto serialize( SerializationVisitor *visitor ) -> void override {
        //
        visitor->serialize( this );
    }

private:
    auto init() -> void;

private:
    double _radius{ 0.5 };
    double _innerRadius{ 0.25 };
    double _size{ 0.5 };
    int _slices{ 16 };
    int _segments{ 8 };
    double _start{ 0.0 };
    double _sweep{ algebra::degToRad( 360.0 ) };
};

// ======================================================================================
// ==================== TubeObject ======================================================
// ======================================================================================

struct TubeObject final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( double radius READ radius WRITE setRadius NOTIFY radiusChanged FINAL )
    Q_PROPERTY( double innerRadius READ innerRadius WRITE setInnerRadius NOTIFY innerRadiusChanged FINAL )
    Q_PROPERTY( double size READ size WRITE setSize NOTIFY sizeChanged FINAL )
    Q_PROPERTY( int slices READ slices WRITE setSlices NOTIFY slicesChanged FINAL )
    Q_PROPERTY( int segments READ segments WRITE setSegments NOTIFY segmentsChanged FINAL )
    Q_PROPERTY( double start READ start WRITE setStart NOTIFY startChanged FINAL )
    Q_PROPERTY( double sweep READ sweep WRITE setSweep NOTIFY sweepChanged FINAL )

public:
    TubeObject( vsg::ref_ptr<TubeNode> node );

    TubeObject( const TubeObject& other ) = delete;
    TubeObject( TubeObject&& other ) = delete;

    TubeObject& operator=( const TubeObject& other ) = delete;
    TubeObject& operator=( TubeObject&& other ) = delete;

    auto radius() const -> double {
        //
        return _node->_radius;
    }

    auto innerRadius() const -> double {
        //
        return _node->_innerRadius;
    }

    auto size() const -> double {
        //
        return _node->_size;
    }

    auto slices() const -> int {
        //
        return _node->_slices;
    }

    auto segments() const -> int {
        //
        return _node->_segments;
    }

    auto start() const -> double {
        //
        return _node->_start;
    }

    auto sweep() const -> double {
        //
        return _node->_sweep;
    }

    auto setRadius( double value ) -> void {
        //
        if ( _node->_radius == value ) return;
        _node->_radius = value;
        emit radiusChanged();
    }

    auto setInnerRadius( double value ) -> void {
        //
        if ( _node->_innerRadius == value ) return;
        _node->_innerRadius = value;
        emit innerRadiusChanged();
    }

    auto setSize( double value ) -> void {
        //
        if ( _node->_size == value ) return;
        _node->_size = value;
        emit sizeChanged();
    }

    auto setSlices( int value ) -> void {
        //
        if ( _node->_slices == value ) return;
        _node->_slices = value;
        emit slicesChanged();
    }

    auto setSegments( int value ) -> void {
        //
        if ( _node->_segments == value ) return;
        _node->_segments = value;
        emit segmentsChanged();
    }

    auto setStart( double value ) -> void {
        //
        if ( _node->_start == value ) return;
        _node->_start = value;
        emit startChanged();
    }

    auto setSweep( double value ) -> void {
        //
        if ( _node->_sweep == value ) return;
        _node->_sweep = value;
        emit sweepChanged();
    }

    [[nodiscard]] virtual auto node() const -> vsg::ref_ptr<SceneObjectNode> {
        //
        return _node;
    }

signals:
    void radiusChanged();
    void innerRadiusChanged();
    void sizeChanged();
    void slicesChanged();
    void segmentsChanged();
    void startChanged();
    void sweepChanged();

private:
    auto init() -> void;

private:
    vsg::ref_ptr<TubeNode> _node{};
};

}  // namespace tire
