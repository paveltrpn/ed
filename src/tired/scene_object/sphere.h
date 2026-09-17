
#pragma once

#include "sceneobjectbase.h"

namespace tire::object {

struct Sphere final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( double radius READ radius WRITE setRadius NOTIFY radiusChanged FINAL )

public:
    Sphere( QObject* parent = nullptr );

    Sphere( const Sphere& other ) = delete;
    Sphere( Sphere&& other ) = delete;

    Sphere& operator=( const Sphere& other ) = delete;
    Sphere& operator=( Sphere&& other ) = delete;

    auto radius() const -> double {
        //
        return _radius;
    }

    auto setRadius( double value ) -> void {
        //
        _radius = value;
    }

signals:
    void radiusChanged();

private:
    double _radius{};
};

}  // namespace tire::object