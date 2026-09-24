
#include "subgraph.h"

namespace tire {

// ======================================================================================
// ==================== Subgraph ========================================================
// ======================================================================================

Subgraph::Subgraph( vsg::observer_ptr<vsg::Viewer> viewer )
    : _viewer{ viewer } {
    //
}

// auto Subgraph::recompile() -> void {
//     auto compileTraversal = vsg::CompileTraversal::create( *_viewer );

//     compileTraversal->compile( _stateGroup );
//     compileTraversal->compile( _outlineStateGroup );

//     {
//         // auto res = _viewer->compileManager->compile( _stateGroup );
//         // auto isUpdateNeeded = res.requiresViewerUpdate( _viewer );
//     }

//     {
//         // _viewer->compileManager->compile( _outlineStateGroup );
//     }

//     // TODO: check is this realy needed!
//     vsg::CompileResult res{};
//     vsg::updateViewer( *_viewer, res );
// }

}  // namespace tire
