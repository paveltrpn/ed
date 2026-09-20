
#pragma once

#include "sceneobjectbase.h"

namespace tire::object {

struct Sphere final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( double radius READ radius WRITE setRadius NOTIFY radiusChanged FINAL )
    Q_PROPERTY( int slices READ slices WRITE setSlices NOTIFY slicesChanged FINAL )
    Q_PROPERTY( int segments READ segments WRITE setSegments NOTIFY segmentsChanged FINAL )
    Q_PROPERTY( double sliceStart READ sliceStart WRITE setSliceStart NOTIFY sliceStartChanged FINAL )
    Q_PROPERTY( double sliceSweep READ sliceSweep WRITE setSliceSweep NOTIFY sliceSweepChanged FINAL )
    Q_PROPERTY( double segmentStart READ segmentStart WRITE setSegmentStart NOTIFY segmentStartChanged FINAL )
    Q_PROPERTY( double segmentSweep READ segmentSweep WRITE setSegmentSweep NOTIFY segmentSweepChanged FINAL )

public:
    Sphere( const SphereObjectData& data );

    Sphere( const Sphere& other ) = delete;
    Sphere( Sphere&& other ) = delete;

    Sphere& operator=( const Sphere& other ) = delete;
    Sphere& operator=( Sphere&& other ) = delete;

    auto radius() const -> double {
        //
        return _radius;
    }

    auto slices() const -> int {
        //
        return _slices;
    }

    auto segments() const -> int {
        //
        return _segments;
    }

    auto sliceStart() const -> double {
        //
        return _sliceStart;
    }

    auto sliceSweep() const -> double {
        //
        return _sliceSweep;
    }

    auto segmentStart() const -> double {
        //
        return _segmentStart;
    }

    auto segmentSweep() const -> double {
        //
        return _segmentSweep;
    }

    auto setRadius( double value ) -> void {
        //
        _radius = value;
    }

    auto setSlices( int value ) -> void {
        //
        _slices = value;
    }

    auto setSegments( int value ) -> void {
        //
        _segments = value;
    }

    auto setSliceStart( double value ) -> void {
        //
        _sliceStart = value;
    }

    auto setSliceSweep( double value ) -> void {
        //
        _sliceSweep = value;
    }

    auto setSegmentStart( double value ) -> void {
        //
        _segmentStart = value;
    }

    auto setSegmentSweep( double value ) -> void {
        //
        _segmentSweep = value;
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
    double _radius{ 1.0 };
    int _slices{ 32 };
    int _segments{ 16 };
    double _sliceStart{ 0.0 };
    double _sliceSweep{ gml::radians( 360.0 ) };
    double _segmentStart{ 0.0 };
    double _segmentSweep{ gml::radians( 180.0 ) };
};

}  // namespace tire::object