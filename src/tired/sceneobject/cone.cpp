
#include <vsg/all.h>

#include "cone.h"
#include "../generatorutils.h"

namespace tire::object {

Cone::Cone( const ConeObjectData& data )
    : SceneObjectBase{ data }
    , _data{ data } {
    init();
}

auto Cone::init() -> void {
}

}  // namespace tire::object