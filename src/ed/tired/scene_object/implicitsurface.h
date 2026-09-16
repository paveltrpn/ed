
#pragma once

#include "sceneobjectbase.h"

namespace tire::object {

struct ImplicitSurface final : public SceneObjectBase {
    Q_OBJECT

public:
    ImplicitSurface( QObject* parent = nullptr );

    ImplicitSurface( const ImplicitSurface& other ) = delete;
    ImplicitSurface( ImplicitSurface&& other ) = delete;

    ImplicitSurface& operator=( const ImplicitSurface& other ) = delete;
    ImplicitSurface& operator=( ImplicitSurface&& other ) = delete;

private:
};

}  // namespace tire::object