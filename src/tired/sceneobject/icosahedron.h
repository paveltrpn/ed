
#pragma once

#include <vsg/all.h>

#include "sceneobjectbase.h"

namespace tire {

struct IcosahedronObject;

// ======================================================================================
// ==================== IcosahedronNode =================================================
// ======================================================================================

struct IcosahedronNode final : public SceneObjectNode {
    friend IcosahedronObject;
    friend JsonSerializationVisitor;

    IcosahedronNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                     vsg::vec4 color, double radius, int segments );

    auto serialize( SerializationVisitor *visitor ) -> void override {
        //
        visitor->serialize( this );
    }

private:
    auto init() -> void;

private:
    double _radius{ 0.5 };
    int _segments{ 1 };
};

// ======================================================================================
// ==================== IcosahedronObject ===============================================
// ======================================================================================

struct IcosahedronObject final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( double radius READ radius WRITE setRadius NOTIFY radiusChanged FINAL )
    Q_PROPERTY( int segments READ segments WRITE setSegments NOTIFY segmentsChanged FINAL )

public:
    IcosahedronObject( vsg::ref_ptr<IcosahedronNode> node );

    IcosahedronObject( const IcosahedronObject& other ) = delete;
    IcosahedronObject( IcosahedronObject&& other ) = delete;

    IcosahedronObject& operator=( const IcosahedronObject& other ) = delete;
    IcosahedronObject& operator=( IcosahedronObject&& other ) = delete;

    auto radius() const -> double {
        //
        return _node->_radius;
    }

    auto segments() const -> int {
        //
        return _node->_segments;
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

    [[nodiscard]] virtual auto node() const -> vsg::ref_ptr<SceneObjectNode> {
        //
        return _node;
    }

signals:
    void radiusChanged();
    void segmentsChanged();

private:
    auto init() -> void;

private:
    vsg::ref_ptr<IcosahedronNode> _node{};
};

}  // namespace tire
