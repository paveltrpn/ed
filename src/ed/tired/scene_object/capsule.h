
#pragma once

#include "sceneobjectbase.h"

namespace tire::object {

struct Capsule final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( double radius READ radius WRITE setRadius NOTIFY radiusChanged FINAL )
    Q_PROPERTY( double height READ height WRITE setHeight NOTIFY heightChanged FINAL )

public:
    Capsule( QObject* parent = nullptr );

    Capsule( const Capsule& other ) = delete;
    Capsule( Capsule&& other ) = delete;

    Capsule& operator=( const Capsule& other ) = delete;
    Capsule& operator=( Capsule&& other ) = delete;

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
    double _radius{};
    double _height{};
};

}  // namespace tire::object