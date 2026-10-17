
#pragma once

#include <vsg/all.h>

#include "sceneobjectbase.h"

namespace tire {

struct SpringObject;

// ======================================================================================
// ==================== SpringNode ======================================================
// ======================================================================================

struct SpringNode final : public SceneObjectNode {
    friend SpringObject;
    friend JsonSerializationVisitor;

    SpringNode( std::string name, std::string uid, vsg::dvec3 position, vsg::dvec3 orientation, vsg::dvec3 scale,
                vsg::vec4 color, double minor, double major, double size, int slices, int segments, double minorStart,
                double minorSweep, double majorStart, double majorSweep );

    auto serialize( SerializationVisitor *visitor ) -> void override {
        //
        visitor->serialize( this );
    }

private:
    auto init() -> void;

private:
    double _minor{ 0.25 };
    double _major{ 1.0 };
    double _size{ 1.0 };
    int _slices{ 8 };
    int _segments{ 32 };
    double _minorStart{ 0.0 };
    double _minorSweep{ gml::radians( 360.0 ) };
    double _majorStart{ 0.0 };
    double _majorSweep{ gml::radians( 720.0 ) };
};

// ======================================================================================
// ==================== SpringObject ====================================================
// ======================================================================================

struct SpringObject final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( double minor READ minor WRITE setMinor NOTIFY minorChanged FINAL )
    Q_PROPERTY( double major READ major WRITE setMajor NOTIFY majorChanged FINAL )
    Q_PROPERTY( double size READ size WRITE setSize NOTIFY sizeChanged FINAL )
    Q_PROPERTY( int slices READ slices WRITE setSlices NOTIFY slicesChanged FINAL )
    Q_PROPERTY( int segments READ segments WRITE setSegments NOTIFY segmentsChanged FINAL )
    Q_PROPERTY( double minorStart READ minorStart WRITE setMinorStart NOTIFY minorStartChanged FINAL )
    Q_PROPERTY( double minorSweep READ minorSweep WRITE setMinorSweep NOTIFY minorSweepChanged FINAL )
    Q_PROPERTY( double majorStart READ majorStart WRITE setMajorStart NOTIFY majorStartChanged FINAL )
    Q_PROPERTY( double majorSweep READ majorSweep WRITE setMajorSweep NOTIFY majorSweepChanged FINAL )

public:
    SpringObject( vsg::ref_ptr<SpringNode> node );

    SpringObject( const SpringObject& other ) = delete;
    SpringObject( SpringObject&& other ) = delete;

    SpringObject& operator=( const SpringObject& other ) = delete;
    SpringObject& operator=( SpringObject&& other ) = delete;

    auto minor() const -> double {
        //
        return _node->_minor;
    }

    auto major() const -> double {
        //
        return _node->_major;
    }

    auto size() const -> double {
        //
        return _node->_size;
    }

    auto slices() const -> int {
        //
        return _node->_slices;
    }

    auto segments() const -> int {
        //
        return _node->_segments;
    }

    auto minorStart() const -> double {
        //
        return _node->_minorStart;
    }

    auto minorSweep() const -> double {
        //
        return _node->_minorSweep;
    }

    auto majorStart() const -> double {
        //
        return _node->_majorStart;
    }

    auto majorSweep() const -> double {
        //
        return _node->_majorSweep;
    }

    auto setMinor( double value ) -> void {
        //
        if ( _node->_minor == value ) return;
        _node->_minor = value;
        emit minorChanged();
    }

    auto setMajor( double value ) -> void {
        //
        if ( _node->_major == value ) return;
        _node->_major = value;
        emit majorChanged();
    }

    auto setSize( double value ) -> void {
        //
        if ( _node->_size == value ) return;
        _node->_size = value;
        emit sizeChanged();
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

    auto setMinorStart( double value ) -> void {
        //
        if ( _node->_minorStart == value ) return;
        _node->_minorStart = value;
        emit minorStartChanged();
    }

    auto setMinorSweep( double value ) -> void {
        //
        if ( _node->_minorSweep == value ) return;
        _node->_minorSweep = value;
        emit minorSweepChanged();
    }

    auto setMajorStart( double value ) -> void {
        //
        if ( _node->_majorStart == value ) return;
        _node->_majorStart = value;
        emit majorStartChanged();
    }

    auto setMajorSweep( double value ) -> void {
        //
        if ( _node->_majorSweep == value ) return;
        _node->_majorSweep = value;
        emit majorSweepChanged();
    }

    [[nodiscard]] virtual auto node() const -> vsg::ref_ptr<SceneObjectNode> {
        //
        return _node;
    }

signals:
    void minorChanged();
    void majorChanged();
    void sizeChanged();
    void slicesChanged();
    void segmentsChanged();
    void minorStartChanged();
    void minorSweepChanged();
    void majorStartChanged();
    void majorSweepChanged();

private:
    auto init() -> void;

private:
    vsg::ref_ptr<SpringNode> _node{};
};

}  // namespace tire
