#pragma once

#include <vsg/all.h>
#include "vsg/commands/Command.h"

namespace tire {

// ======================================================================================
// ==================== BoundingDraw ====================================================
// ======================================================================================

struct BoundingDraw final : public vsg::Commands {
    BoundingDraw();

    auto setTransform( const vsg::mat4& matrix ) -> void;

private:
    vsg::mat4 _transform{};
    vsg::ref_ptr<vsg::mat4Array> _transformArray{};
};

}  // namespace tire