#pragma once

#include <functional>
#include <memory>
#include <thread>
#include <utility>

#include "uv.h"

#include "io/payload.h"
#include "std/queue.h"

namespace tire {

struct SchedulerBase {
    SchedulerBase( const SchedulerBase &other ) = delete;
    SchedulerBase( SchedulerBase &&other ) = delete;

    SchedulerBase();

    auto operator=( const SchedulerBase &other ) -> SchedulerBase & = delete;
    auto operator=( SchedulerBase &&other ) -> SchedulerBase & = delete;

    virtual ~SchedulerBase();

private:
    // This static method executes inside event loop when uv_async_send()
    // is called.
    static auto asyncCallback( uv_async_t *handle ) -> void;

    auto stop() -> void;

protected:
    //
    uv_async_t asyncWakeup_{};
    locking_queue<std::pair<std::function<void( uv_loop_t *, AsyncJobPayload * )>, AsyncJobPayload *>> asyncJobQueue_{
        128 };

private:
    // Main and only uv loop handle.
    uv_loop_t *loop_{};

    // Loop thread.
    std::unique_ptr<std::thread> thread_{};
};

}  // namespace tire
