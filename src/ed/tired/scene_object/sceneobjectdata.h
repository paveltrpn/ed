
#pragma once

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>

#include <vsg/all.h>

#include "ihasmaterial.h"
#include "ihascollision.h"

namespace tire {

enum class SceneObjectTypeEnum {
    BASE = 0,
    BOX,
    SPHERE,
    CYLINDER,
    CONE,
    CAPSULE,
    MESH,
    IMPLICIT,
    BEZIER_SURFACE,
    BEZIER_SPLINE,
    LANDSCAPE,
    POLYTOPE
};

struct SceneObjectData {
    Q_GADGET

    Q_PROPERTY( SceneObjectTypeEnum type READ type WRITE setType FINAL )
    Q_PROPERTY( QString name READ name WRITE setName FINAL )
    Q_PROPERTY( QUuid uid READ uid WRITE setUid FINAL )
    Q_PROPERTY( vsg::dvec3 position READ position WRITE setPosition FINAL )
    Q_PROPERTY( vsg::dvec3 orientation READ orientation WRITE setOrientation FINAL )
    Q_PROPERTY( vsg::dvec3 scale READ scale WRITE setScale FINAL )
    Q_PROPERTY( vsg::dvec4 color READ color WRITE setColor FINAL )

public:
    SceneObjectData()
        : _type{ SceneObjectTypeEnum::BASE }
        , _name{ "OBJECT" }
        , _uid{ QUuid::createUuid() }
        , _position{ 0.0, 0.0, 0.0 }
        , _orientation{ 0.0, 0.0, 0.0 }
        , _scale{ 1.0, 1.0, 1.0 }
        , _color{ 1.0, 1.0, 1.0, 1.0 } {};

    SceneObjectData( SceneObjectTypeEnum type, QString name, QUuid uid, vsg::dvec3 position, vsg::dvec3 orientation,
                     vsg::dvec3 scale, vsg::dvec4 color )
        : _type{ type }
        , _name{ name }
        , _uid{ uid }
        , _position{ position }
        , _orientation{ orientation }
        , _scale{ scale }
        , _color{ color } {};

    SceneObjectData( const SceneObjectData &other ) = default;
    SceneObjectData( SceneObjectData &&other ) = default;

    SceneObjectData &operator=( const SceneObjectData &other ) = default;
    SceneObjectData &operator=( SceneObjectData &&other ) = default;

    virtual ~SceneObjectData() = default;

    SceneObjectTypeEnum type() const {
        //
        return _type;
    }

    QString name() const {
        //
        return _name;
    }

    QUuid uid() const {
        //
        return _uid;
    }

    vsg::dvec3 position() const {
        //
        return _position;
    }

    vsg::dvec3 orientation() const {
        //
        return _orientation;
    }

    vsg::dvec3 scale() const {
        //
        return _scale;
    }

    vsg::dvec4 color() const {
        //
        return _color;
    }

    void setType( SceneObjectTypeEnum value ) {
        //
        _type = value;
    }

    void setName( const QString &value ) {
        //
        _name = value;
    }

    void setUid( const QUuid &value ) {
        //
        _uid = value;
    }

    void setPosition( vsg::dvec3 value ) {
        //
        _position = value;
    }

    void setOrientation( vsg::dvec3 value ) {
        //
        _orientation = value;
    }

    void setScale( vsg::dvec3 value ) {
        //
        _scale = value;
    }

    void setColor( vsg::dvec4 value ) {
        //
        _color = value;
    }

    virtual QJsonObject toJson() const {
        //
        auto &&positionArr = QJsonArray{ _position.x, _position.y, _position.z };
        auto &&orientationArr = QJsonArray{ _orientation.x, _orientation.y, _orientation.z };
        auto &&scaleArr = QJsonArray{ _scale.x, _scale.y, _scale.z };
        auto &&colorArr = QJsonArray{ _color.x, _color.y, _color.z, _color.w };

        return { { "type", static_cast<int>( _type ) },
                 { "name", _name },
                 { "uid", _uid.toString() },
                 { "position", positionArr },
                 { "orientation", orientationArr },
                 { "scale", scaleArr },
                 { "color", colorArr } };
    };

    virtual void fromJson( const QJsonObject &data ) {
        //
        _type = static_cast<SceneObjectTypeEnum>( data.value( "type" ).toInt() );
        _name = data.value( "name" ).toString();
        _uid = QUuid{ data.value( "uid" ).toString() };

        auto &&positionArr = data.value( "position" ).toArray();
        auto &&orientationArr = data.value( "orientation" ).toArray();
        auto &&scaleArr = data.value( "scale" ).toArray();
        auto &&colorArr = data.value( "color" ).toArray();

        _position = vsg::dvec3{ positionArr[0].toDouble(), positionArr[1].toDouble(), positionArr[2].toDouble() };
        _orientation =
            vsg::dvec3{ orientationArr[0].toDouble(), orientationArr[1].toDouble(), orientationArr[2].toDouble() };
        _scale = vsg::dvec3{ scaleArr[0].toDouble(), scaleArr[1].toDouble(), scaleArr[2].toDouble() };
        _color = vsg::dvec4{ colorArr[0].toDouble(), colorArr[1].toDouble(), colorArr[2].toDouble(),
                             colorArr[3].toDouble() };
    }

private:
    SceneObjectTypeEnum _type{};

    QString _name{};
    QUuid _uid{};

    vsg::dvec3 _position{};
    vsg::dvec3 _orientation{};
    vsg::dvec3 _scale{};
    vsg::dvec4 _color{};
};

// ==========================================================================================================

struct BoxObjectData final
    : public SceneObjectData
    , public IHasCollision {
    Q_GADGET

public:
    BoxObjectData()
        : SceneObjectData{}
        , _size{ 1.0 } {}

    BoxObjectData( const SceneObjectData &base, double size )
        : SceneObjectData{ base }
        , _size{ size } {};

    BoxObjectData( const BoxObjectData &other ) = default;
    BoxObjectData( BoxObjectData &&other ) = default;

    BoxObjectData &operator=( const BoxObjectData &other ) = default;
    BoxObjectData &operator=( BoxObjectData &&other ) = default;

    auto getShape() -> void override{
        //
        // TODO: implement interface
    };

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "size", _size },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _size = derived.value( "size" ).toDouble();
    }

private:
    double _size{};
};

// ==========================================================================================================

struct SphereObjectData final
    : public SceneObjectData
    , public IHasCollision {
    Q_GADGET

public:
    SphereObjectData() = default;

    SphereObjectData( const SphereObjectData &other ) = default;
    SphereObjectData( SphereObjectData &&other ) = default;

    SphereObjectData &operator=( const SphereObjectData &other ) = default;
    SphereObjectData &operator=( SphereObjectData &&other ) = default;

    auto getShape() -> void override{
        //
        // TODO: implement interface
    };

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "radius", _radius },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _radius = derived.value( "radius" ).toDouble();
    }

private:
    double _radius{};
};

// ==========================================================================================================

struct CylinderObjectData final
    : public SceneObjectData
    , public IHasCollision {
    Q_GADGET

public:
    CylinderObjectData() = default;

    CylinderObjectData( const CylinderObjectData &other ) = default;
    CylinderObjectData( CylinderObjectData &&other ) = default;

    CylinderObjectData &operator=( const CylinderObjectData &other ) = default;
    CylinderObjectData &operator=( CylinderObjectData &&other ) = default;

    auto getShape() -> void override{
        //
        // TODO: implement interface
    };

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "radius", _radius },
            { "height", _height },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _radius = derived.value( "radius" ).toDouble();
        _height = derived.value( "height" ).toDouble();
    }

private:
    double _radius{};
    double _height{};
};

// ==========================================================================================================

struct MeshData final
    : public SceneObjectData
    , public IHasCollision
    , public IHasMaterial {
    Q_GADGET

public:
    MeshData() = default;

    MeshData( const MeshData &other ) = default;
    MeshData( MeshData &&other ) = default;

    MeshData &operator=( const MeshData &other ) = default;
    MeshData &operator=( MeshData &&other ) = default;

    auto getMaterial() -> void override{
        //
        // TODO: implement interface

    };

    auto getShape() -> void override{
        //
        // TODO: implement interface
    };

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{};

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        //
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();
    }

private:
};

}  // namespace tire

Q_DECLARE_METATYPE( tire::SceneObjectTypeEnum )

Q_DECLARE_METATYPE( tire::SceneObjectData )
Q_DECLARE_METATYPE( tire::BoxObjectData )
Q_DECLARE_METATYPE( tire::SphereObjectData )
Q_DECLARE_METATYPE( tire::MeshData )
