#include "io/scheduler.h"

#include <utility>

namespace tire {

auto Scheduler::post( std::function<void( uv_loop_t * )> task ) -> void {
    auto scheduledTask = [task = std::move( task )]( uv_loop_t *loop, AsyncJobPayload * ) mutable -> void {
        //
        task( loop );
    };

    schedule( std::move( scheduledTask ), nullptr );
}

void Scheduler::schedule( std::function<void( uv_loop_t *, AsyncJobPayload * )> task, AsyncJobPayload *p ) {
    enqueue( std::make_pair( std::move( task ), p ) );

    // Wakeup event loop and process task queue.
    uv_async_send( &asyncWakeup_ );
}

}  // namespace tire
