
#pragma once

#include <vsg/all.h>

#include "sceneobjectbase.h"

namespace tire {

struct TorusKnotObject;

// ======================================================================================
// ==================== TorusKnotNode ===================================================
// ======================================================================================

struct TorusKnotNode final : public SceneObjectNode {
    friend TorusKnotObject;
    friend JsonSerializationVisitor;

    TorusKnotNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                   vsg::vec4 color, int p, int q, int slices, int segments );

    auto serialize( SerializationVisitor *visitor ) -> void override {
        //
        visitor->serialize( this );
    }

private:
    auto init() -> void;

private:
    int _p{ 2 };
    int _q{ 3 };
    int _slices{ 8 };
    int _segments{ 96 };
};

// ======================================================================================
// ==================== TorusKnotObject =================================================
// ======================================================================================

struct TorusKnotObject final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( int p READ p WRITE setP NOTIFY pChanged FINAL )
    Q_PROPERTY( int q READ q WRITE setQ NOTIFY qChanged FINAL )
    Q_PROPERTY( int slices READ slices WRITE setSlices NOTIFY slicesChanged FINAL )
    Q_PROPERTY( int segments READ segments WRITE setSegments NOTIFY segmentsChanged FINAL )

public:
    TorusKnotObject( vsg::ref_ptr<TorusKnotNode> node );

    TorusKnotObject( const TorusKnotObject& other ) = delete;
    TorusKnotObject( TorusKnotObject&& other ) = delete;

    TorusKnotObject& operator=( const TorusKnotObject& other ) = delete;
    TorusKnotObject& operator=( TorusKnotObject&& other ) = delete;

    auto p() const -> int {
        //
        return _node->_p;
    }

    auto q() const -> int {
        //
        return _node->_q;
    }

    auto slices() const -> int {
        //
        return _node->_slices;
    }

    auto segments() const -> int {
        //
        return _node->_segments;
    }

    auto setP( int value ) -> void {
        //
        if ( _node->_p == value ) return;
        _node->_p = value;
        emit pChanged();
    }

    auto setQ( int value ) -> void {
        //
        if ( _node->_q == value ) return;
        _node->_q = value;
        emit qChanged();
    }

    auto setSlices( int value ) -> void {
        //
        if ( _node->_slices == value ) return;
        _node->_slices = value;
        emit slicesChanged();
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
    void pChanged();
    void qChanged();
    void slicesChanged();
    void segmentsChanged();

private:
    auto init() -> void;

private:
    vsg::ref_ptr<TorusKnotNode> _node{};
};

}  // namespace tire
