
#include <vsg/all.h>

#include "viewer.h"

namespace tire {

// ====================================================================
// ========== Viewer ==================================================
// ====================================================================

auto Viewer::fetchEvents( const vsg::UIEvents& events ) -> void {
    _events.append_range( events );
}

}  // namespace tire
