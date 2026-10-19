

#include <algorithm>

#include <vsg/all.h>

#include "image/tga.h"
#include "log/log.h"

#include "scene.h"

#include "program/program.h"
#include "config/config.h"
#include "../sceneobject/sceneobjectdata.h"
#include "../sceneobject/box.h"
#include "../sceneobject/sphere.h"
#include "../sceneobject/cylinder.h"
#include "../sceneobject/capsule.h"
#include "../sceneobject/cone.h"
#include "../sceneobject/torus.h"
#include "../sceneobject/icosphere.h"
#include "../sceneobject/icosahedron.h"
#include "../sceneobject/dodecahedron.h"
#include "../sceneobject/plane.h"
#include "../sceneobject/disk.h"
#include "../sceneobject/tube.h"
#include "../sceneobject/cappedtube.h"
#include "../sceneobject/roundedbox.h"
#include "../sceneobject/sphericalcone.h"
#include "../sceneobject/torusknot.h"
#include "../sceneobject/spring.h"
#include "../sceneobject/teapot.h"
#include "../sceneobject/serialization.h"

#include "../vsgcommands/setpolygonmode.h"
#include "tired/subgraph/boundingdraw.h"

namespace tire {

// ======================================================================================
// ==================== SceneUIProxy ====================================================
// ======================================================================================

SceneUIProxy::SceneUIProxy( vsg::observer_ptr<SceneSubgraph> scene, vsg::observer_ptr<BoundingRender> boundingRender,
                            QObject* parent )
    : QObject{ parent }
    , _scene{ scene }
    , _boundingRender{ boundingRender }
    , _objectsListModel{ new SceneObjectsListModel{ scene.get()->objectsList(), this } } {
    //
}

auto SceneUIProxy::sceneNode() const -> vsg::ref_ptr<SceneSubgraph> {
    //
    return _scene;
}

auto SceneUIProxy::objects() const -> SceneObjectsListModel* {
    return _objectsListModel;
}

auto SceneUIProxy::selectedObjectUid() const -> QString {
    return _selectedObjectUid.toString();
}

auto SceneUIProxy::setSelectedObjectUid( const QString& value ) -> void {
    _selectedObjectUid = QUuid::fromString( value );

    _boundingRender.get()->clear();

    if ( _selectedObjectUid.isNull() ) {
        emit selectedObjectUidChanged();
        emit selectedObjectChanged( nullptr );
        emit isAnyObjectSelectedChanged();

        return;
    }

    auto selectedObject = _scene.get()->findObject( QUuid::fromString( value ) );

    _boundingRender.get()->attach( selectedObject->node()->boundingDraw() );

    emit selectedObjectUidChanged();
    emit selectedObjectChanged( selectedObject.get() );
    emit isAnyObjectSelectedChanged();
}

auto SceneUIProxy::isAnyObjectSelected() const -> bool {
    return !_selectedObjectUid.isNull();
}

auto SceneUIProxy::renderMode() const -> int {
    return static_cast<int>( _scene.get()->_renderMode );
}

auto SceneUIProxy::setRenderMode( int value ) -> void {
    _scene.get()->_renderMode = static_cast<ObjectsRenderMode>( value );

    switch ( _scene.get()->_renderMode ) {
        case ObjectsRenderMode::WIREFRAME: {
            _scene.get()->_polygonModeCmd->mode = VK_POLYGON_MODE_LINE;
            _scene.get()->_setCullModeCmd->mode = VK_CULL_MODE_NONE;
            break;
        }
        case ObjectsRenderMode::SOLID: {
            _scene.get()->_polygonModeCmd->mode = VK_POLYGON_MODE_FILL;
            _scene.get()->_setCullModeCmd->mode = VK_CULL_MODE_BACK_BIT;
            break;
        }
        case ObjectsRenderMode::SOLIDWIRE: {
            break;
        }
        default:
            break;
    }

    emit renderModeChanged();
}

auto SceneUIProxy::appearnceMode() const -> int {
    return static_cast<int>( _scene.get()->_appearenceMode );
}

auto SceneUIProxy::setAppearnceMode( int value ) -> void {
    _scene.get()->_appearenceMode = static_cast<ObjectsAppearenceMode>( value );
    _scene.get()->updateObjectParamsUniformValue();
    emit appearnceModeChanged();
}

auto SceneUIProxy::lightMode() const -> int {
    return static_cast<int>( _scene.get()->_lightMode );
}

auto SceneUIProxy::setLightMode( int value ) -> void {
    _scene.get()->_lightMode = static_cast<ObjectsLightMode>( value );
    _scene.get()->updateObjectParamsUniformValue();
    emit lightModeChanged();
}

SceneObjectBase* SceneUIProxy::findObject( const QString& uid ) const {
    auto obj = _scene.get()->findObject( QUuid::fromString( uid ) );
    return obj.get();
}

auto SceneUIProxy::serializeObjects() const -> QJsonArray {
    //
    auto result = QJsonArray{};

    for ( const auto& object : *_scene.get()->objectsList() ) {
        if ( !object ) {
            continue;
        }

        auto visitor = JsonSerializationVisitor{};
        object->node()->serialize( &visitor );

        result.append( visitor.json() );
    }

    return result;
}

void SceneUIProxy::addObject( const QVariantMap& data ) {
    const auto json = QJsonObject::fromVariantMap( data );

    const auto type = static_cast<SceneObjectTypeEnum>( json.value( "type" ).toInt() );

    const auto appendObject = [this]( const auto& addFn ) -> void {
        const auto row = static_cast<int>( _scene.get()->objectsList()->size() );

        _objectsListModel->beginInsertRows( QModelIndex(), row, row );

        addFn();

        _objectsListModel->endInsertRows();

        emit objectsChanged();
    };

    switch ( type ) {
        case SceneObjectTypeEnum::BOX: {
            auto d = BoxObjectData{};
            d.fromJson( json );

            appendObject( [this, &d]() -> void {
                _scene.get()->addBox( d.name().toStdString(), d.uid().toString().toStdString(), d.position(),
                                      d.orientation(), d.scale(), vsg::vec4{ d.color() },
                                      vsg::dvec3{ d._sizeX, d._sizeY, d._sizeZ },
                                      vsg::ivec3{ d._segmentsX, d._segmentsY, d._segmentsZ } );
            } );

            break;
        }
        case SceneObjectTypeEnum::SPHERE: {
            auto d = SphereObjectData{};
            d.fromJson( json );

            appendObject( [this, &d]() -> void {
                _scene.get()->addSphere( d.name().toStdString(), d.uid().toString().toStdString(), d.position(),
                                         d.orientation(), d.scale(), vsg::vec4{ d.color() }, d._radius, d._slices,
                                         d._segments, d._sliceStart, d._sliceSweep, d._segmentStart, d._segmentSweep );
            } );

            break;
        }
        case SceneObjectTypeEnum::CYLINDER: {
            auto d = CylinderObjectData{};
            d.fromJson( json );

            appendObject( [this, &d]() -> void {
                _scene.get()->addCylinder( d.name().toStdString(), d.uid().toString().toStdString(), d.position(),
                                           d.orientation(), d.scale(), vsg::vec4{ d.color() }, d._radius, d._size,
                                           d._slices, d._segments, d._rings, d._start, d._sweep );
            } );

            break;
        }
        case SceneObjectTypeEnum::CAPSULE: {
            auto d = CapsuleObjectData{};
            d.fromJson( json );

            appendObject( [this, &d]() -> void {
                _scene.get()->addCapsule( d.name().toStdString(), d.uid().toString().toStdString(), d.position(),
                                          d.orientation(), d.scale(), vsg::vec4{ d.color() }, d._radius, d._size,
                                          d._slices, d._segments, d._rings, d._start, d._sweep );
            } );

            break;
        }
        case SceneObjectTypeEnum::CONE: {
            auto d = ConeObjectData{};
            d.fromJson( json );

            appendObject( [this, &d]() -> void {
                _scene.get()->addCone( d.name().toStdString(), d.uid().toString().toStdString(), d.position(),
                                       d.orientation(), d.scale(), vsg::vec4{ d.color() }, d._radius, d._size,
                                       d._slices, d._segments, d._rings, d._start, d._sweep );
            } );

            break;
        }
        case SceneObjectTypeEnum::TORUS: {
            auto d = TorusObjectData{};
            d.fromJson( json );

            appendObject( [this, &d]() -> void {
                _scene.get()->addTorus( d.name().toStdString(), d.uid().toString().toStdString(), d.position(),
                                        d.orientation(), d.scale(), vsg::vec4{ d.color() }, d._minor, d._major,
                                        d._slices, d._segments, d._minorStart, d._minorSweep, d._majorStart,
                                        d._majorSweep );
            } );

            break;
        }
        case SceneObjectTypeEnum::ICOSPHERE: {
            auto d = IcosphereObjectData{};
            d.fromJson( json );

            appendObject( [this, &d]() -> void {
                _scene.get()->addIcosphere( d.name().toStdString(), d.uid().toString().toStdString(), d.position(),
                                            d.orientation(), d.scale(), vsg::vec4{ d.color() }, d._radius,
                                            d._segments );
            } );

            break;
        }
        case SceneObjectTypeEnum::ICOSAHEDRON: {
            auto d = IcosahedronObjectData{};
            d.fromJson( json );

            appendObject( [this, &d]() -> void {
                _scene.get()->addIcosahedron( d.name().toStdString(), d.uid().toString().toStdString(), d.position(),
                                              d.orientation(), d.scale(), vsg::vec4{ d.color() }, d._radius,
                                              d._segments );
            } );

            break;
        }
        case SceneObjectTypeEnum::DODECAHEDRON: {
            auto d = DodecahedronObjectData{};
            d.fromJson( json );

            appendObject( [this, &d]() -> void {
                _scene.get()->addDodecahedron( d.name().toStdString(), d.uid().toString().toStdString(), d.position(),
                                               d.orientation(), d.scale(), vsg::vec4{ d.color() }, d._radius,
                                               d._segments, d._rings );
            } );

            break;
        }
        case SceneObjectTypeEnum::PLANE: {
            auto d = PlaneObjectData{};
            d.fromJson( json );

            appendObject( [this, &d]() -> void {
                _scene.get()->addPlane( d.name().toStdString(), d.uid().toString().toStdString(), d.position(),
                                        d.orientation(), d.scale(), vsg::vec4{ d.color() },
                                        vsg::dvec2{ d._sizeX, d._sizeY }, vsg::ivec2{ d._segmentsX, d._segmentsY } );
            } );

            break;
        }
        case SceneObjectTypeEnum::DISK: {
            auto d = DiskObjectData{};
            d.fromJson( json );

            appendObject( [this, &d]() -> void {
                _scene.get()->addDisk( d.name().toStdString(), d.uid().toString().toStdString(), d.position(),
                                       d.orientation(), d.scale(), vsg::vec4{ d.color() }, d._radius, d._innerRadius,
                                       d._slices, d._rings, d._start, d._sweep );
            } );

            break;
        }
        case SceneObjectTypeEnum::TUBE: {
            auto d = TubeObjectData{};
            d.fromJson( json );

            appendObject( [this, &d]() -> void {
                _scene.get()->addTube( d.name().toStdString(), d.uid().toString().toStdString(), d.position(),
                                       d.orientation(), d.scale(), vsg::vec4{ d.color() }, d._radius, d._innerRadius,
                                       d._size, d._slices, d._segments, d._start, d._sweep );
            } );

            break;
        }
        case SceneObjectTypeEnum::CAPPED_TUBE: {
            auto d = CappedTubeObjectData{};
            d.fromJson( json );

            appendObject( [this, &d]() -> void {
                _scene.get()->addCappedTube( d.name().toStdString(), d.uid().toString().toStdString(), d.position(),
                                             d.orientation(), d.scale(), vsg::vec4{ d.color() }, d._radius,
                                             d._innerRadius, d._size, d._slices, d._segments, d._rings, d._start,
                                             d._sweep );
            } );

            break;
        }
        case SceneObjectTypeEnum::ROUNDED_BOX: {
            auto d = RoundedBoxObjectData{};
            d.fromJson( json );

            appendObject( [this, &d]() -> void {
                _scene.get()->addRoundedBox( d.name().toStdString(), d.uid().toString().toStdString(), d.position(),
                                             d.orientation(), d.scale(), vsg::vec4{ d.color() }, d._radius,
                                             vsg::dvec3{ d._sizeX, d._sizeY, d._sizeZ }, d._slices,
                                             vsg::ivec3{ d._segmentsX, d._segmentsY, d._segmentsZ } );
            } );

            break;
        }
        case SceneObjectTypeEnum::SPHERICAL_CONE: {
            auto d = SphericalConeObjectData{};
            d.fromJson( json );

            appendObject( [this, &d]() -> void {
                _scene.get()->addSphericalCone( d.name().toStdString(), d.uid().toString().toStdString(), d.position(),
                                                d.orientation(), d.scale(), vsg::vec4{ d.color() }, d._radius, d._size,
                                                d._slices, d._segments, d._rings, d._start, d._sweep );
            } );

            break;
        }
        case SceneObjectTypeEnum::TORUS_KNOT: {
            auto d = TorusKnotObjectData{};
            d.fromJson( json );

            appendObject( [this, &d]() -> void {
                _scene.get()->addTorusKnot( d.name().toStdString(), d.uid().toString().toStdString(), d.position(),
                                            d.orientation(), d.scale(), vsg::vec4{ d.color() }, d._p, d._q, d._slices,
                                            d._segments );
            } );

            break;
        }
        case SceneObjectTypeEnum::SPRING: {
            auto d = SpringObjectData{};
            d.fromJson( json );

            appendObject( [this, &d]() -> void {
                _scene.get()->addSpring( d.name().toStdString(), d.uid().toString().toStdString(), d.position(),
                                         d.orientation(), d.scale(), vsg::vec4{ d.color() }, d._minor, d._major,
                                         d._size, d._slices, d._segments, d._minorStart, d._minorSweep, d._majorStart,
                                         d._majorSweep );
            } );

            break;
        }
        case SceneObjectTypeEnum::TEAPOT: {
            auto d = TeapotObjectData{};
            d.fromJson( json );

            appendObject( [this, &d]() -> void {
                _scene.get()->addTeapot( d.name().toStdString(), d.uid().toString().toStdString(), d.position(),
                                         d.orientation(), d.scale(), vsg::vec4{ d.color() }, d._segments );
            } );

            break;
        }
        default: {
            log::warning()( "Unsupported scene object type: {}", json.value( "type" ).toInt() );
            break;
        }
    }
}

void SceneUIProxy::removeObject( const QUuid& uid ) {
    auto obj = _scene.get()->findObject( uid );

    if ( !obj ) {
        return;
    }

    if ( uid == _selectedObjectUid ) {
        setSelectedObjectUid( QUuid{}.toString() );
    }

    const auto& list = *_scene.get()->objectsList();

    const auto it = std::find( list.begin(), list.end(), obj );

    const auto row = static_cast<int>( std::distance( list.begin(), it ) );

    _objectsListModel->beginRemoveRows( QModelIndex(), row, row );

    _scene.get()->removeObject( uid );

    _objectsListModel->endRemoveRows();

    emit objectsChanged();
}

// ======================================================================================
// ==================== SceneSubgraph ===================================================
// ======================================================================================

SceneSubgraph::SceneSubgraph( vsg::observer_ptr<vsg::Viewer> viewer )
    : Subgraph{ viewer }
    , _objectsList{ std::make_shared<std::vector<std::shared_ptr<SceneObjectBase>>>() }
    , _stateGroup{ vsg::StateGroup::create() } {
    //
    this->addChild( _stateGroup );

    initPipeline();
}

auto SceneSubgraph::stateGroups() const -> std::vector<vsg::ref_ptr<vsg::StateGroup>> {
    return { _stateGroup };
}

auto SceneSubgraph::attach( vsg::ref_ptr<SceneObjectNode> object ) -> void {
    vsg::CompileResult cr{};

    const auto op = tire::AttachAndCompileOp::create( _viewer, _stateGroup, object, cr );

    _viewer.get()->addUpdateOperation( op );
}

auto SceneSubgraph::detach( vsg::ref_ptr<SceneObjectNode> object ) -> void {
    vsg::CompileResult cr{};

    const auto op = tire::DetachOp::create( _viewer, _stateGroup, object );

    _viewer.get()->addUpdateOperation( op );
}

auto SceneSubgraph::objectsList() const -> std::shared_ptr<std::vector<std::shared_ptr<SceneObjectBase>>> {
    return _objectsList;
}

auto SceneSubgraph::findObject( const QUuid& uid ) const -> std::shared_ptr<SceneObjectBase> {
    auto it = std::find_if( _objectsList->begin(), _objectsList->end(),
                            //
                            [uid]( const auto& item ) -> bool { return item->uid() == uid.toString(); } );

    if ( it == _objectsList->end() ) {
        return {};
    }

    return *it;
}

auto SceneSubgraph::removeObject( const QUuid& uid ) -> std::shared_ptr<SceneObjectBase> {
    auto it = std::find_if( _objectsList->begin(), _objectsList->end(),
                            //
                            [uid]( const auto& item ) -> bool { return item->uid() == uid.toString(); } );

    if ( it == _objectsList->end() ) {
        return {};
    }

    auto object = *it;

    detach( object->node() );

    _objectsList->erase( it );

    return object;
}

auto SceneSubgraph::addBox( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                            vsg::dvec3 scale, vsg::vec4 color, vsg::dvec3 size, vsg::ivec3 segments ) -> void {
    auto node = vsg::ref_ptr<BoxNode>{ new BoxNode{ name, uid, position, orientation, scale, color, size, segments } };

    auto object = std::make_shared<BoxObject>( node );

    attach( node );

    node->setOwner( object.get() );

    _objectsList->push_back( object );
}

auto SceneSubgraph::addSphere( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                               vsg::dvec3 scale, vsg::vec4 color, double radius, int slices, int segments,
                               double sliceStart, double sliceSweep, double segmentStart, double segmentSweep )
    -> void {
    auto node =
        vsg::ref_ptr<SphereNode>{ new SphereNode{ name, uid, position, orientation, scale, color, radius, slices,
                                                  segments, sliceStart, sliceSweep, segmentStart, segmentSweep } };

    auto object = std::make_shared<SphereObject>( node );

    attach( node );

    node->setOwner( object.get() );

    _objectsList->push_back( object );
}

auto SceneSubgraph::addCylinder( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                                 vsg::dvec3 scale, vsg::vec4 color, double radius, double size, int slices,
                                 int segments, int rings, double start, double sweep ) -> void {
    auto node = vsg::ref_ptr<CylinderNode>{ new CylinderNode{ name, uid, position, orientation, scale, color, radius,
                                                              size, slices, segments, rings, start, sweep } };

    auto object = std::make_shared<CylinderObject>( node );

    attach( node );

    node->setOwner( object.get() );

    _objectsList->push_back( object );
}

auto SceneSubgraph::addCapsule( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                                vsg::dvec3 scale, vsg::vec4 color, double radius, double size, int slices, int segments,
                                int rings, double start, double sweep ) -> void {
    auto node = vsg::ref_ptr<CapsuleNode>{ new CapsuleNode{ name, uid, position, orientation, scale, color, radius,
                                                            size, slices, segments, rings, start, sweep } };

    auto object = std::make_shared<CapsuleObject>( node );

    attach( node );

    node->setOwner( object.get() );

    _objectsList->push_back( object );
}

auto SceneSubgraph::addCone( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                             vsg::dvec3 scale, vsg::vec4 color, double radius, double size, int slices, int segments,
                             int rings, double start, double sweep ) -> void {
    auto node = vsg::ref_ptr<ConeNode>{ new ConeNode{ name, uid, position, orientation, scale, color, radius, size,
                                                      slices, segments, rings, start, sweep } };

    auto object = std::make_shared<ConeObject>( node );

    attach( node );

    node->setOwner( object.get() );

    _objectsList->push_back( object );
}

auto SceneSubgraph::addTorus( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                              vsg::dvec3 scale, vsg::vec4 color, double minor, double major, int slices, int segments,
                              double minorStart, double minorSweep, double majorStart, double majorSweep ) -> void {
    auto node =
        vsg::ref_ptr<TorusNode>{ new TorusNode{ name, uid, position, orientation, scale, color, minor, major, slices,
                                                segments, minorStart, minorSweep, majorStart, majorSweep } };

    auto object = std::make_shared<TorusObject>( node );

    attach( node );

    node->setOwner( object.get() );

    _objectsList->push_back( object );
}

auto SceneSubgraph::addIcosphere( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                                  vsg::dvec3 scale, vsg::vec4 color, double radius, int segments ) -> void {
    auto node = vsg::ref_ptr<IcosphereNode>{
        new IcosphereNode{ name, uid, position, orientation, scale, color, radius, segments } };

    auto object = std::make_shared<IcosphereObject>( node );

    attach( node );

    node->setOwner( object.get() );

    _objectsList->push_back( object );
}

auto SceneSubgraph::addIcosahedron( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                                    vsg::dvec3 scale, vsg::vec4 color, double radius, int segments ) -> void {
    auto node = vsg::ref_ptr<IcosahedronNode>{
        new IcosahedronNode{ name, uid, position, orientation, scale, color, radius, segments } };

    auto object = std::make_shared<IcosahedronObject>( node );

    attach( node );

    node->setOwner( object.get() );

    _objectsList->push_back( object );
}

auto SceneSubgraph::addDodecahedron( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                                     vsg::dvec3 scale, vsg::vec4 color, double radius, int segments, int rings )
    -> void {
    auto node = vsg::ref_ptr<DodecahedronNode>{
        new DodecahedronNode{ name, uid, position, orientation, scale, color, radius, segments, rings } };

    auto object = std::make_shared<DodecahedronObject>( node );

    attach( node );

    node->setOwner( object.get() );

    _objectsList->push_back( object );
}

auto SceneSubgraph::addPlane( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                              vsg::dvec3 scale, vsg::vec4 color, vsg::dvec2 size, vsg::ivec2 segments ) -> void {
    auto node =
        vsg::ref_ptr<PlaneNode>{ new PlaneNode{ name, uid, position, orientation, scale, color, size, segments } };

    auto object = std::make_shared<PlaneObject>( node );

    attach( node );

    node->setOwner( object.get() );

    _objectsList->push_back( object );
}

auto SceneSubgraph::addDisk( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                             vsg::dvec3 scale, vsg::vec4 color, double radius, double innerRadius, int slices,
                             int rings, double start, double sweep ) -> void {
    auto node = vsg::ref_ptr<DiskNode>{ new DiskNode{ name, uid, position, orientation, scale, color, radius,
                                                      innerRadius, slices, rings, start, sweep } };

    auto object = std::make_shared<DiskObject>( node );

    attach( node );

    node->setOwner( object.get() );

    _objectsList->push_back( object );
}

auto SceneSubgraph::addTube( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                             vsg::dvec3 scale, vsg::vec4 color, double radius, double innerRadius, double size,
                             int slices, int segments, double start, double sweep ) -> void {
    auto node = vsg::ref_ptr<TubeNode>{ new TubeNode{ name, uid, position, orientation, scale, color, radius,
                                                      innerRadius, size, slices, segments, start, sweep } };

    auto object = std::make_shared<TubeObject>( node );

    attach( node );

    node->setOwner( object.get() );

    _objectsList->push_back( object );
}

auto SceneSubgraph::addCappedTube( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                                   vsg::dvec3 scale, vsg::vec4 color, double radius, double innerRadius, double size,
                                   int slices, int segments, int rings, double start, double sweep ) -> void {
    auto node =
        vsg::ref_ptr<CappedTubeNode>{ new CappedTubeNode{ name, uid, position, orientation, scale, color, radius,
                                                          innerRadius, size, slices, segments, rings, start, sweep } };

    auto object = std::make_shared<CappedTubeObject>( node );

    attach( node );

    node->setOwner( object.get() );

    _objectsList->push_back( object );
}

auto SceneSubgraph::addRoundedBox( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                                   vsg::dvec3 scale, vsg::vec4 color, double radius, vsg::dvec3 size, int slices,
                                   vsg::ivec3 segments ) -> void {
    auto node = vsg::ref_ptr<RoundedBoxNode>{
        new RoundedBoxNode{ name, uid, position, orientation, scale, color, radius, size, slices, segments } };

    auto object = std::make_shared<RoundedBoxObject>( node );

    attach( node );

    node->setOwner( object.get() );

    _objectsList->push_back( object );
}

auto SceneSubgraph::addSphericalCone( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                                      vsg::dvec3 scale, vsg::vec4 color, double radius, double size, int slices,
                                      int segments, int rings, double start, double sweep ) -> void {
    auto node = vsg::ref_ptr<SphericalConeNode>{ new SphericalConeNode{
        name, uid, position, orientation, scale, color, radius, size, slices, segments, rings, start, sweep } };

    auto object = std::make_shared<SphericalConeObject>( node );

    attach( node );

    node->setOwner( object.get() );

    _objectsList->push_back( object );
}

auto SceneSubgraph::addTorusKnot( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                                  vsg::dvec3 scale, vsg::vec4 color, int p, int q, int slices, int segments ) -> void {
    auto node = vsg::ref_ptr<TorusKnotNode>{
        new TorusKnotNode{ name, uid, position, orientation, scale, color, p, q, slices, segments } };

    auto object = std::make_shared<TorusKnotObject>( node );

    attach( node );

    node->setOwner( object.get() );

    _objectsList->push_back( object );
}

auto SceneSubgraph::addSpring( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                               vsg::dvec3 scale, vsg::vec4 color, double minor, double major, double size, int slices,
                               int segments, double minorStart, double minorSweep, double majorStart,
                               double majorSweep ) -> void {
    auto node =
        vsg::ref_ptr<SpringNode>{ new SpringNode{ name, uid, position, orientation, scale, color, minor, major, size,
                                                  slices, segments, minorStart, minorSweep, majorStart, majorSweep } };

    auto object = std::make_shared<SpringObject>( node );

    attach( node );

    node->setOwner( object.get() );

    _objectsList->push_back( object );
}

auto SceneSubgraph::addTeapot( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                               vsg::dvec3 scale, vsg::vec4 color, int segments ) -> void {
    auto node = vsg::ref_ptr<TeapotNode>{ new TeapotNode{ name, uid, position, orientation, scale, color, segments } };

    auto object = std::make_shared<TeapotObject>( node );

    attach( node );

    node->setOwner( object.get() );

    _objectsList->push_back( object );
}

auto SceneSubgraph::initPipeline() -> void {
    const auto sceneobjectProgram = Program{ TextProgramSource{ "sceneobject" } };
    const auto vertexShader = vsg::ShaderStage::create( VK_SHADER_STAGE_VERTEX_BIT, "main",
                                                        sceneobjectProgram.spirv( ShaderStageType::VERTEX ) );
    const auto fragmentShader = vsg::ShaderStage::create( VK_SHADER_STAGE_FRAGMENT_BIT, "main",
                                                          sceneobjectProgram.spirv( ShaderStageType::FRAGMENT ) );

    if ( !vertexShader || !fragmentShader ) {
        log::fatal()( "Could not create shaders." );
    }

    const auto basePath = Config::instance().basePath().string();

    // Read texture image.
    const auto rawData = std::make_unique<Tga>( std::format( "{}{}", basePath, "/assets/textures/glitch.tga" ) );
    const auto rawDataSize = rawData->width() * rawData->height() * rawData->components();

    const auto textureData = vsg::ubvec4Array2D::create( rawData->width(), rawData->height() );
    textureData->properties.format = VK_FORMAT_R8G8B8A8_SRGB;
    textureData->properties.dataVariance = vsg::DYNAMIC_DATA;
    std::memcpy( textureData->dataPointer(), rawData->data(), rawDataSize );
    textureData->dirty();

    const auto textureImage = vsg::ref_ptr<vsg::Image>{ new vsg::Image{ textureData } };
    // textureImage->imageType = VK_IMAGE_TYPE_2D;
    // textureImage->format = VK_FORMAT_R8G8B8A8_SRGB;
    // textureImage->extent = { width, height, 1 };
    // textureImage->mipLevels = 1;
    // textureImage->arrayLayers = 1;
    // textureImage->samples = VK_SAMPLE_COUNT_1_BIT;
    // textureImage->tiling = VK_IMAGE_TILING_OPTIMAL;
    // textureImage->usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    // textureImage->sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    const auto textureImageView = vsg::ImageView::create( textureImage );

    // create texture image and associated DescriptorSets and binding
    const auto sampler = vsg::Sampler::create();
    sampler->magFilter = VK_FILTER_LINEAR;
    sampler->minFilter = VK_FILTER_LINEAR;
    sampler->mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    const auto imageInfo =
        vsg::ImageInfo::create( sampler, textureImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
    const auto texture = vsg::DescriptorImage::create( imageInfo, 0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER );

    _objectParamsUniformValue =
        vsg::intArray::create( { static_cast<int>( _appearenceMode ), static_cast<int>( _lightMode ), 0, 0 } );
    _objectParamsUniformValue->properties.dataVariance = vsg::DYNAMIC_DATA;

    const auto objectParamsUniformDescriptor = vsg::DescriptorBuffer::create(
        _objectParamsUniformValue, /* dstBinding */ 1, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER );

    // set up graphics pipeline
    const auto descriptorBindings = vsg::DescriptorSetLayoutBindings{
        { /* binding */ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, /* count */ 1, VK_SHADER_STAGE_FRAGMENT_BIT,
          nullptr },
        { /* binding */ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, /* count */ 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr } };

    const auto descriptorSetLayout = vsg::DescriptorSetLayout::create( descriptorBindings );

    const auto pushConstantRanges = vsg::PushConstantRanges{
        { VK_SHADER_STAGE_VERTEX_BIT, 0, 128 }
        // projection, view, and model matrices, actual push constant calls automatically provided by the VSG's RecordTraversal
    };

    const auto vertexBindingsDescriptions = vsg::VertexInputState::Bindings{
        VkVertexInputBindingDescription{ 0, sizeof( vsg::vec3 ), VK_VERTEX_INPUT_RATE_VERTEX },  // vertex data
        VkVertexInputBindingDescription{ 1, sizeof( vsg::vec3 ), VK_VERTEX_INPUT_RATE_VERTEX },  // colour data
        VkVertexInputBindingDescription{ 2, sizeof( vsg::vec2 ), VK_VERTEX_INPUT_RATE_VERTEX },  // tex coord data
        VkVertexInputBindingDescription{ 3, sizeof( vsg::vec3 ), VK_VERTEX_INPUT_RATE_VERTEX }   // normals coord data
    };

    const auto vertexAttributeDescriptions = vsg::VertexInputState::Attributes{
        VkVertexInputAttributeDescription{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 },  // vertex data
        VkVertexInputAttributeDescription{ 1, 1, VK_FORMAT_R32G32B32_SFLOAT, 0 },  // colour data
        VkVertexInputAttributeDescription{ 2, 2, VK_FORMAT_R32G32_SFLOAT, 0 },     // tex coord data
        VkVertexInputAttributeDescription{ 3, 3, VK_FORMAT_R32G32B32_SFLOAT, 0 }   // normals coord data
    };

    _rasterizationState = vsg::RasterizationState::create();
    _rasterizationState->depthClampEnable = VK_FALSE;
    _rasterizationState->rasterizerDiscardEnable = VK_FALSE;
    _rasterizationState->polygonMode = VK_POLYGON_MODE_FILL;
    _rasterizationState->cullMode = VK_CULL_MODE_BACK_BIT;
    _rasterizationState->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    _rasterizationState->depthBiasEnable = VK_FALSE;
    _rasterizationState->depthBiasConstantFactor = 1.0f;
    _rasterizationState->depthBiasClamp = 0.0f;
    _rasterizationState->depthBiasSlopeFactor = 1.0f;
    _rasterizationState->lineWidth = 1.0f;

    const auto dynamicState = vsg::DynamicState::create();
    dynamicState->dynamicStates = { VK_DYNAMIC_STATE_LINE_WIDTH, VK_DYNAMIC_STATE_POLYGON_MODE_EXT,
                                    VK_DYNAMIC_STATE_CULL_MODE };

    _lineWidthCmd = vsg::SetLineWidth::create();
    _lineWidthCmd->lineWidth = 3.0f;

    _polygonModeCmd = vsg::SetPolygonMode::create();

    _setCullModeCmd = vsg::SetCullMode::create();
    _setCullModeCmd->mode = VK_CULL_MODE_BACK_BIT;

    const auto pipelineStates = vsg::GraphicsPipelineStates{
        vsg::VertexInputState::create( vertexBindingsDescriptions, vertexAttributeDescriptions ),
        vsg::InputAssemblyState::create(),
        _rasterizationState,
        vsg::MultisampleState::create(),
        vsg::ColorBlendState::create(),
        vsg::DepthStencilState::create(),
        dynamicState };

    const auto pipelineLayout =
        vsg::PipelineLayout::create( vsg::DescriptorSetLayouts{ descriptorSetLayout }, pushConstantRanges );

    _graphicsPipeline = vsg::ref_ptr<vsg::DynamicRenderGraphicsPipeline>{ new vsg::DynamicRenderGraphicsPipeline{
        pipelineLayout, vsg::ShaderStages{ vertexShader, fragmentShader }, pipelineStates } };

    const auto bindGraphicsPipeline = vsg::BindDynamicRenderGraphicsPipeline::create( _graphicsPipeline );

    const auto descriptorSet =
        vsg::DescriptorSet::create( descriptorSetLayout, vsg::Descriptors{ texture, objectParamsUniformDescriptor } );

    const auto bindDescriptorSet =
        vsg::BindDescriptorSet::create( VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, descriptorSet );

    _stateGroup->add( bindGraphicsPipeline );
    _stateGroup->add( bindDescriptorSet );

    _stateGroup->addChild( _polygonModeCmd );
    _stateGroup->addChild( _lineWidthCmd );
    _stateGroup->addChild( _setCullModeCmd );
}

auto SceneSubgraph::updateObjectParamsUniformValue() -> void {
    ( *_objectParamsUniformValue )[0] = static_cast<int>( _appearenceMode );
    ( *_objectParamsUniformValue )[1] = static_cast<int>( _lightMode );
    ( *_objectParamsUniformValue )[2] = 0;
    ( *_objectParamsUniformValue )[3] = 0;

    _objectParamsUniformValue->dirty();
}

}  // namespace tire
