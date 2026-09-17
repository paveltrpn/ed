
#pragma once

#include "sceneobjectbase.h"

namespace tire::object {

struct Beziersurface final : public SceneObjectBase {
    Q_OBJECT

public:
    Beziersurface( QObject* parent = nullptr );

    Beziersurface( const Beziersurface& other ) = delete;
    Beziersurface( Beziersurface&& other ) = delete;

    Beziersurface& operator=( const Beziersurface& other ) = delete;
    Beziersurface& operator=( Beziersurface&& other ) = delete;

private:
};

}  // namespace tire::object