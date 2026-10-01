
#pragma once

#include "subgraph.h"
#include "objectslist.h"

#include "../vsgcommands/setpolygonmode.h"
#include "../vsgcommands/setcullmode.h"

#include "boundingrender.h"

namespace tire {

// ======================================================================================
// ==================== SceneUIProxy ====================================================
// ======================================================================================

enum class ObjectsRenderMode { WIREFRAME, SOLID, SOLIDWIRE };
enum class ObjectsAppearenceMode { COLOR, TEXTURE };
enum class ObjectsLightMode { NONE, CONSTANT, INSCENE };

struct SceneSubgraph;

struct SceneUIProxy final : public QObject {
    Q_OBJECT

    Q_PROPERTY( ObjectsList* objects READ objects NOTIFY objectsChanged FINAL )
    Q_PROPERTY( QString selectedObjectUid READ selectedObjectUid WRITE setSelectedObjectUid NOTIFY
                    selectedObjectUidChanged FINAL )
    Q_PROPERTY( bool isAnyObjectSelected READ isAnyObjectSelected NOTIFY isAnyObjectSelectedChanged FINAL )

    Q_PROPERTY( int renderMode READ renderMode WRITE setRenderMode NOTIFY renderModeChanged FINAL )
    Q_PROPERTY( int appearnceMode READ appearnceMode WRITE setAppearnceMode NOTIFY appearnceModeChanged FINAL )
    Q_PROPERTY( int lightMode READ lightMode WRITE setLightMode NOTIFY lightModeChanged FINAL )

    Q_PROPERTY( int showOuline READ showOuline WRITE setShowOuline NOTIFY showOulineChanged FINAL )

public:
    SceneUIProxy( vsg::observer_ptr<SceneSubgraph> scene, vsg::observer_ptr<BoundingRender> boundingRender,
                  QObject* parent = nullptr );

    [[nodiscard]]
    auto sceneNode() const -> vsg::ref_ptr<SceneSubgraph>;

    Q_INVOKABLE void addBox( const BoxObjectData& data );
    Q_INVOKABLE void addSphere( const SphereObjectData& data );
    Q_INVOKABLE void addCylinder( const CylinderObjectData& data );
    Q_INVOKABLE void addCapsule( const CapsuleObjectData& data );

    Q_INVOKABLE void removeObject( const QUuid& uid );

    [[nodiscard]]
    auto objects() const -> ObjectsList*;

    [[nodiscard]]
    auto selectedObjectUid() const -> QString;
    auto setSelectedObjectUid( const QString& value ) -> void;

    [[nodiscard]]
    auto isAnyObjectSelected() const -> bool;

    [[nodiscard]]
    auto renderMode() const -> int;
    auto setRenderMode( int value ) -> void;

    [[nodiscard]]
    auto appearnceMode() const -> int;
    auto setAppearnceMode( int value ) -> void;

    [[nodiscard]]
    auto lightMode() const -> int;
    auto setLightMode( int value ) -> void;

    [[nodiscard]]
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
    void isAnyObjectSelectedChanged();

private:
    vsg::observer_ptr<SceneSubgraph> _scene{};

    vsg::observer_ptr<BoundingRender> _boundingRender{};

    ObjectsList* _objects{};

    QUuid _selectedObjectUid{};

    ObjectsRenderMode _renderMode{ ObjectsRenderMode::SOLID };

    bool _showOuline{ false };
};

// ======================================================================================
// ==================== SceneSubgraph ===================================================
// ======================================================================================

struct SceneSubgraph final : Subgraph {
    SceneSubgraph( vsg::observer_ptr<vsg::Viewer> viewer );

    auto stateGroups() const -> std::vector<vsg::ref_ptr<vsg::StateGroup>> override;

    auto initPipeline() -> void;

    auto attach( std::shared_ptr<SceneObjectBase> object ) -> void;
    auto detach( std::shared_ptr<SceneObjectBase> object ) -> void;

    friend SceneUIProxy;

private:
    auto updateObjectParamsUniformValue() -> void;

private:
    vsg::ref_ptr<vsg::StateGroup> _stateGroup{};

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
