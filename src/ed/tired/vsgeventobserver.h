#pragma once

#include <vsg/all.h>
#include <vsg/app/Viewer.h>
#include <vsg/ui/Keyboard.h>
#include <vsg/ui/PointerEvent.h>
#include <vsg/ui/ScrollWheelEvent.h>
#include <vsg/ui/TouchEvent.h>

namespace tire {

// ======================================================================================
// ==================== VsgEventObserver ================================================
// ======================================================================================

struct VsgEventObserver {
    virtual void handleVsgEvent( vsg::ScrollWheelEvent& scrollWheel ) = 0;
};

// ======================================================================================
// ==================== VsgEventEmitter =================================================
// ======================================================================================

struct VsgEventEmitter {};

}  // namespace tire
