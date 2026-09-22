
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
enum class ObjectsAppearenceMode { COLOR, TEXTURE };
enum class ObjectsLightMode { NONE, CONSTANT, INSCENE };

struct SceneSubgraph;

struct Scene final : public QObject {
    Q_OBJECT

    Q_PROPERTY( ObjectsList* objects READ objects NOTIFY objectsChanged FINAL )
    Q_PROPERTY( QString selectedObjectUid READ selectedObjectUid WRITE setSelectedObjectUid NOTIFY
                    selectedObjectUidChanged FINAL )

    Q_PROPERTY( int renderMode READ renderMode WRITE setRenderMode NOTIFY renderModeChanged FINAL )
    Q_PROPERTY( int appearnceMode READ appearnceMode WRITE setAppearnceMode NOTIFY appearnceModeChanged FINAL )
    Q_PROPERTY( int lightMode READ lightMode WRITE setLightMode NOTIFY lightModeChanged FINAL )

    Q_PROPERTY( int showOuline READ showOuline WRITE setShowOuline NOTIFY showOulineChanged FINAL )

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

    auto appearnceMode() const -> int;
    auto setAppearnceMode( int value ) -> void;

    auto lightMode() const -> int;
    auto setLightMode( int value ) -> void;

    auto showOuline() const -> bool;
    auto setShowOuline( bool value ) -> void;

    Q_INVOKABLE SceneObjectBase* findObject( const QString& uid ) const;

signals:
    void objectsChanged();
    void selectedObjectUidChanged();
    void selectedObjectChanged( SceneObjectBase* object );
    void renderModeChanged();
    void appearnceModeChanged();
    void lightModeChanged();
    void showOulineChanged();

private:
    vsg::ref_ptr<SceneSubgraph> _node{};
    ObjectsList* _objects{};

    QUuid _selectedObjectUid{};

    ObjectsRenderMode _renderMode{ ObjectsRenderMode::SOLID };

    bool _showOuline{ false };
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
    auto updateObjectParamsUniformValue() -> void;

private:
    vsg::ref_ptr<vsg::RasterizationState> _rasterizationState{};

    vsg::ref_ptr<vsg::SetPolygonMode> _polygonModeCmd{};
    vsg::ref_ptr<vsg::SetLineWidth> _lineWidthCmd{};
    vsg::ref_ptr<vsg::SetCullMode> _setCullModeCmd{};

    vsg::ref_ptr<vsg::GraphicsPipeline> _graphicsPipeline{};

    vsg::ref_ptr<vsg::intArray> _objectParamsUniformValue{};

    ObjectsAppearenceMode _appearnceMode{ ObjectsAppearenceMode::TEXTURE };
    ObjectsLightMode _lightMode{ ObjectsLightMode::CONSTANT };
};

}  // namespace tire
