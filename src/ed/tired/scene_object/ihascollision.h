
#pragma once

namespace tire {

struct IHasCollision {
    virtual auto getShape() -> void = 0;
};

}  // namespace tire
