
#pragma once

#include "sceneobjectbase.h"

namespace tire::object {

struct Cone final : public SceneObjectBase {
    Q_OBJECT

public:
    Cone( const ConeObjectData& data );

    Cone( const Cone& other ) = delete;
    Cone( Cone&& other ) = delete;

    Cone& operator=( const Cone& other ) = delete;
    Cone& operator=( Cone&& other ) = delete;

private:
    auto init() -> void;

private:
    ConeObjectData _data{};
};

}  // namespace tire::object
