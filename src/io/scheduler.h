#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include <uv.h>

#include "io/async.h"
#include "io/schedulerbase.h"

namespace tire {

template <typename T>
struct TimeoutAwaitable;

template <typename T>
struct FileIOAwaitable;

template <typename T>
struct FilesystemWatchAwaitable;

struct Scheduler final : SchedulerBase {
public:
    template <typename T>
    friend struct TimeoutAwaitable;

    template <typename T>
    friend struct FileIOAwaitable;

    template <typename T>
    friend struct FilesystemWatchAwaitable;

public:
    // Fire once by timeout.
    auto timeout( uint64_t timeout ) -> TimeoutAwaitable<Task<void>>;

    //
    auto openFile( const std::string &path ) -> FileIOAwaitable<Task<void>>;

    //
    auto watchFile( const std::string &path ) -> FilesystemWatchAwaitable<Task<void>>;

private:
    // Submit callback to event loop.
    void schedule( std::function<void( uv_loop_t *, AsyncJobPayload * )> task, AsyncJobPayload *p );
};

}  // namespace tire
