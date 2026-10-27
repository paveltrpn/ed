
#include "simplematerial.h"

namespace tire {

// ======================================================================================
// ==================== SimpleMaterial ==================================================
// ======================================================================================

SimpleMaterial::SimpleMaterial( algebra::vector3f baseColor, float transparancy )
    : _baseColor{ baseColor }
    , _transparancy{ transparancy } {
}

}  // namespace tire
