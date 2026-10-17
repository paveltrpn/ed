
#pragma once

#include <vsg/all.h>

#include "sceneobjectbase.h"

namespace tire {

struct RoundedBoxObject;

// ======================================================================================
// ==================== RoundedBoxNode ==================================================
// ======================================================================================

struct RoundedBoxNode final : public SceneObjectNode {
    friend RoundedBoxObject;
    friend JsonSerializationVisitor;

    RoundedBoxNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                    vsg::vec4 color, double radius, vsg::dvec3 size, int slices, vsg::ivec3 segments );

    auto serialize( SerializationVisitor* visitor ) -> void override {
        //
        visitor->serialize( this );
    }

private:
    auto init() -> void;

private:
    double _radius{ 0.1 };
    vsg::dvec3 _size{ 0.4, 0.4, 0.4 };
    int _slices{ 4 };
    vsg::ivec3 _segments{ 1, 1, 1 };
};

// ======================================================================================
// ==================== RoundedBoxObject ================================================
// ======================================================================================

struct RoundedBoxObject final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( double radius READ radius WRITE setRadius NOTIFY radiusChanged FINAL )

    Q_PROPERTY( double sizeX READ sizeX WRITE setSizeX NOTIFY sizeXChanged FINAL )
    Q_PROPERTY( double sizeY READ sizeY WRITE setSizeY NOTIFY sizeYChanged FINAL )
    Q_PROPERTY( double sizeZ READ sizeZ WRITE setSizeZ NOTIFY sizeZChanged FINAL )

    Q_PROPERTY( int slices READ slices WRITE setSlices NOTIFY slicesChanged FINAL )

    Q_PROPERTY( int segmentsX READ segmentsX WRITE setSegmentsX NOTIFY segmentsXChanged FINAL )
    Q_PROPERTY( int segmentsY READ segmentsY WRITE setSegmentsY NOTIFY segmentsYChanged FINAL )
    Q_PROPERTY( int segmentsZ READ segmentsZ WRITE setSegmentsZ NOTIFY segmentsZChanged FINAL )

public:
    RoundedBoxObject( vsg::ref_ptr<RoundedBoxNode> node );

    RoundedBoxObject( const RoundedBoxObject& other ) = delete;
    RoundedBoxObject( RoundedBoxObject&& other ) = delete;

    RoundedBoxObject& operator=( const RoundedBoxObject& other ) = delete;
    RoundedBoxObject& operator=( RoundedBoxObject&& other ) = delete;

    auto radius() const -> double {
        //
        return _node->_radius;
    }

    auto sizeX() const -> double {
        //
        return _node->_size.x;
    }

    auto sizeY() const -> double {
        //
        return _node->_size.y;
    }

    auto sizeZ() const -> double {
        //
        return _node->_size.z;
    }

    auto slices() const -> int {
        //
        return _node->_slices;
    }

    auto segmentsX() const -> int {
        //
        return _node->_segments.x;
    }

    auto segmentsY() const -> int {
        //
        return _node->_segments.y;
    }

    auto segmentsZ() const -> int {
        //
        return _node->_segments.z;
    }

    auto setRadius( double value ) -> void {
        //
        if ( _node->_radius == value ) return;
        _node->_radius = value;
        emit radiusChanged();
    }

    auto setSizeX( double value ) -> void {
        //
        if ( _node->_size.x == value ) return;
        _node->_size.x = value;
        emit sizeXChanged();
    }

    auto setSizeY( double value ) -> void {
        //
        if ( _node->_size.y == value ) return;
        _node->_size.y = value;
        emit sizeYChanged();
    }

    auto setSizeZ( double value ) -> void {
        //
        if ( _node->_size.z == value ) return;
        _node->_size.z = value;
        emit sizeZChanged();
    }

    auto setSlices( int value ) -> void {
        //
        if ( _node->_slices == value ) return;
        _node->_slices = value;
        emit slicesChanged();
    }

    auto setSegmentsX( int value ) -> void {
        //
        if ( _node->_segments.x == value ) return;
        _node->_segments.x = value;
        emit segmentsXChanged();
    }

    auto setSegmentsY( int value ) -> void {
        //
        if ( _node->_segments.y == value ) return;
        _node->_segments.y = value;
        emit segmentsYChanged();
    }

    auto setSegmentsZ( int value ) -> void {
        //
        if ( _node->_segments.z == value ) return;
        _node->_segments.z = value;
        emit segmentsZChanged();
    }

    [[nodiscard]] virtual auto node() const -> vsg::ref_ptr<SceneObjectNode> {
        //
        return _node;
    }

signals:
    void radiusChanged();

    void sizeXChanged();
    void sizeYChanged();
    void sizeZChanged();

    void slicesChanged();

    void segmentsXChanged();
    void segmentsYChanged();
    void segmentsZChanged();

private:
    auto init() -> void;

private:
    vsg::ref_ptr<RoundedBoxNode> _node{};
};

}  // namespace tire
