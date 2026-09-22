
#include "subgraph.h"

namespace tire {

// ======================================================================================
// ==================== Subgraph ========================================================
// ======================================================================================

Subgraph::Subgraph( vsg::Viewer* viewer )
    : _viewer{ viewer }
    , _stateGroup{ vsg::StateGroup::create() } {
    //

    _outlineStateGroup = vsg::StateGroup::create();

    this->addChild( _outlineStateGroup );
    this->addChild( _stateGroup );
}

auto Subgraph::link( std::shared_ptr<SceneObjectBase> object ) -> void {
    _stateGroup->addChild( object->node() );
    recompile();
}

auto Subgraph::recompile() -> void {
    _viewer->compileManager->compile( _stateGroup );
    _viewer->compileManager->compile( _outlineStateGroup );

    vsg::CompileResult res{};
    vsg::updateViewer( *_viewer, res );
}

}  // namespace tire
