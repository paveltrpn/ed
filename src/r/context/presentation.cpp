
#include "presentation.h"

#include "vkinstance.h"
#include "device.h"

namespace tire {

Presentation::Presentation( const VKInstance *instance, const VKDevice *device )
    : _instance{ instance }
    , _device{ device } {
}

Presentation::~Presentation() {
}

}  // namespace tire
