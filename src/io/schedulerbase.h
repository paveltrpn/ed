#pragma once

#include <condition_variable>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

#include "uv.h"

#include "io/payload.h"

namespace tire {

struct SchedulerBase {
    using AsyncJob = std::pair<std::function<void( uv_loop_t *, AsyncJobPayload * )>, AsyncJobPayload *>;

    SchedulerBase( const SchedulerBase &other ) = delete;
    SchedulerBase( SchedulerBase &&other ) = delete;

    SchedulerBase();

    auto operator=( const SchedulerBase &other ) -> SchedulerBase & = delete;
    auto operator=( SchedulerBase &&other ) -> SchedulerBase & = delete;

    virtual ~SchedulerBase();

protected:
    [[nodiscard]] auto isLoopThread() const noexcept -> bool;
    auto enqueue( AsyncJob job ) -> void;

private:
    static auto asyncCallback( uv_async_t *handle ) -> void;

    auto dequeue( AsyncJob &job ) -> bool;
    auto stop() -> void;

protected:
    uv_async_t asyncWakeup_{};

private:
    uv_loop_t *loop_{};
    std::unique_ptr<std::thread> thread_{};
    std::mutex _startupMutex{};
    std::condition_variable _startupCondition{};
    bool _startupComplete{ false };
    int _startupStatus{ 0 };
    std::mutex _jobMutex{};
    std::deque<AsyncJob> _jobs{};
};

}  // namespace tire
