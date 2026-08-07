
#pragma once

namespace tire {

struct IHasMaterial {
    virtual auto getMaterial() -> void = 0;
};

}  // namespace tire