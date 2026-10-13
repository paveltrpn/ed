
#pragma once

#include <vsg/all.h>

#include "sceneobjectbase.h"

namespace tire {

struct CylinderObject;

// ======================================================================================
// ==================== CylinderNode ====================================================
// ======================================================================================

struct CylinderNode final : public SceneObjectNode {
    friend CylinderObject;

    CylinderNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                  vsg::dvec3 scale, vsg::vec4 color, double radius, double size, int slices, int segments,
                  int rings, double start, double sweep );

private:
    auto init() -> void;

private:
    double _radius{ 0.5 };
    double _size{ 0.5 };
    int _slices{ 16 };
    int _segments{ 8 };
    int _rings{ 4 };
    double _start{ 0.0 };
    double _sweep{ gml::radians( 360.0 ) };
};

// ======================================================================================
// ==================== CylinderObject ==================================================
// ======================================================================================

struct CylinderObject final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( double radius READ radius WRITE setRadius NOTIFY radiusChanged FINAL )
    Q_PROPERTY( double size READ size WRITE setSize NOTIFY sizeChanged FINAL )
    Q_PROPERTY( int slices READ slices WRITE setSlices NOTIFY slicesChanged FINAL )
    Q_PROPERTY( int segments READ segments WRITE setSegments NOTIFY segmentsChanged FINAL )
    Q_PROPERTY( int rings READ rings WRITE setRings NOTIFY ringsChanged FINAL )
    Q_PROPERTY( double start READ start WRITE setStart NOTIFY startChanged FINAL )
    Q_PROPERTY( double sweep READ sweep WRITE setSweep NOTIFY sweepChanged FINAL )

public:
    CylinderObject( vsg::ref_ptr<CylinderNode> node );

    CylinderObject( const CylinderObject& other ) = delete;
    CylinderObject( CylinderObject&& other ) = delete;

    CylinderObject& operator=( const CylinderObject& other ) = delete;
    CylinderObject& operator=( CylinderObject&& other ) = delete;

    auto radius() const -> double {
        //
        return _node->_radius;
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

    auto rings() const -> int {
        //
        return _node->_rings;
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

    auto setRings( int value ) -> void {
        //
        if ( _node->_rings == value ) return;
        _node->_rings = value;
        emit ringsChanged();
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
    void sizeChanged();
    void slicesChanged();
    void segmentsChanged();
    void ringsChanged();
    void startChanged();
    void sweepChanged();

private:
    auto init() -> void;

private:
    vsg::ref_ptr<CylinderNode> _node{};
};

}  // namespace tire
