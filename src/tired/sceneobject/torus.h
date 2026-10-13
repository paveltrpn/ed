#pragma once

#include "sceneobjectbase.h"

namespace tire::object {

struct Torus final : public SceneObjectBase {
    Q_OBJECT

public:
    Torus( const TorusObjectData& data );

    Torus( const Torus& other ) = delete;
    Torus( Torus&& other ) = delete;

    Torus& operator=( const Torus& other ) = delete;
    Torus& operator=( Torus&& other ) = delete;

private:
    auto init() -> void;

private:
    TorusObjectData _data{};
};

}  // namespace tire::object
