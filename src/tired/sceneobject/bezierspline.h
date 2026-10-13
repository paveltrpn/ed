
#pragma once

#include "sceneobjectbase.h"

namespace tire::object {

struct Bezierspline final : public SceneObjectBase {
    Q_OBJECT

public:
    Bezierspline( QObject* parent = nullptr );

    Bezierspline( const Bezierspline& other ) = delete;
    Bezierspline( Bezierspline&& other ) = delete;

    Bezierspline& operator=( const Bezierspline& other ) = delete;
    Bezierspline& operator=( Bezierspline&& other ) = delete;

private:
};

}  // namespace tire::object