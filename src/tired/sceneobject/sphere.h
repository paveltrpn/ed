
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
        return _data._radius;
    }

    auto slices() const -> int {
        //
        return _data._slices;
    }

    auto segments() const -> int {
        //
        return _data._segments;
    }

    auto sliceStart() const -> double {
        //
        return _data._sliceStart;
    }

    auto sliceSweep() const -> double {
        //
        return _data._sliceSweep;
    }

    auto segmentStart() const -> double {
        //
        return _data._segmentStart;
    }

    auto segmentSweep() const -> double {
        //
        return _data._segmentSweep;
    }

    auto setRadius( double value ) -> void {
        //
        _data._radius = value;
    }

    auto setSlices( int value ) -> void {
        //
        _data._slices = value;
    }

    auto setSegments( int value ) -> void {
        //
        _data._segments = value;
    }

    auto setSliceStart( double value ) -> void {
        //
        _data._sliceStart = value;
    }

    auto setSliceSweep( double value ) -> void {
        //
        _data._sliceSweep = value;
    }

    auto setSegmentStart( double value ) -> void {
        //
        _data._segmentStart = value;
    }

    auto setSegmentSweep( double value ) -> void {
        //
        _data._segmentSweep = value;
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
    SphereObjectData _data{};
};

}  // namespace tire::object