
#pragma once

#include <vsg/all.h>

#include "sceneobjectbase.h"

namespace tire {

struct PlaneObject;

// ======================================================================================
// ==================== PlaneNode =======================================================
// ======================================================================================

struct PlaneNode final : public SceneObjectNode {
    friend PlaneObject;
    friend JsonSerializationVisitor;

    PlaneNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
               vsg::vec4 color, vsg::dvec2 size, vsg::ivec2 segments );

    auto serialize( SerializationVisitor *visitor ) -> void override {
        //
        visitor->serialize( this );
    }

private:
    auto init() -> void;

private:
    vsg::dvec2 _size{ 0.5, 0.5 };
    vsg::ivec2 _segments{ 1, 1 };
};

// ======================================================================================
// ==================== PlaneObject =====================================================
// ======================================================================================

struct PlaneObject final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( double sizeX READ sizeX WRITE setSizeX NOTIFY sizeXChanged FINAL )
    Q_PROPERTY( double sizeY READ sizeY WRITE setSizeY NOTIFY sizeYChanged FINAL )

    Q_PROPERTY( int segmentsX READ segmentsX WRITE setSegmentsX NOTIFY segmentsXChanged FINAL )
    Q_PROPERTY( int segmentsY READ segmentsY WRITE setSegmentsY NOTIFY segmentsYChanged FINAL )

public:
    PlaneObject( vsg::ref_ptr<PlaneNode> node );

    PlaneObject( const PlaneObject& other ) = delete;
    PlaneObject( PlaneObject&& other ) = delete;

    PlaneObject& operator=( const PlaneObject& other ) = delete;
    PlaneObject& operator=( PlaneObject&& other ) = delete;

    auto sizeX() const -> double {
        //
        return _node->_size.x;
    }

    auto sizeY() const -> double {
        //
        return _node->_size.y;
    }

    auto segmentsX() const -> int {
        //
        return _node->_segments.x;
    }

    auto segmentsY() const -> int {
        //
        return _node->_segments.y;
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

    [[nodiscard]] virtual auto node() const -> vsg::ref_ptr<SceneObjectNode> {
        //
        return _node;
    }

signals:
    void sizeXChanged();
    void sizeYChanged();

    void segmentsXChanged();
    void segmentsYChanged();

private:
    auto init() -> void;

private:
    vsg::ref_ptr<PlaneNode> _node{};
};

}  // namespace tire
