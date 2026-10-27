#pragma once

#include "algebra/vector3.h"

namespace tire {

// ======================================================================================
// ==================== SimpleMaterial ==================================================
// ======================================================================================

struct SimpleMaterial {
    SimpleMaterial() = default;

    SimpleMaterial( algebra::vector3f baseColor, float transparancy );

    SimpleMaterial( const SimpleMaterial &other ) = default;
    SimpleMaterial( SimpleMaterial &&other ) = default;

    auto operator=( const SimpleMaterial &other ) -> SimpleMaterial & = default;
    auto operator=( SimpleMaterial &&other ) -> SimpleMaterial & = default;

    virtual ~SimpleMaterial() = default;

private:
    algebra::vector3f _baseColor{ 1.0f, 1.0f, 1.0f };
    float _transparancy{ 1.0f };
};

}  // namespace tire
