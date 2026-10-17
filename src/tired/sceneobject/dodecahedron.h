
#pragma once

#include <vsg/all.h>

#include "sceneobjectbase.h"

namespace tire {

struct DodecahedronObject;

// ======================================================================================
// ==================== DodecahedronNode ================================================
// ======================================================================================

struct DodecahedronNode final : public SceneObjectNode {
    friend DodecahedronObject;
    friend JsonSerializationVisitor;

    DodecahedronNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation,
                      vsg::dvec3 scale, vsg::vec4 color, double radius, int segments, int rings );

    auto serialize( SerializationVisitor *visitor ) -> void override {
        //
        visitor->serialize( this );
    }

private:
    auto init() -> void;

private:
    double _radius{ 0.5 };
    int _segments{ 1 };
    int _rings{ 1 };
};

// ======================================================================================
// ==================== DodecahedronObject ==============================================
// ======================================================================================

struct DodecahedronObject final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( double radius READ radius WRITE setRadius NOTIFY radiusChanged FINAL )
    Q_PROPERTY( int segments READ segments WRITE setSegments NOTIFY segmentsChanged FINAL )
    Q_PROPERTY( int rings READ rings WRITE setRings NOTIFY ringsChanged FINAL )

public:
    DodecahedronObject( vsg::ref_ptr<DodecahedronNode> node );

    DodecahedronObject( const DodecahedronObject& other ) = delete;
    DodecahedronObject( DodecahedronObject&& other ) = delete;

    DodecahedronObject& operator=( const DodecahedronObject& other ) = delete;
    DodecahedronObject& operator=( DodecahedronObject&& other ) = delete;

    auto radius() const -> double {
        //
        return _node->_radius;
    }

    auto segments() const -> int {
        //
        return _node->_segments;
    }

    auto rings() const -> int {
        //
        return _node->_rings;
    }

    auto setRadius( double value ) -> void {
        //
        if ( _node->_radius == value ) return;
        _node->_radius = value;
        emit radiusChanged();
    }

    auto setSegments( int value ) -> void {
        //
        if ( _node->_segments == value ) return;
        _node->_segments = value;
        emit segmentsChanged();
    }

    auto setRings( int value ) -> void {
        //
        if ( _node->_rings == value ) return;
        _node->_rings = value;
        emit ringsChanged();
    }

    [[nodiscard]] virtual auto node() const -> vsg::ref_ptr<SceneObjectNode> {
        //
        return _node;
    }

signals:
    void radiusChanged();
    void segmentsChanged();
    void ringsChanged();

private:
    auto init() -> void;

private:
    vsg::ref_ptr<DodecahedronNode> _node{};
};

}  // namespace tire
