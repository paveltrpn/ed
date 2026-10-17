
#include "serialization.h"

#include <QJsonArray>

#include "sceneobjectnode.h"
#include "box.h"
#include "sphere.h"
#include "cylinder.h"
#include "capsule.h"
#include "cone.h"
#include "torus.h"

namespace tire {

// ======================================================================================
// ==================== JsonSerializationVisitor ========================================
// ======================================================================================

[[nodiscard]] auto JsonSerializationVisitor::json() const -> QJsonObject {
    //
    return _json;
}

auto JsonSerializationVisitor::baseData( SceneObjectNode *object ) const -> QJsonObject {
    //
    auto &&positionArr = QJsonArray{ object->_position.x, object->_position.y, object->_position.z };
    auto &&orientationArr = QJsonArray{ object->_orientation.x, object->_orientation.y, object->_orientation.z };
    auto &&scaleArr = QJsonArray{ object->_scale.x, object->_scale.y, object->_scale.z };
    auto &&colorArr = QJsonArray{ object->_color.x, object->_color.y, object->_color.z, object->_color.w };

    return { { "type", static_cast<int>( object->_type ) },
             { "name", QString::fromStdString( object->_name ) },
             { "uid", QString::fromStdString( object->_uid ) },
             { "position", positionArr },
             { "orientation", orientationArr },
             { "scale", scaleArr },
             { "color", colorArr } };
}

auto JsonSerializationVisitor::serialize( BoxNode *object ) -> void {
    //
    auto json = baseData( object );

    const auto derived = QJsonObject{
        { "sizeX", object->_size.x },         { "sizeY", object->_size.y },
        { "sizeZ", object->_size.z },         { "segmentsX", object->_segments.x },
        { "segmentsY", object->_segments.y }, { "segmentsZ", object->_segments.z },
    };

    json.insert( "derived", derived );

    _json = json;
}

auto JsonSerializationVisitor::serialize( SphereNode *object ) -> void {
    //
    auto json = baseData( object );

    const auto derived = QJsonObject{
        { "radius", object->_radius },
        { "slices", object->_slices },
        { "segments", object->_segments },
        { "sliceStart", object->_sliceStart },
        { "sliceSweep", object->_sliceSweep },
        { "segmentStart", object->_segmentStart },
        { "segmentSweep", object->_segmentSweep },
    };

    json.insert( "derived", derived );

    _json = json;
}

auto JsonSerializationVisitor::serialize( CylinderNode *object ) -> void {
    //
    auto json = baseData( object );

    const auto derived = QJsonObject{
        { "radius", object->_radius },     { "size", object->_size },   { "slices", object->_slices },
        { "segments", object->_segments }, { "rings", object->_rings }, { "start", object->_start },
        { "sweep", object->_sweep },
    };

    json.insert( "derived", derived );

    _json = json;
}

auto JsonSerializationVisitor::serialize( CapsuleNode *object ) -> void {
    //
    auto json = baseData( object );

    const auto derived = QJsonObject{
        { "radius", object->_radius },     { "size", object->_size },   { "slices", object->_slices },
        { "segments", object->_segments }, { "rings", object->_rings }, { "start", object->_start },
        { "sweep", object->_sweep },
    };

    json.insert( "derived", derived );

    _json = json;
}

auto JsonSerializationVisitor::serialize( ConeNode *object ) -> void {
    //
    auto json = baseData( object );

    const auto derived = QJsonObject{
        { "radius", object->_radius },     { "size", object->_size },   { "slices", object->_slices },
        { "segments", object->_segments }, { "rings", object->_rings }, { "start", object->_start },
        { "sweep", object->_sweep },
    };

    json.insert( "derived", derived );

    _json = json;
}

auto JsonSerializationVisitor::serialize( TorusNode *object ) -> void {
    //
    auto json = baseData( object );

    const auto derived = QJsonObject{
        { "minor", object->_minor },           { "major", object->_major },
        { "slices", object->_slices },         { "segments", object->_segments },
        { "minorStart", object->_minorStart }, { "minorSweep", object->_minorSweep },
        { "majorStart", object->_majorStart }, { "majorSweep", object->_majorSweep },
    };

    json.insert( "derived", derived );

    _json = json;
}

}  // namespace tire
