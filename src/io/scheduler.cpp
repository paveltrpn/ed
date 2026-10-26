#include "io/scheduler.h"

#include <utility>

namespace tire {

void Scheduler::schedule( std::function<void( uv_loop_t *, AsyncJobPayload * )> task, AsyncJobPayload *p ) {
    asyncJobQueue_.push( std::make_pair( std::move( task ), p ) );

    // Wakeup event loop and process task queue.
    uv_async_send( &asyncWakeup_ );
}

}  // namespace tire
