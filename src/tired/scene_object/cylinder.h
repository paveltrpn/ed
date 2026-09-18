
#pragma once

#include "sceneobjectbase.h"

namespace tire::object {

struct Cylinder final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( double radius READ radius WRITE setRadius NOTIFY radiusChanged FINAL )
    Q_PROPERTY( double height READ height WRITE setHeight NOTIFY heightChanged FINAL )

public:
    Cylinder( QObject* parent = nullptr );

    Cylinder( const CylinderObjectData& data );

    Cylinder( const Cylinder& other ) = delete;
    Cylinder( Cylinder&& other ) = delete;

    Cylinder& operator=( const Cylinder& other ) = delete;
    Cylinder& operator=( Cylinder&& other ) = delete;

    auto radius() const -> double {
        //
        return _radius;
    }

    auto height() const -> double {
        //
        return _height;
    }

    auto setRadius( double value ) -> void {
        //
        _radius = value;
    }

    auto setHeight( double value ) -> void {
        //
        _height = value;
    }

signals:
    void radiusChanged();
    void heightChanged();

private:
    auto init() -> void;

private:
    double _radius{};
    double _height{};
};

}  // namespace tire::object