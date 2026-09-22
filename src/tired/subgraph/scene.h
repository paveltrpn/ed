
#pragma once

#include "subgraph.h"
#include "objectslist.h"

#include "../vsgcommands/setpolygonmode.h"
#include "../vsgcommands/setcullmode.h"

namespace tire {

// ======================================================================================
// ==================== Scene ===========================================================
// ======================================================================================

enum class ObjectsRenderMode { WIREFRAME, SOLID, SOLIDWIRE };

struct SceneSubgraph;

struct Scene final : public QObject {
    Q_OBJECT

    Q_PROPERTY( ObjectsList* objects READ objects NOTIFY objectsChanged FINAL )
    Q_PROPERTY( QString selectedObjectUid READ selectedObjectUid WRITE setSelectedObjectUid NOTIFY
                    selectedObjectUidChanged FINAL )

    Q_PROPERTY( int renderMode READ renderMode WRITE setRenderMode NOTIFY renderModeChanged FINAL )
public:
    Scene( vsg::Viewer* viewer, QObject* parent = nullptr );

    auto node() const -> vsg::ref_ptr<SceneSubgraph>;

    Q_INVOKABLE void addBox( const BoxObjectData& data );
    Q_INVOKABLE void addSphere( const SphereObjectData& data );
    Q_INVOKABLE void addCylinder( const CylinderObjectData& data );
    Q_INVOKABLE void addCapsule( const CapsuleObjectData& data );

    auto objects() const -> ObjectsList*;

    auto selectedObjectUid() const -> QString;
    auto setSelectedObjectUid( const QString& value ) -> void;

    auto renderMode() const -> int;
    auto setRenderMode( int value ) -> void;

    Q_INVOKABLE SceneObjectBase* findObject( const QString& uid ) const;

signals:
    void objectsChanged();
    void selectedObjectUidChanged();
    void selectedObjectChanged( SceneObjectBase* object );
    void renderModeChanged();

private:
    vsg::ref_ptr<SceneSubgraph> _node{};
    ObjectsList* _objects{};

    QUuid _selectedObjectUid{};

    ObjectsRenderMode _renderMode{ ObjectsRenderMode::SOLID };
};

// ======================================================================================
// ==================== SceneSubgraph ===================================================
// ======================================================================================

struct SceneSubgraph final : Subgraph {
    SceneSubgraph( vsg::Viewer* viewer );

    auto initPipeline() -> void override;
    auto initPipelineOutline() -> void;

    friend Scene;

private:
    vsg::ref_ptr<vsg::RasterizationState> _rasterizationState{};

    vsg::ref_ptr<vsg::SetPolygonMode> _polygonModeCmd{};
    vsg::ref_ptr<vsg::SetLineWidth> _lineWidthCmd{};
    vsg::ref_ptr<vsg::SetCullMode> _setCullModeCmd{};

    vsg::ref_ptr<vsg::GraphicsPipeline> _graphicsPipeline{};
};

}  // namespace tire
