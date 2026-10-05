#pragma once

#include <vsg/app/Viewer.h>

namespace tire {

// ====================================================================
// ========== Viewer ==================================================
// ====================================================================

class Viewer : public vsg::Inherit<vsg::Viewer, Viewer> {
public:
    Viewer() = default;
    auto fetchEvents( const vsg::UIEvents &events ) -> void;
};

}  // namespace tire

EVSG_type_name( tire::Viewer );
