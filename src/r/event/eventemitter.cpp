
#include <stdexcept>

#include "event.h"
#include "eventemitter.h"
#include "eventobserver.h"
#include "log/log.h"

namespace tire {

// ============================================================================
// ============== GlobalEventEmitter ==========================================
// ============================================================================

GlobalEventEmitter::GlobalEventEmitter() {
}

void GlobalEventEmitter::init() {
    if ( _initSuccess ) {
        log::error()( "Warning: Singleton already initialized. Ignoring new arguments." );
    }

    std::call_once( _initFlag, [&]() {
        _instance.store( new GlobalEventEmitter{} );
        _initSuccess = true;
    } );
}

GlobalEventEmitter &GlobalEventEmitter::instance() {
    GlobalEventEmitter *ptr = _instance.load();

    if ( !ptr ) {
        throw std::logic_error( "Singleton must be initialized via init() before calling getInstance()." );
    }
    return *ptr;
}

void GlobalEventEmitter::attach( std::weak_ptr<EventObserver> observer ) {
    _observers.push_back( observer );
}

void GlobalEventEmitter::detach( std::weak_ptr<EventObserver> observer ) {
    // auto it = std::find( _observers.begin(), _observers.end(), observer.lock().get() );
    // if ( it != _observers.end() ) {
    //     _observers.erase( it );
    // }
}

void GlobalEventEmitter::notify( std::shared_ptr<EventBase> event ) {
    for ( auto weakRefObserver : _observers ) {
        auto observer = weakRefObserver.lock();
        observer->handleEvent( event );
    }
}

}  // namespace tire