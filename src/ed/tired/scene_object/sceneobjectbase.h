
#pragma once

#include <print>
#include <QObject>

#include <vsg/all.h>

#include "sceneobjectgraph.h"
#include "sceneobjectdata.h"

namespace tire {

struct SceneObjectBase : public QObject {
    Q_OBJECT

public:
    SceneObjectBase( QObject* parent = nullptr );

    SceneObjectBase( const SceneObjectBase& other ) = delete;
    SceneObjectBase( SceneObjectBase&& other ) = delete;

    SceneObjectBase& operator=( const SceneObjectBase& other ) = delete;
    SceneObjectBase& operator=( SceneObjectBase&& other ) = delete;

    virtual auto data() const -> SceneObjectData const& = 0;
    virtual auto data() -> SceneObjectData& = 0;

    auto root() const -> vsg::ref_ptr<SceneObjectGraph>;

signals:
    void dataChanged();

protected:
    vsg::ref_ptr<SceneObjectGraph> _thisObject{};

private:
    bool _selected{ false };
};

}  // namespace tire