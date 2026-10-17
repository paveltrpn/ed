
#pragma once

#include <vsg/all.h>

#include "sceneobjectbase.h"

namespace tire {

struct IcosphereObject;

// ======================================================================================
// ==================== IcosphereNode ===================================================
// ======================================================================================

struct IcosphereNode final : public SceneObjectNode {
    friend IcosphereObject;
    friend JsonSerializationVisitor;

    IcosphereNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                   vsg::vec4 color, double radius, int segments );

    auto serialize( SerializationVisitor *visitor ) -> void override {
        //
        visitor->serialize( this );
    }

private:
    auto init() -> void;

private:
    double _radius{ 0.5 };
    int _segments{ 4 };
};

// ======================================================================================
// ==================== IcosphereObject =================================================
// ======================================================================================

struct IcosphereObject final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( double radius READ radius WRITE setRadius NOTIFY radiusChanged FINAL )
    Q_PROPERTY( int segments READ segments WRITE setSegments NOTIFY segmentsChanged FINAL )

public:
    IcosphereObject( vsg::ref_ptr<IcosphereNode> node );

    IcosphereObject( const IcosphereObject& other ) = delete;
    IcosphereObject( IcosphereObject&& other ) = delete;

    IcosphereObject& operator=( const IcosphereObject& other ) = delete;
    IcosphereObject& operator=( IcosphereObject&& other ) = delete;

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
    vsg::ref_ptr<IcosphereNode> _node{};
};

}  // namespace tire
