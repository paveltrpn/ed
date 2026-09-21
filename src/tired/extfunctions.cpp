
#include "extfunctions.h"

#include "log/log.h"

namespace tire {

// ======================================================================================
// ==================== Declarations ====================================================
// ======================================================================================

PFN_vkCmdSetCullModeEXT vkCmdSetCullMode{ nullptr };
PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonMode{ nullptr };

// ======================================================================================
// ==================== initExtFunctions ================================================
// ======================================================================================

auto initExtFunctions( vsg::ref_ptr<vsg::Device> device ) -> void {
    const auto deviceExtensions = device->getExtensions();

    vkCmdSetCullMode = deviceExtensions->vkCmdSetCullMode;

    {
        const auto res = device->getProcAddr( vkCmdSetPolygonMode, "vkCmdSetPolygonModeEXT" );
        if ( !res ) {
            log::warning()( "Can't init \"vkCmdSetPolygonModeEXT\"!" );
        }
    }
}

}  // namespace tire
