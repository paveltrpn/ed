
#pragma once

#include "sceneobjectbase.h"

namespace tire::object {

struct Polytope final : public SceneObjectBase {
    Q_OBJECT

public:
    Polytope( QObject* parent = nullptr );

    Polytope( const Polytope& other ) = delete;
    Polytope( Polytope&& other ) = delete;

    Polytope& operator=( const Polytope& other ) = delete;
    Polytope& operator=( Polytope&& other ) = delete;

private:
};

}  // namespace tire::object