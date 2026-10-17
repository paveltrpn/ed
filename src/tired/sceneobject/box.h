
#pragma once

#include <vsg/all.h>

#include "sceneobjectbase.h"

namespace tire {

struct BoxObject;

// ======================================================================================
// ==================== BoxNode =========================================================
// ======================================================================================

struct BoxNode final : public SceneObjectNode {
    friend BoxObject;
    friend JsonSerializationVisitor;

    BoxNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
             vsg::vec4 color, vsg::dvec3 size, vsg::ivec3 segments );

    auto serialize( SerializationVisitor *visitor ) -> void override {
        //
        visitor->serialize( this );
    }

private:
    auto init() -> void;

private:
    vsg::dvec3 _size{ 0.5, 0.5, 0.5 };
    vsg::ivec3 _segments{ 1, 1, 1 };
};

// ======================================================================================
// ==================== BoxObject =======================================================
// ======================================================================================

struct BoxObject final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( double sizeX READ sizeX WRITE setSizeX NOTIFY sizeXChanged FINAL )
    Q_PROPERTY( double sizeY READ sizeY WRITE setSizeY NOTIFY sizeYChanged FINAL )
    Q_PROPERTY( double sizeZ READ sizeZ WRITE setSizeZ NOTIFY sizeZChanged FINAL )

    Q_PROPERTY( int segmentsX READ segmentsX WRITE setSegmentsX NOTIFY segmentsXChanged FINAL )
    Q_PROPERTY( int segmentsY READ segmentsY WRITE setSegmentsY NOTIFY segmentsYChanged FINAL )
    Q_PROPERTY( int segmentsZ READ segmentsZ WRITE setSegmentsZ NOTIFY segmentsZChanged FINAL )

public:
    BoxObject( vsg::ref_ptr<BoxNode> node );

    BoxObject( const BoxObject& other ) = delete;
    BoxObject( BoxObject&& other ) = delete;

    BoxObject& operator=( const BoxObject& other ) = delete;
    BoxObject& operator=( BoxObject&& other ) = delete;

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

    auto setSegmentsX( int value ) -> void {
        //
        _node->_segments.x = value;
    }

    auto setSegmentsY( int value ) -> void {
        //
        _node->_segments.y = value;
    }

    auto setSegmentsZ( int value ) -> void {
        //
        _node->_segments.z = value;
    }

    [[nodiscard]] virtual auto node() const -> vsg::ref_ptr<SceneObjectNode> {
        //
        return _node;
    }

signals:
    void sizeXChanged();
    void sizeYChanged();
    void sizeZChanged();

    void segmentsXChanged();
    void segmentsYChanged();
    void segmentsZChanged();

private:
    auto init() -> void;

private:
    vsg::ref_ptr<BoxNode> _node{};
};

}  // namespace tire