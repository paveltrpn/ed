
#pragma once

#include <vsg/all.h>

#include "sceneobjectbase.h"

namespace tire {

struct TeapotObject;

// ======================================================================================
// ==================== TeapotNode ======================================================
// ======================================================================================

struct TeapotNode final : public SceneObjectNode {
    friend TeapotObject;
    friend JsonSerializationVisitor;

    TeapotNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                vsg::vec4 color, int segments );

    auto serialize( SerializationVisitor *visitor ) -> void override {
        //
        visitor->serialize( this );
    }

private:
    auto init() -> void;

private:
    int _segments{ 8 };
};

// ======================================================================================
// ==================== TeapotObject ====================================================
// ======================================================================================

struct TeapotObject final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( int segments READ segments WRITE setSegments NOTIFY segmentsChanged FINAL )

public:
    TeapotObject( vsg::ref_ptr<TeapotNode> node );

    TeapotObject( const TeapotObject& other ) = delete;
    TeapotObject( TeapotObject&& other ) = delete;

    TeapotObject& operator=( const TeapotObject& other ) = delete;
    TeapotObject& operator=( TeapotObject&& other ) = delete;

    auto segments() const -> int {
        //
        return _node->_segments;
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
    void segmentsChanged();

private:
    auto init() -> void;

private:
    vsg::ref_ptr<TeapotNode> _node{};
};

}  // namespace tire
