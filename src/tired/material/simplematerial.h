#pragma once

#include "algebra/vector4.h"

namespace tire {

// ======================================================================================
// ==================== SimpleMaterial ==================================================
// ======================================================================================

struct SimpleMaterial {
    SimpleMaterial() = default;

    SimpleMaterial( algebra::vector4f baseColor );

    SimpleMaterial( const SimpleMaterial &other ) = default;
    SimpleMaterial( SimpleMaterial &&other ) = default;

    auto operator=( const SimpleMaterial &other ) -> SimpleMaterial & = default;
    auto operator=( SimpleMaterial &&other ) -> SimpleMaterial & = default;

    virtual ~SimpleMaterial() = default;

private:
    algebra::vector4f _baseColor{};
};

}  // namespace tire
