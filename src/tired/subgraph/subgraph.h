
#pragma once

#include <memory.h>
#include <vector>

#include <QObject>

#include <vsg/all.h>

namespace tire {

// ======================================================================================
// ==================== DeferredDeleteOp ================================================
// ======================================================================================

struct DeferredDeleteOp : public vsg::Inherit<vsg::Operation, DeferredDeleteOp> {
    DeferredDeleteOp( vsg::ref_ptr<vsg::Node> node, int frameDelay = 4 )
        : _node( node )
        , _frameDelay( frameDelay ) {}

    void run() override {
        if ( _frameDelay <= 0 ) {
            _node = nullptr;

            vsg::ref_ptr<vsg::Viewer> ref_viewer = _viewer;

            ref_viewer->updateOperations->remove( vsg::ref_ptr<DeferredDeleteOp>( this ) );
        }
        _frameDelay--;
    }

private:
    vsg::observer_ptr<vsg::Viewer> _viewer;
    vsg::ref_ptr<vsg::Node> _node;
    int _frameDelay;
};

// ======================================================================================
// ==================== AttachOp ========================================================
// ======================================================================================

struct AttachOp : public vsg::Inherit<vsg::Operation, AttachOp> {
    AttachOp( vsg::observer_ptr<vsg::Viewer> viewer, vsg::ref_ptr<vsg::Group> attachmentPoint,
              vsg::ref_ptr<vsg::Node> node, const vsg::CompileResult& compileResult )
        : _viewer( viewer )
        , _attachmentPoint( attachmentPoint )
        , _node( node )
        , _compileResult( compileResult ) {}

    vsg::observer_ptr<vsg::Viewer> _viewer;
    vsg::ref_ptr<vsg::Group> _attachmentPoint;
    vsg::ref_ptr<vsg::Node> _node;
    vsg::CompileResult _compileResult;

    void run() override {
        vsg::ref_ptr<vsg::Viewer> ref_viewer = _viewer;

        if ( ref_viewer ) {
            updateViewer( *ref_viewer, _compileResult );
        }

        _attachmentPoint->addChild( _node );
    }
};

// ======================================================================================
// ==================== AttachAndCompileOp ==============================================
// ======================================================================================

struct AttachAndCompileOp : public vsg::Inherit<vsg::Operation, AttachAndCompileOp> {
    AttachAndCompileOp( vsg::observer_ptr<vsg::Viewer> viewer, vsg::ref_ptr<vsg::Group> attachmentPoint,
                        vsg::ref_ptr<vsg::Node> node, const vsg::CompileResult& compileResult )
        : _viewer( viewer )
        , _attachmentPoint( attachmentPoint )
        , _node( node )
        , _compileResult( compileResult ) {}

    void run() override {
        vsg::ref_ptr<vsg::Viewer> ref_viewer = _viewer;

        _attachmentPoint->addChild( _node );

        auto ct = vsg::CompileTraversal::create( *ref_viewer );

        ct->compile( _attachmentPoint );

        // TODO: needot o update viewer?
        // updateViewer( *ref_viewer, _compileResult );
    }

private:
    vsg::observer_ptr<vsg::Viewer> _viewer;
    vsg::ref_ptr<vsg::Group> _attachmentPoint;
    vsg::ref_ptr<vsg::Node> _node;
    vsg::CompileResult _compileResult;
};

// ======================================================================================
// ==================== DetachOp ========================================================
// ======================================================================================

struct DetachOp : public vsg::Inherit<vsg::Operation, DetachOp> {
    DetachOp( vsg::ref_ptr<vsg::Group> detachmentPoint, vsg::ref_ptr<vsg::Node> node )
        : _detachmentPoint( detachmentPoint )
        , _node( node ) {}

    void run() override {
        if ( _detachmentPoint ) {
            auto it = std::find( _detachmentPoint->children.begin(), _detachmentPoint->children.end(), _node );
            if ( it != _detachmentPoint->children.end() ) {
                _detachmentPoint->children.erase( it );

                vsg::ref_ptr<vsg::Viewer> ref_viewer = _viewer;

                ref_viewer->addUpdateOperation( DeferredDeleteOp::create( _node ), vsg::UpdateOperations::ALL_FRAMES );
            }
        }
    }

private:
    vsg::observer_ptr<vsg::Viewer> _viewer;
    vsg::ref_ptr<vsg::Group> _detachmentPoint;
    vsg::ref_ptr<vsg::Node> _node;
};

// void MyVsg::detachSubgraph(vsg::ref_ptr<vsg::Group> detachmentPoint, vsg::ref_ptr<vsg::Node> node)
// {
//     _viewer->addUpdateOperation(opDetach::create(detachmentPoint, node));
// }

// ======================================================================================
// ==================== Subgraph ========================================================
// ======================================================================================

// "StateGroup is the root of the scene/command graph to hold the GraphicsPipeline, and
// binding of Descriptors to decorate the whole graph.
// For example, a vsg::Geometry node alone only defines what to draw. To define
// how it is drawn (shaders, pipeline state), it must be a child of a vsg::StateGroup
// configured with the appropriate ShaderSet and pipeline states."
struct Subgraph : vsg::Group {
    Subgraph( vsg::observer_ptr<vsg::Viewer> viewer );

    virtual auto stateGroups() const -> std::vector<vsg::ref_ptr<vsg::StateGroup>> = 0;

protected:
    vsg::observer_ptr<vsg::Viewer> _viewer{};
};

}  // namespace tire
