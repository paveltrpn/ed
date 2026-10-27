
#pragma once

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>

#include <vsg/all.h>

#include "algebra/common.h"

namespace tire {

enum class SceneObjectTypeEnum {
    BASE = 0,
    BOX,
    SPHERE,
    CYLINDER,
    CAPSULE,
    CONE,
    TORUS,
    MESH,
    IMPLICIT,
    BEZIER_SURFACE,
    BEZIER_SPLINE,
    LANDSCAPE,
    POLYTOPE,
    ICOSPHERE,
    ICOSAHEDRON,
    DODECAHEDRON,
    PLANE,
    DISK,
    TUBE,
    CAPPED_TUBE,
    ROUNDED_BOX,
    SPHERICAL_CONE,
    TORUS_KNOT,
    SPRING,
    TEAPOT
};

// ======================================================================================
// ==================== SceneObjectData =================================================
// ======================================================================================

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
    SceneObjectData( SceneObjectTypeEnum type = SceneObjectTypeEnum::BASE, const QString &name = "OBJECT" )
        : _type{ type }
        , _name{ name } {}

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
        _type = static_cast<SceneObjectTypeEnum>( data.value( "type" ).toInt( static_cast<int>( _type ) ) );
        _name = data.value( "name" ).toString( _name );
        _uid = QUuid{ data.value( "uid" ).toString( _uid.toString() ) };

        const auto positionArr = data.value( "position" ).toArray();
        const auto orientationArr = data.value( "orientation" ).toArray();
        const auto scaleArr = data.value( "scale" ).toArray();
        const auto colorArr = data.value( "color" ).toArray();

        _position = vsg::dvec3{ positionArr[0].toDouble( _position.x ), positionArr[1].toDouble( _position.y ),
                                positionArr[2].toDouble( _position.z ) };
        _orientation =
            vsg::dvec3{ orientationArr[0].toDouble( _orientation.x ), orientationArr[1].toDouble( _orientation.y ),
                        orientationArr[2].toDouble( _orientation.z ) };
        _scale = vsg::dvec3{ scaleArr[0].toDouble( _scale.x ), scaleArr[1].toDouble( _scale.y ),
                             scaleArr[2].toDouble( _scale.z ) };
        _color = vsg::dvec4{ colorArr[0].toDouble( _color.x ), colorArr[1].toDouble( _color.y ),
                             colorArr[2].toDouble( _color.z ), colorArr[3].toDouble( _color.w ) };
    }

    SceneObjectTypeEnum _type{};
    QString _name{};

    QUuid _uid{ QUuid::createUuid() };

    vsg::dvec3 _position{ 0.0, 0.0, 0.0 };
    vsg::dvec3 _orientation{ 0.0, 0.0, 0.0 };
    vsg::dvec3 _scale{ 1.0, 1.0, 1.0 };
    vsg::dvec4 _color{ 1.0, 1.0, 1.0, 1.0 };
};

// ======================================================================================
// ==================== BoxObjectData ===================================================
// ======================================================================================

struct BoxObjectData final : public SceneObjectData {
    Q_GADGET

public:
    BoxObjectData()
        : SceneObjectData{ SceneObjectTypeEnum::BOX } {};

    BoxObjectData( const BoxObjectData &other ) = default;
    BoxObjectData( BoxObjectData &&other ) = default;

    BoxObjectData &operator=( const BoxObjectData &other ) = default;
    BoxObjectData &operator=( BoxObjectData &&other ) = default;

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "sizeX", _sizeX },         { "sizeY", _sizeY },         { "sizeZ", _sizeZ },
            { "segmentsX", _segmentsX }, { "segmentsY", _segmentsY }, { "segmentsZ", _segmentsZ },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _sizeX = derived.value( "sizeX" ).toDouble( _sizeX );
        _sizeY = derived.value( "sizeY" ).toDouble( _sizeY );
        _sizeZ = derived.value( "sizeZ" ).toDouble( _sizeZ );

        _segmentsX = derived.value( "segmentsX" ).toInt( _segmentsX );
        _segmentsY = derived.value( "segmentsY" ).toInt( _segmentsY );
        _segmentsZ = derived.value( "segmentsZ" ).toInt( _segmentsZ );
    }

    double _sizeX{ 0.5 };
    double _sizeY{ 0.5 };
    double _sizeZ{ 0.5 };

    int _segmentsX{ 1 };
    int _segmentsY{ 1 };
    int _segmentsZ{ 1 };
};

// ======================================================================================
// ==================== SphereObjectData ================================================
// ======================================================================================

struct SphereObjectData final : public SceneObjectData {
    Q_GADGET

public:
    SphereObjectData()
        : SceneObjectData{ SceneObjectTypeEnum::SPHERE } {};

    SphereObjectData( const SphereObjectData &other ) = default;
    SphereObjectData( SphereObjectData &&other ) = default;

    SphereObjectData &operator=( const SphereObjectData &other ) = default;
    SphereObjectData &operator=( SphereObjectData &&other ) = default;

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "radius", _radius },
            { "slices", _slices },
            { "segments", _segments },
            { "sliceStart", _sliceStart },
            { "liceSweep", _sliceSweep },
            { "segmentStart", _segmentStart },
            { "segmentSweep", _segmentSweep },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _radius = derived.value( "radius" ).toDouble( _radius );
        _slices = derived.value( "slices" ).toInt( _slices );
        _segments = derived.value( "segments" ).toInt( _segments );
        _sliceStart = derived.value( "sliceStart" ).toDouble( _sliceStart );
        _sliceSweep = derived.value( "sliceSweep" ).toDouble( _sliceSweep );
        _segmentStart = derived.value( "segmentStart" ).toDouble( _segmentStart );
        _segmentSweep = derived.value( "segmentSweep" ).toDouble( _segmentSweep );
    }

    double _radius{ 0.5 };
    int _slices{ 16 };
    int _segments{ 8 };
    double _sliceStart{ 0.0 };
    double _sliceSweep{ algebra::degToRad( 360.0 ) };
    double _segmentStart{ 0.0 };
    double _segmentSweep{ algebra::degToRad( 180.0 ) };
};

// ======================================================================================
// ==================== CylinderObjectData ==============================================
// ======================================================================================

struct CylinderObjectData final : public SceneObjectData {
    Q_GADGET

public:
    CylinderObjectData()
        : SceneObjectData{ SceneObjectTypeEnum::CYLINDER } {};

    CylinderObjectData( const CylinderObjectData &other ) = default;
    CylinderObjectData( CylinderObjectData &&other ) = default;

    CylinderObjectData &operator=( const CylinderObjectData &other ) = default;
    CylinderObjectData &operator=( CylinderObjectData &&other ) = default;

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "radius", _radius }, { "size", _size },   { "slices", _slices }, { "segments", _segments },
            { "rings", _rings },   { "start", _start }, { "sweep", _sweep },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _radius = derived.value( "radius" ).toDouble( _radius );
        _size = derived.value( "size" ).toDouble( _size );
        _slices = derived.value( "slices" ).toInt( _slices );
        _segments = derived.value( "segments" ).toInt( _segments );
        _rings = derived.value( "rings" ).toInt( _rings );
        _start = derived.value( "start" ).toDouble( _start );
        _sweep = derived.value( "sweep" ).toDouble( _sweep );
    }

    double _radius = { 0.5 };
    double _size = { 0.5 };
    int _slices = { 16 };
    int _segments = { 8 };
    int _rings = { 4 };
    double _start = { 0.0 };
    double _sweep = { algebra::degToRad( 360.0 ) };
};

// ======================================================================================
// ==================== CapsuleObjectData ===============================================
// ======================================================================================

struct CapsuleObjectData final : public SceneObjectData {
    Q_GADGET

public:
    CapsuleObjectData()
        : SceneObjectData{ SceneObjectTypeEnum::CAPSULE } {};

    CapsuleObjectData( const CapsuleObjectData &other ) = default;
    CapsuleObjectData( CapsuleObjectData &&other ) = default;

    CapsuleObjectData &operator=( const CapsuleObjectData &other ) = default;
    CapsuleObjectData &operator=( CapsuleObjectData &&other ) = default;

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "radius", _radius }, { "size", _size },   { "slices", _slices }, { "segments", _segments },
            { "rings", _rings },   { "start", _start }, { "sweep", _sweep },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _radius = derived.value( "radius" ).toDouble( _radius );
        _size = derived.value( "size" ).toDouble( _size );
        _slices = derived.value( "slices" ).toInt( _slices );
        _segments = derived.value( "segments" ).toInt( _segments );
        _rings = derived.value( "rings" ).toInt( _rings );
        _start = derived.value( "start" ).toDouble( _start );
        _sweep = derived.value( "sweep" ).toDouble( _sweep );
    }

    double _radius = { 0.5 };
    double _size = { 0.5 };
    int _slices = { 16 };
    int _segments = { 8 };
    int _rings = { 4 };
    double _start = { 0.0 };
    double _sweep = { algebra::degToRad( 360.0 ) };
};

// ======================================================================================
// ==================== ConeObjectData ==================================================
// ======================================================================================

struct ConeObjectData final : public SceneObjectData {
    Q_GADGET

public:
    ConeObjectData()
        : SceneObjectData{ SceneObjectTypeEnum::CONE } {};

    ConeObjectData( const ConeObjectData &other ) = default;
    ConeObjectData( ConeObjectData &&other ) = default;

    ConeObjectData &operator=( const ConeObjectData &other ) = default;
    ConeObjectData &operator=( ConeObjectData &&other ) = default;

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "radius", _radius }, { "size", _size },   { "slices", _slices }, { "segments", _segments },
            { "rings", _rings },   { "start", _start }, { "sweep", _sweep },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _radius = derived.value( "radius" ).toDouble( _radius );
        _size = derived.value( "size" ).toDouble( _size );
        _slices = derived.value( "slices" ).toInt( _slices );
        _segments = derived.value( "segments" ).toInt( _segments );
        _rings = derived.value( "rings" ).toInt( _rings );
        _start = derived.value( "start" ).toDouble( _start );
        _sweep = derived.value( "sweep" ).toDouble( _sweep );
    }

    double _radius = { 0.5 };
    double _size = { 0.5 };
    int _slices = { 16 };
    int _segments = { 8 };
    int _rings = { 4 };
    double _start = { 0.0 };
    double _sweep = { algebra::degToRad( 360.0 ) };
};

// ======================================================================================
// ==================== TorusObjectData =================================================
// ======================================================================================

struct TorusObjectData final : public SceneObjectData {
    Q_GADGET

public:
    TorusObjectData()
        : SceneObjectData{ SceneObjectTypeEnum::TORUS } {};

    TorusObjectData( const TorusObjectData &other ) = default;
    TorusObjectData( TorusObjectData &&other ) = default;

    TorusObjectData &operator=( const TorusObjectData &other ) = default;
    TorusObjectData &operator=( TorusObjectData &&other ) = default;

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "minor", _minor },           { "major", _major },           { "slices", _slices },
            { "segments", _segments },     { "minorStart", _minorStart }, { "minorSweep", _minorSweep },
            { "majorStart", _majorStart }, { "majorSweep", _majorSweep },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _minor = derived.value( "minor" ).toDouble( _minor );
        _major = derived.value( "major" ).toDouble( _major );
        _slices = derived.value( "slices" ).toInt( _slices );
        _segments = derived.value( "segments" ).toInt( _segments );
        _minorStart = derived.value( "minorStart" ).toDouble( _minorStart );
        _minorSweep = derived.value( "minorSweep" ).toDouble( _minorSweep );
        _majorStart = derived.value( "majorStart" ).toDouble( _majorStart );
        _majorSweep = derived.value( "majorSweep" ).toDouble( _majorSweep );
    }

    double _minor{ 0.25 };
    double _major{ 1.0 };
    int _slices{ 16 };
    int _segments{ 32 };
    double _minorStart{ 0.0 };
    double _minorSweep{ algebra::degToRad( 360.0 ) };
    double _majorStart{ 0.0 };
    double _majorSweep{ algebra::degToRad( 360.0 ) };
};

// ======================================================================================
// ==================== MeshObjectData ==================================================
// ======================================================================================

struct MeshObjectData final : public SceneObjectData {
    Q_GADGET

public:
    MeshObjectData() = default;

    MeshObjectData( const MeshObjectData &other ) = default;
    MeshObjectData( MeshObjectData &&other ) = default;

    MeshObjectData &operator=( const MeshObjectData &other ) = default;
    MeshObjectData &operator=( MeshObjectData &&other ) = default;

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

// ======================================================================================
// ==================== ImplicitSurfaceObjectData =======================================
// ======================================================================================

struct ImplicitSurfaceObjectData final : public SceneObjectData {
    Q_GADGET

public:
    ImplicitSurfaceObjectData() = default;

    ImplicitSurfaceObjectData( const ImplicitSurfaceObjectData &other ) = default;
    ImplicitSurfaceObjectData( ImplicitSurfaceObjectData &&other ) = default;

    ImplicitSurfaceObjectData &operator=( const ImplicitSurfaceObjectData &other ) = default;
    ImplicitSurfaceObjectData &operator=( ImplicitSurfaceObjectData &&other ) = default;

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
};

// ======================================================================================
// ==================== BezierSurfaceObjectData =========================================
// ======================================================================================

struct BezierSurfaceObjectData final : public SceneObjectData {
    Q_GADGET

public:
    BezierSurfaceObjectData() = default;

    BezierSurfaceObjectData( const BezierSurfaceObjectData &other ) = default;
    BezierSurfaceObjectData( BezierSurfaceObjectData &&other ) = default;

    BezierSurfaceObjectData &operator=( const BezierSurfaceObjectData &other ) = default;
    BezierSurfaceObjectData &operator=( BezierSurfaceObjectData &&other ) = default;

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
};

// ======================================================================================
// ==================== BezierSplineObjectData ==========================================
// ======================================================================================

struct BezierSplineObjectData final : public SceneObjectData {
    Q_GADGET

public:
    BezierSplineObjectData() = default;

    BezierSplineObjectData( const BezierSplineObjectData &other ) = default;
    BezierSplineObjectData( BezierSplineObjectData &&other ) = default;

    BezierSplineObjectData &operator=( const BezierSplineObjectData &other ) = default;
    BezierSplineObjectData &operator=( BezierSplineObjectData &&other ) = default;

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
};

// ======================================================================================
// ==================== LandscapeObjectData =============================================
// ======================================================================================

struct LandscapeObjectData final : public SceneObjectData {
    Q_GADGET

public:
    LandscapeObjectData() = default;

    LandscapeObjectData( const LandscapeObjectData &other ) = default;
    LandscapeObjectData( LandscapeObjectData &&other ) = default;

    LandscapeObjectData &operator=( const LandscapeObjectData &other ) = default;
    LandscapeObjectData &operator=( LandscapeObjectData &&other ) = default;

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
};

// ======================================================================================
// ==================== PolytopeObjectData ==============================================
// ======================================================================================

struct PolytopeObjectData final : public SceneObjectData {
    Q_GADGET

public:
    PolytopeObjectData() = default;

    PolytopeObjectData( const PolytopeObjectData &other ) = default;
    PolytopeObjectData( PolytopeObjectData &&other ) = default;

    PolytopeObjectData &operator=( const PolytopeObjectData &other ) = default;
    PolytopeObjectData &operator=( PolytopeObjectData &&other ) = default;

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
};

// ======================================================================================
// ==================== IcosphereObjectData =============================================
// ======================================================================================

struct IcosphereObjectData final : public SceneObjectData {
    Q_GADGET

public:
    IcosphereObjectData()
        : SceneObjectData{ SceneObjectTypeEnum::ICOSPHERE } {};

    IcosphereObjectData( const IcosphereObjectData &other ) = default;
    IcosphereObjectData( IcosphereObjectData &&other ) = default;

    IcosphereObjectData &operator=( const IcosphereObjectData &other ) = default;
    IcosphereObjectData &operator=( IcosphereObjectData &&other ) = default;

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "radius", _radius },
            { "segments", _segments },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _radius = derived.value( "radius" ).toDouble( _radius );
        _segments = derived.value( "segments" ).toInt( _segments );
    }

    double _radius{ 0.5 };
    int _segments{ 4 };
};

// ======================================================================================
// ==================== IcosahedronObjectData ===========================================
// ======================================================================================

struct IcosahedronObjectData final : public SceneObjectData {
    Q_GADGET

public:
    IcosahedronObjectData()
        : SceneObjectData{ SceneObjectTypeEnum::ICOSAHEDRON } {};

    IcosahedronObjectData( const IcosahedronObjectData &other ) = default;
    IcosahedronObjectData( IcosahedronObjectData &&other ) = default;

    IcosahedronObjectData &operator=( const IcosahedronObjectData &other ) = default;
    IcosahedronObjectData &operator=( IcosahedronObjectData &&other ) = default;

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "radius", _radius },
            { "segments", _segments },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _radius = derived.value( "radius" ).toDouble( _radius );
        _segments = derived.value( "segments" ).toInt( _segments );
    }

    double _radius{ 0.5 };
    int _segments{ 1 };
};

// ======================================================================================
// ==================== DodecahedronObjectData ==========================================
// ======================================================================================

struct DodecahedronObjectData final : public SceneObjectData {
    Q_GADGET

public:
    DodecahedronObjectData()
        : SceneObjectData{ SceneObjectTypeEnum::DODECAHEDRON } {};

    DodecahedronObjectData( const DodecahedronObjectData &other ) = default;
    DodecahedronObjectData( DodecahedronObjectData &&other ) = default;

    DodecahedronObjectData &operator=( const DodecahedronObjectData &other ) = default;
    DodecahedronObjectData &operator=( DodecahedronObjectData &&other ) = default;

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "radius", _radius },
            { "segments", _segments },
            { "rings", _rings },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _radius = derived.value( "radius" ).toDouble( _radius );
        _segments = derived.value( "segments" ).toInt( _segments );
        _rings = derived.value( "rings" ).toInt( _rings );
    }

    double _radius{ 0.5 };
    int _segments{ 1 };
    int _rings{ 1 };
};

// ======================================================================================
// ==================== PlaneObjectData =================================================
// ======================================================================================

struct PlaneObjectData final : public SceneObjectData {
    Q_GADGET

public:
    PlaneObjectData()
        : SceneObjectData{ SceneObjectTypeEnum::PLANE } {};

    PlaneObjectData( const PlaneObjectData &other ) = default;
    PlaneObjectData( PlaneObjectData &&other ) = default;

    PlaneObjectData &operator=( const PlaneObjectData &other ) = default;
    PlaneObjectData &operator=( PlaneObjectData &&other ) = default;

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "sizeX", _sizeX },
            { "sizeY", _sizeY },
            { "segmentsX", _segmentsX },
            { "segmentsY", _segmentsY },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _sizeX = derived.value( "sizeX" ).toDouble( _sizeX );
        _sizeY = derived.value( "sizeY" ).toDouble( _sizeY );

        _segmentsX = derived.value( "segmentsX" ).toInt( _segmentsX );
        _segmentsY = derived.value( "segmentsY" ).toInt( _segmentsY );
    }

    double _sizeX{ 0.5 };
    double _sizeY{ 0.5 };

    int _segmentsX{ 1 };
    int _segmentsY{ 1 };
};

// ======================================================================================
// ==================== DiskObjectData ==================================================
// ======================================================================================

struct DiskObjectData final : public SceneObjectData {
    Q_GADGET

public:
    DiskObjectData()
        : SceneObjectData{ SceneObjectTypeEnum::DISK } {};

    DiskObjectData( const DiskObjectData &other ) = default;
    DiskObjectData( DiskObjectData &&other ) = default;

    DiskObjectData &operator=( const DiskObjectData &other ) = default;
    DiskObjectData &operator=( DiskObjectData &&other ) = default;

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "radius", _radius }, { "innerRadius", _innerRadius },
            { "slices", _slices }, { "rings", _rings },
            { "start", _start },   { "sweep", _sweep },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _radius = derived.value( "radius" ).toDouble( _radius );
        _innerRadius = derived.value( "innerRadius" ).toDouble( _innerRadius );
        _slices = derived.value( "slices" ).toInt( _slices );
        _rings = derived.value( "rings" ).toInt( _rings );
        _start = derived.value( "start" ).toDouble( _start );
        _sweep = derived.value( "sweep" ).toDouble( _sweep );
    }

    double _radius{ 0.5 };
    double _innerRadius{ 0.0 };
    int _slices{ 16 };
    int _rings{ 4 };
    double _start{ 0.0 };
    double _sweep{ algebra::degToRad( 360.0 ) };
};

// ======================================================================================
// ==================== TubeObjectData ==================================================
// ======================================================================================

struct TubeObjectData final : public SceneObjectData {
    Q_GADGET

public:
    TubeObjectData()
        : SceneObjectData{ SceneObjectTypeEnum::TUBE } {};

    TubeObjectData( const TubeObjectData &other ) = default;
    TubeObjectData( TubeObjectData &&other ) = default;

    TubeObjectData &operator=( const TubeObjectData &other ) = default;
    TubeObjectData &operator=( TubeObjectData &&other ) = default;

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "radius", _radius }, { "innerRadius", _innerRadius }, { "size", _size },
            { "slices", _slices }, { "segments", _segments },       { "start", _start },
            { "sweep", _sweep },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _radius = derived.value( "radius" ).toDouble( _radius );
        _innerRadius = derived.value( "innerRadius" ).toDouble( _innerRadius );
        _size = derived.value( "size" ).toDouble( _size );
        _slices = derived.value( "slices" ).toInt( _slices );
        _segments = derived.value( "segments" ).toInt( _segments );
        _start = derived.value( "start" ).toDouble( _start );
        _sweep = derived.value( "sweep" ).toDouble( _sweep );
    }

    double _radius{ 0.5 };
    double _innerRadius{ 0.25 };
    double _size{ 0.5 };
    int _slices{ 16 };
    int _segments{ 8 };
    double _start{ 0.0 };
    double _sweep{ algebra::degToRad( 360.0 ) };
};

// ======================================================================================
// ==================== CappedTubeObjectData ============================================
// ======================================================================================

struct CappedTubeObjectData final : public SceneObjectData {
    Q_GADGET

public:
    CappedTubeObjectData()
        : SceneObjectData{ SceneObjectTypeEnum::CAPPED_TUBE } {};

    CappedTubeObjectData( const CappedTubeObjectData &other ) = default;
    CappedTubeObjectData( CappedTubeObjectData &&other ) = default;

    CappedTubeObjectData &operator=( const CappedTubeObjectData &other ) = default;
    CappedTubeObjectData &operator=( CappedTubeObjectData &&other ) = default;

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "radius", _radius },     { "innerRadius", _innerRadius },
            { "size", _size },         { "slices", _slices },
            { "segments", _segments }, { "rings", _rings },
            { "start", _start },       { "sweep", _sweep },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _radius = derived.value( "radius" ).toDouble( _radius );
        _innerRadius = derived.value( "innerRadius" ).toDouble( _innerRadius );
        _size = derived.value( "size" ).toDouble( _size );
        _slices = derived.value( "slices" ).toInt( _slices );
        _segments = derived.value( "segments" ).toInt( _segments );
        _rings = derived.value( "rings" ).toInt( _rings );
        _start = derived.value( "start" ).toDouble( _start );
        _sweep = derived.value( "sweep" ).toDouble( _sweep );
    }

    double _radius{ 0.5 };
    double _innerRadius{ 0.25 };
    double _size{ 0.5 };
    int _slices{ 16 };
    int _segments{ 8 };
    int _rings{ 1 };
    double _start{ 0.0 };
    double _sweep{ algebra::degToRad( 360.0 ) };
};

// ======================================================================================
// ==================== RoundedBoxObjectData ============================================
// ======================================================================================

struct RoundedBoxObjectData final : public SceneObjectData {
    Q_GADGET

public:
    RoundedBoxObjectData()
        : SceneObjectData{ SceneObjectTypeEnum::ROUNDED_BOX } {};

    RoundedBoxObjectData( const RoundedBoxObjectData &other ) = default;
    RoundedBoxObjectData( RoundedBoxObjectData &&other ) = default;

    RoundedBoxObjectData &operator=( const RoundedBoxObjectData &other ) = default;
    RoundedBoxObjectData &operator=( RoundedBoxObjectData &&other ) = default;

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "radius", _radius },       { "sizeX", _sizeX },         { "sizeY", _sizeY },
            { "sizeZ", _sizeZ },         { "slices", _slices },       { "segmentsX", _segmentsX },
            { "segmentsY", _segmentsY }, { "segmentsZ", _segmentsZ },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _radius = derived.value( "radius" ).toDouble( _radius );
        _sizeX = derived.value( "sizeX" ).toDouble( _sizeX );
        _sizeY = derived.value( "sizeY" ).toDouble( _sizeY );
        _sizeZ = derived.value( "sizeZ" ).toDouble( _sizeZ );
        _slices = derived.value( "slices" ).toInt( _slices );
        _segmentsX = derived.value( "segmentsX" ).toInt( _segmentsX );
        _segmentsY = derived.value( "segmentsY" ).toInt( _segmentsY );
        _segmentsZ = derived.value( "segmentsZ" ).toInt( _segmentsZ );
    }

    double _radius{ 0.1 };
    double _sizeX{ 0.4 };
    double _sizeY{ 0.4 };
    double _sizeZ{ 0.4 };
    int _slices{ 4 };
    int _segmentsX{ 1 };
    int _segmentsY{ 1 };
    int _segmentsZ{ 1 };
};

// ======================================================================================
// ==================== SphericalConeObjectData =========================================
// ======================================================================================

struct SphericalConeObjectData final : public SceneObjectData {
    Q_GADGET

public:
    SphericalConeObjectData()
        : SceneObjectData{ SceneObjectTypeEnum::SPHERICAL_CONE } {};

    SphericalConeObjectData( const SphericalConeObjectData &other ) = default;
    SphericalConeObjectData( SphericalConeObjectData &&other ) = default;

    SphericalConeObjectData &operator=( const SphericalConeObjectData &other ) = default;
    SphericalConeObjectData &operator=( SphericalConeObjectData &&other ) = default;

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "radius", _radius }, { "size", _size },   { "slices", _slices }, { "segments", _segments },
            { "rings", _rings },   { "start", _start }, { "sweep", _sweep },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _radius = derived.value( "radius" ).toDouble( _radius );
        _size = derived.value( "size" ).toDouble( _size );
        _slices = derived.value( "slices" ).toInt( _slices );
        _segments = derived.value( "segments" ).toInt( _segments );
        _rings = derived.value( "rings" ).toInt( _rings );
        _start = derived.value( "start" ).toDouble( _start );
        _sweep = derived.value( "sweep" ).toDouble( _sweep );
    }

    double _radius{ 0.5 };
    double _size{ 0.5 };
    int _slices{ 16 };
    int _segments{ 8 };
    int _rings{ 4 };
    double _start{ 0.0 };
    double _sweep{ algebra::degToRad( 360.0 ) };
};

// ======================================================================================
// ==================== TorusKnotObjectData =============================================
// ======================================================================================

struct TorusKnotObjectData final : public SceneObjectData {
    Q_GADGET

public:
    TorusKnotObjectData()
        : SceneObjectData{ SceneObjectTypeEnum::TORUS_KNOT } {};

    TorusKnotObjectData( const TorusKnotObjectData &other ) = default;
    TorusKnotObjectData( TorusKnotObjectData &&other ) = default;

    TorusKnotObjectData &operator=( const TorusKnotObjectData &other ) = default;
    TorusKnotObjectData &operator=( TorusKnotObjectData &&other ) = default;

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "p", _p },
            { "q", _q },
            { "slices", _slices },
            { "segments", _segments },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _p = derived.value( "p" ).toInt( _p );
        _q = derived.value( "q" ).toInt( _q );
        _slices = derived.value( "slices" ).toInt( _slices );
        _segments = derived.value( "segments" ).toInt( _segments );
    }

    int _p{ 2 };
    int _q{ 3 };
    int _slices{ 8 };
    int _segments{ 96 };
};

// ======================================================================================
// ==================== SpringObjectData ================================================
// ======================================================================================

struct SpringObjectData final : public SceneObjectData {
    Q_GADGET

public:
    SpringObjectData()
        : SceneObjectData{ SceneObjectTypeEnum::SPRING } {};

    SpringObjectData( const SpringObjectData &other ) = default;
    SpringObjectData( SpringObjectData &&other ) = default;

    SpringObjectData &operator=( const SpringObjectData &other ) = default;
    SpringObjectData &operator=( SpringObjectData &&other ) = default;

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "minor", _minor },           { "major", _major },           { "size", _size },
            { "slices", _slices },         { "segments", _segments },     { "minorStart", _minorStart },
            { "minorSweep", _minorSweep }, { "majorStart", _majorStart }, { "majorSweep", _majorSweep },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _minor = derived.value( "minor" ).toDouble( _minor );
        _major = derived.value( "major" ).toDouble( _major );
        _size = derived.value( "size" ).toDouble( _size );
        _slices = derived.value( "slices" ).toInt( _slices );
        _segments = derived.value( "segments" ).toInt( _segments );
        _minorStart = derived.value( "minorStart" ).toDouble( _minorStart );
        _minorSweep = derived.value( "minorSweep" ).toDouble( _minorSweep );
        _majorStart = derived.value( "majorStart" ).toDouble( _majorStart );
        _majorSweep = derived.value( "majorSweep" ).toDouble( _majorSweep );
    }

    double _minor{ 0.25 };
    double _major{ 1.0 };
    double _size{ 1.0 };
    int _slices{ 8 };
    int _segments{ 32 };
    double _minorStart{ 0.0 };
    double _minorSweep{ algebra::degToRad( 360.0 ) };
    double _majorStart{ 0.0 };
    double _majorSweep{ algebra::degToRad( 720.0 ) };
};

// ======================================================================================
// ==================== TeapotObjectData ================================================
// ======================================================================================

struct TeapotObjectData final : public SceneObjectData {
    Q_GADGET

public:
    TeapotObjectData()
        : SceneObjectData{ SceneObjectTypeEnum::TEAPOT } {};

    TeapotObjectData( const TeapotObjectData &other ) = default;
    TeapotObjectData( TeapotObjectData &&other ) = default;

    TeapotObjectData &operator=( const TeapotObjectData &other ) = default;
    TeapotObjectData &operator=( TeapotObjectData &&other ) = default;

    QJsonObject toJson() const override {
        auto base = SceneObjectData::toJson();

        const auto self = QJsonObject{
            { "segments", _segments },
        };

        base.insert( "derived", self );

        return base;
    };

    void fromJson( const QJsonObject &data ) override {
        SceneObjectData::fromJson( data );

        const auto &derived = data.value( "derived" ).toObject();

        _segments = derived.value( "segments" ).toInt( _segments );
    }

    int _segments{ 8 };
};

}  // namespace tire

Q_DECLARE_METATYPE( tire::SceneObjectTypeEnum )

Q_DECLARE_METATYPE( tire::SceneObjectData )

Q_DECLARE_METATYPE( tire::BoxObjectData )
Q_DECLARE_METATYPE( tire::SphereObjectData )
Q_DECLARE_METATYPE( tire::CylinderObjectData )
Q_DECLARE_METATYPE( tire::CapsuleObjectData )

Q_DECLARE_METATYPE( tire::MeshObjectData )

Q_DECLARE_METATYPE( tire::BezierSurfaceObjectData )
Q_DECLARE_METATYPE( tire::BezierSplineObjectData )

Q_DECLARE_METATYPE( tire::LandscapeObjectData )
Q_DECLARE_METATYPE( tire::PolytopeObjectData )

Q_DECLARE_METATYPE( tire::IcosphereObjectData )
Q_DECLARE_METATYPE( tire::IcosahedronObjectData )
Q_DECLARE_METATYPE( tire::DodecahedronObjectData )
Q_DECLARE_METATYPE( tire::PlaneObjectData )
Q_DECLARE_METATYPE( tire::DiskObjectData )
Q_DECLARE_METATYPE( tire::TubeObjectData )
Q_DECLARE_METATYPE( tire::CappedTubeObjectData )
Q_DECLARE_METATYPE( tire::RoundedBoxObjectData )
Q_DECLARE_METATYPE( tire::SphericalConeObjectData )
Q_DECLARE_METATYPE( tire::TorusKnotObjectData )
Q_DECLARE_METATYPE( tire::SpringObjectData )
Q_DECLARE_METATYPE( tire::TeapotObjectData )
