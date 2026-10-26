#include "io/timer.h"

namespace tire {

auto Scheduler::timeout( uint64_t timeout ) -> TimeoutAwaitable<Task<void>> {
    //
    return TimeoutAwaitable<Task<void>>{ /* Context */ *this, timeout };
};

}  // namespace tire
