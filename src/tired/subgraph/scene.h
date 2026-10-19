
#pragma once

#include <QJsonArray>
#include <QVariantMap>

#include "subgraph.h"
#include "objectslist.h"

#include "../vsgcommands/setpolygonmode.h"
#include "../vsgcommands/setcullmode.h"

#include "boundingrender.h"
#include "../dynamicrendergraphicspipeline.h"

namespace tire {

// ======================================================================================
// ==================== SceneUIProxy ====================================================
// ======================================================================================

enum class ObjectsRenderMode {
    //
    WIREFRAME,
    SOLID,
    SOLIDWIRE
};

enum class ObjectsAppearenceMode {
    //
    COLOR,
    TEXTURE
};

enum class ObjectsLightMode {
    //
    NONE,
    CONSTANT,
    INSCENE
};

struct SceneSubgraph;

struct SceneUIProxy final : public QObject {
    Q_OBJECT

    Q_PROPERTY( SceneObjectsListModel* objects READ objects NOTIFY objectsChanged FINAL )
    Q_PROPERTY( QString selectedObjectUid READ selectedObjectUid WRITE setSelectedObjectUid NOTIFY
                    selectedObjectUidChanged FINAL )
    Q_PROPERTY( bool isAnyObjectSelected READ isAnyObjectSelected NOTIFY isAnyObjectSelectedChanged FINAL )

    Q_PROPERTY( int renderMode READ renderMode WRITE setRenderMode NOTIFY renderModeChanged FINAL )
    Q_PROPERTY( int appearnceMode READ appearnceMode WRITE setAppearnceMode NOTIFY appearnceModeChanged FINAL )
    Q_PROPERTY( int lightMode READ lightMode WRITE setLightMode NOTIFY lightModeChanged FINAL )

public:
    SceneUIProxy( vsg::observer_ptr<SceneSubgraph> scene, vsg::observer_ptr<BoundingRender> boundingRender,
                  QObject* parent = nullptr );

    [[nodiscard]] auto sceneNode() const -> vsg::ref_ptr<SceneSubgraph>;

    Q_INVOKABLE void add( const QVariantMap& data );

    Q_INVOKABLE void removeObject( const QUuid& uid );

    [[nodiscard]] auto objects() const -> SceneObjectsListModel*;

    [[nodiscard]] auto selectedObjectUid() const -> QString;
    auto setSelectedObjectUid( const QString& value ) -> void;

    [[nodiscard]] auto isAnyObjectSelected() const -> bool;

    [[nodiscard]] auto renderMode() const -> int;
    auto setRenderMode( int value ) -> void;

    [[nodiscard]] auto appearnceMode() const -> int;
    auto setAppearnceMode( int value ) -> void;

    [[nodiscard]] auto lightMode() const -> int;
    auto setLightMode( int value ) -> void;

    Q_INVOKABLE SceneObjectBase* findObject( const QString& uid ) const;
    Q_INVOKABLE [[nodiscard]] QJsonArray serializeObjects() const;

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

    SceneObjectsListModel* _objectsListModel{};

    QUuid _selectedObjectUid{};
};

// ======================================================================================
// ==================== SceneSubgraph ===================================================
// ======================================================================================

struct SceneSubgraph final : Subgraph {
    SceneSubgraph( vsg::observer_ptr<vsg::Viewer> viewer );

    auto stateGroups() const -> std::vector<vsg::ref_ptr<vsg::StateGroup>> override;

    auto initPipeline() -> void;

    auto attach( vsg::ref_ptr<SceneObjectNode> object ) -> void;
    auto detach( vsg::ref_ptr<SceneObjectNode> object ) -> void;

    auto addBox( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                 vsg::vec4 color, vsg::dvec3 size, vsg::ivec3 segments ) -> void;

    auto addSphere( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                    vsg::vec4 color, double radius, int slices, int segments, double sliceStart, double sliceSweep,
                    double segmentStart, double segmentSweep ) -> void;

    auto addCylinder( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                      vsg::vec4 color, double radius, double size, int slices, int segments, int rings, double start,
                      double sweep ) -> void;

    auto addCapsule( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                     vsg::vec4 color, double radius, double size, int slices, int segments, int rings, double start,
                     double sweep ) -> void;

    auto addCone( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                  vsg::vec4 color, double radius, double size, int slices, int segments, int rings, double start,
                  double sweep ) -> void;

    auto addTorus( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                   vsg::vec4 color, double minor, double major, int slices, int segments, double minorStart,
                   double minorSweep, double majorStart, double majorSweep ) -> void;

    auto addIcosphere( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                       vsg::vec4 color, double radius, int segments ) -> void;

    auto addIcosahedron( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                         vsg::dvec3 scale, vsg::vec4 color, double radius, int segments ) -> void;

    auto addDodecahedron( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                          vsg::dvec3 scale, vsg::vec4 color, double radius, int segments, int rings ) -> void;

    auto addPlane( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                   vsg::vec4 color, vsg::dvec2 size, vsg::ivec2 segments ) -> void;

    auto addDisk( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                  vsg::vec4 color, double radius, double innerRadius, int slices, int rings, double start,
                  double sweep ) -> void;

    auto addTube( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                  vsg::vec4 color, double radius, double innerRadius, double size, int slices, int segments,
                  double start, double sweep ) -> void;

    auto addCappedTube( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                        vsg::dvec3 scale, vsg::vec4 color, double radius, double innerRadius, double size, int slices,
                        int segments, int rings, double start, double sweep ) -> void;

    auto addRoundedBox( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                        vsg::dvec3 scale, vsg::vec4 color, double radius, vsg::dvec3 size, int slices,
                        vsg::ivec3 segments ) -> void;

    auto addSphericalCone( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                           vsg::dvec3 scale, vsg::vec4 color, double radius, double size, int slices, int segments,
                           int rings, double start, double sweep ) -> void;

    auto addTorusKnot( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                       vsg::vec4 color, int p, int q, int slices, int segments ) -> void;

    auto addSpring( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                    vsg::vec4 color, double minor, double major, double size, int slices, int segments,
                    double minorStart, double minorSweep, double majorStart, double majorSweep ) -> void;

    auto addTeapot( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                    vsg::vec4 color, int segments ) -> void;

    [[nodiscard]] auto objectsList() const -> std::shared_ptr<std::vector<std::shared_ptr<SceneObjectBase>>>;

    [[nodiscard]] auto findObject( const QUuid& uid ) const -> std::shared_ptr<SceneObjectBase>;
    auto removeObject( const QUuid& uid ) -> std::shared_ptr<SceneObjectBase>;

    friend SceneUIProxy;

private:
    auto updateObjectParamsUniformValue() -> void;

private:
    std::shared_ptr<std::vector<std::shared_ptr<SceneObjectBase>>> _objectsList{};

    vsg::ref_ptr<vsg::StateGroup> _stateGroup{};

    vsg::ref_ptr<vsg::RasterizationState> _rasterizationState{};

    vsg::ref_ptr<vsg::SetPolygonMode> _polygonModeCmd{};
    vsg::ref_ptr<vsg::SetLineWidth> _lineWidthCmd{};
    vsg::ref_ptr<vsg::SetCullMode> _setCullModeCmd{};

    vsg::ref_ptr<vsg::DynamicRenderGraphicsPipeline> _graphicsPipeline{};

    vsg::ref_ptr<vsg::intArray> _objectParamsUniformValue{};

    ObjectsAppearenceMode _appearenceMode{ ObjectsAppearenceMode::TEXTURE };
    ObjectsLightMode _lightMode{ ObjectsLightMode::CONSTANT };
    ObjectsRenderMode _renderMode{ ObjectsRenderMode::SOLID };
};

}  // namespace tire
