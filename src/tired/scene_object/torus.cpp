
#include <vsg/all.h>

#include "torus.h"
#include "../generatorutils.h"

namespace tire::object {

Torus::Torus( const TorusObjectData& data )
    : SceneObjectBase{ data }
    , _data{ data } {
    init();
}

auto Torus::init() -> void {
}

}  // namespace tire::object