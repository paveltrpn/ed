
#pragma once

#include <print>
#include <QObject>

#include <vsg/all.h>

#include "sceneobjectgraph.h"
#include "sceneobjectdata.h"

namespace tire {

struct SceneObjectBase : public QObject {
    Q_OBJECT

    Q_PROPERTY( bool selected READ selected WRITE setSelected NOTIFY selectedChanged FINAL )

public:
    SceneObjectBase( QObject* parent = nullptr );

    SceneObjectBase( const SceneObjectBase& other ) = delete;
    SceneObjectBase( SceneObjectBase&& other ) = delete;

    SceneObjectBase& operator=( const SceneObjectBase& other ) = delete;
    SceneObjectBase& operator=( SceneObjectBase&& other ) = delete;

    Q_INVOKABLE void setSelected( bool value );
    Q_INVOKABLE bool selected();

    virtual auto data() const -> SceneObjectData const& = 0;
    virtual auto data() -> SceneObjectData& = 0;

    auto root() const -> vsg::ref_ptr<SceneObjectGraph>;

    auto test() const -> void { std::println( "==== print for test ====" ); }

signals:
    void dataChanged();
    void selectedChanged();

protected:
    vsg::ref_ptr<SceneObjectGraph> _thisObject{};

private:
    bool _selected{ false };
};

}  // namespace tire