
#include "rendervk.h"

namespace tire {

auto RenderVK::handleEvent( std::shared_ptr<EventBase> event ) -> void {
    auto keyEvent = dynamic_cast<EventKey*>( event.get() );
    if ( keyEvent ) {
        switch ( keyEvent->_action ) {
            case tire::KeyAction::PRESS: {
                if ( keyEvent->_key == 71 ) {
                    holdMouse_ = !holdMouse_;
                    return;
                }
                if ( keyEvent->_key == 256 ) {
                    run_ = false;
                    return;
                }
                break;
            }
            case tire::KeyAction::RELEASE: {
                break;
            }
        }
    }
};

}  // namespace tire
