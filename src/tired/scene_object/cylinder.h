
#pragma once

#include "sceneobjectbase.h"

namespace tire::object {

struct Cylinder final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( double radius READ radius WRITE setRadius NOTIFY radiusChanged FINAL )
    Q_PROPERTY( double size READ size WRITE setSize NOTIFY sizeChanged FINAL )
    Q_PROPERTY( int slices READ slices WRITE setSlices NOTIFY slicesChanged FINAL )
    Q_PROPERTY( int segments READ segments WRITE setSegments NOTIFY segmentsChanged FINAL )
    Q_PROPERTY( int rings READ rings WRITE setRings NOTIFY ringsChanged FINAL )
    Q_PROPERTY( double start READ start WRITE setStart NOTIFY startChanged FINAL )
    Q_PROPERTY( double sweep READ sweep WRITE setSweep NOTIFY sweepChanged FINAL )

public:
    Cylinder( const CylinderObjectData& data );

    Cylinder( const Cylinder& other ) = delete;
    Cylinder( Cylinder&& other ) = delete;

    Cylinder& operator=( const Cylinder& other ) = delete;
    Cylinder& operator=( Cylinder&& other ) = delete;

    auto radius() const -> double {
        //
        return _data._radius;
    }

    auto size() const -> double {
        //
        return _data._size;
    }

    auto slices() const -> int {
        //
        return _data._slices;
    }

    auto segments() const -> int {
        //
        return _data._segments;
    }

    auto rings() const -> int {
        //
        return _data._rings;
    }

    auto start() const -> double {
        //
        return _data._start;
    }

    auto sweep() const -> double {
        //
        return _data._sweep;
    }

    auto setRadius( double value ) -> void {
        //
        _data._radius = value;
    }

    auto setSize( double value ) -> void {
        //
        _data._size = value;
    }

    auto setSlices( int value ) -> void {
        //
        _data._slices = value;
    }

    auto setSegments( int value ) -> void {
        //
        _data._segments = value;
    }

    auto setRings( int value ) -> void {
        //
        _data._rings = value;
    }

    auto setStart( double value ) -> void {
        //
        _data._start = value;
    }

    auto setSweep( double value ) -> void {
        //
        _data._sweep = value;
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
    CylinderObjectData _data{};
};

}  // namespace tire::object