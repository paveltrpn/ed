
#pragma once

#include <vsg/all.h>

#include "sceneobjectbase.h"

namespace tire {

struct SphereObject;

// ======================================================================================
// ==================== SphereNode ======================================================
// ======================================================================================

struct SphereNode final : public SceneObjectNode {
    friend SphereObject;

    SphereNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                vsg::vec4 color, double radius, int slices, int segments, double sliceStart, double sliceSweep,
                double segmentStart, double segmentSweep );

private:
    auto init() -> void;

private:
    double _radius{ 0.5 };
    int _slices{ 16 };
    int _segments{ 8 };
    double _sliceStart{ 0.0 };
    double _sliceSweep{ gml::radians( 360.0 ) };
    double _segmentStart{ 0.0 };
    double _segmentSweep{ gml::radians( 180.0 ) };
};

// ======================================================================================
// ==================== SphereObject ====================================================
// ======================================================================================

struct SphereObject final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( double radius READ radius WRITE setRadius NOTIFY radiusChanged FINAL )
    Q_PROPERTY( int slices READ slices WRITE setSlices NOTIFY slicesChanged FINAL )
    Q_PROPERTY( int segments READ segments WRITE setSegments NOTIFY segmentsChanged FINAL )
    Q_PROPERTY( double sliceStart READ sliceStart WRITE setSliceStart NOTIFY sliceStartChanged FINAL )
    Q_PROPERTY( double sliceSweep READ sliceSweep WRITE setSliceSweep NOTIFY sliceSweepChanged FINAL )
    Q_PROPERTY( double segmentStart READ segmentStart WRITE setSegmentStart NOTIFY segmentStartChanged FINAL )
    Q_PROPERTY( double segmentSweep READ segmentSweep WRITE setSegmentSweep NOTIFY segmentSweepChanged FINAL )

public:
    SphereObject( vsg::ref_ptr<SphereNode> node );

    SphereObject( const SphereObject& other ) = delete;
    SphereObject( SphereObject&& other ) = delete;

    SphereObject& operator=( const SphereObject& other ) = delete;
    SphereObject& operator=( SphereObject&& other ) = delete;

    auto radius() const -> double {
        //
        return _node->_radius;
    }

    auto slices() const -> int {
        //
        return _node->_slices;
    }

    auto segments() const -> int {
        //
        return _node->_segments;
    }

    auto sliceStart() const -> double {
        //
        return _node->_sliceStart;
    }

    auto sliceSweep() const -> double {
        //
        return _node->_sliceSweep;
    }

    auto segmentStart() const -> double {
        //
        return _node->_segmentStart;
    }

    auto segmentSweep() const -> double {
        //
        return _node->_segmentSweep;
    }

    auto setRadius( double value ) -> void {
        //
        if ( _node->_radius == value ) return;
        _node->_radius = value;
        emit radiusChanged();
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

    auto setSliceStart( double value ) -> void {
        //
        if ( _node->_sliceStart == value ) return;
        _node->_sliceStart = value;
        emit sliceStartChanged();
    }

    auto setSliceSweep( double value ) -> void {
        //
        if ( _node->_sliceSweep == value ) return;
        _node->_sliceSweep = value;
        emit sliceSweepChanged();
    }

    auto setSegmentStart( double value ) -> void {
        //
        if ( _node->_segmentStart == value ) return;
        _node->_segmentStart = value;
        emit segmentStartChanged();
    }

    auto setSegmentSweep( double value ) -> void {
        //
        if ( _node->_segmentSweep == value ) return;
        _node->_segmentSweep = value;
        emit segmentSweepChanged();
    }

    [[nodiscard]] virtual auto node() const -> vsg::ref_ptr<SceneObjectNode> {
        //
        return _node;
    }

signals:
    void radiusChanged();
    void slicesChanged();
    void segmentsChanged();
    void sliceStartChanged();
    void sliceSweepChanged();
    void segmentStartChanged();
    void segmentSweepChanged();

private:
    auto init() -> void;

private:
    vsg::ref_ptr<SphereNode> _node{};
};

}  // namespace tire
