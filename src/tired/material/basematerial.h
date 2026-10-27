#pragma once

#include "simplematerial.h"

namespace tire {

// ======================================================================================
// ==================== BaseMaterial ====================================================
// ======================================================================================

struct BaseMaterial: public SimpleMaterial {
    BaseMaterial() = default;

    BaseMaterial( const BaseMaterial &other ) = default;
    BaseMaterial( BaseMaterial &&other ) = default;

    auto operator=( const BaseMaterial &other ) -> BaseMaterial & = default;
    auto operator=( BaseMaterial &&other ) -> BaseMaterial & = default;

    virtual ~BaseMaterial() = default;

private:
    float _emission{};
    float _metallic{};
    float _roughness{};
};

}  // namespace tire

