
#pragma once

#include "sceneobjectbase.h"

namespace tire::object {

struct Mesh final : public SceneObjectBase {
    Q_OBJECT

public:
    Mesh( QObject* parent = nullptr );

    Mesh( const Mesh& other ) = delete;
    Mesh( Mesh&& other ) = delete;

    Mesh& operator=( const Mesh& other ) = delete;
    Mesh& operator=( Mesh&& other ) = delete;

private:
};

}  // namespace tire::object