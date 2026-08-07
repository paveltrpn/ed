
#include "subgraph.h"

namespace tire {

Subgraph::Subgraph( vsg::Viewer* viewer )
    : _viewer{ viewer }
    , _stateGroup{ vsg::StateGroup::create() } {
    //
    this->addChild( _stateGroup );
}

auto Subgraph::link( std::shared_ptr<SceneObjectBase> object ) -> void {
    _stateGroup->addChild( object->root() );
    recompile();
}

auto Subgraph::recompile() -> void {
    _viewer->compileManager->compile( _stateGroup );
    vsg::CompileResult res{};
    vsg::updateViewer( *_viewer, res );
}

}  // namespace tire
