
#pragma once

#include "subgraph.h"
#include "objectslist.h"

namespace tire {

// ======================================================================================
// ==================== Scene ===========================================================
// ======================================================================================

struct SceneSubgraph;

struct Scene final : public QObject {
    Q_OBJECT

    Q_PROPERTY( ObjectsList* objects READ objects NOTIFY objectsChanged FINAL )
    Q_PROPERTY( QString selectedObjectUid READ selectedObjectUid WRITE setSelectedObjectUid NOTIFY
                    selectedObjectUidChanged FINAL )

public:
    Scene( vsg::Viewer* viewer, QObject* parent = nullptr );

    auto node() const -> vsg::ref_ptr<SceneSubgraph>;

    Q_INVOKABLE void addBox( float px, float py, float pz, float rx, float ry, float rz, float sx, float sy, float sz );
    Q_INVOKABLE void addSphere( float px, float py, float pz, float rx, float ry, float rz, float sx, float sy,
                                float sz );
    Q_INVOKABLE void addCylinder( float px, float py, float pz, float rx, float ry, float rz, float sx, float sy,
                                  float sz );
    Q_INVOKABLE void addCapsule( float px, float py, float pz, float rx, float ry, float rz, float sx, float sy,
                                 float sz );

    auto objects() const -> ObjectsList*;

    auto selectedObjectUid() const -> QString;
    auto setSelectedObjectUid( const QString& value ) -> void;

    Q_INVOKABLE SceneObjectBase* findObject( const QString& uid ) const;

signals:
    void objectsChanged();
    void selectedObjectUidChanged();
    void selectedObjectChanged( SceneObjectBase* object );

private:
    vsg::ref_ptr<SceneSubgraph> _node{};
    ObjectsList* _objects{};

    QUuid _selectedObjectUid{};
};

// ======================================================================================
// ==================== SceneSubgraph ===================================================
// ======================================================================================

struct SceneSubgraph final : Subgraph {
    SceneSubgraph( vsg::Viewer* viewer );

    auto initPipeline() -> void override;

private:
};

}  // namespace tire
