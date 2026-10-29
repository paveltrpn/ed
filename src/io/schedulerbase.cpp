#include "io/schedulerbase.h"

#include <cstdlib>
#include <new>
#include <stdexcept>

#include "log/log.h"

namespace tire {

SchedulerBase::SchedulerBase()
    : loop_{ static_cast<uv_loop_t *>( std::malloc( sizeof( uv_loop_t ) ) ) } {
    if ( loop_ == nullptr ) {
        throw std::bad_alloc{};
    }

    thread_ = std::make_unique<std::thread>( [this]() -> void {
        auto status = uv_loop_init( loop_ );
        auto loopInitialized = status == 0;

        if ( status == 0 ) {
            asyncWakeup_.data = this;
            status = uv_async_init( loop_, &asyncWakeup_, asyncCallback );
        }

        {
            const std::scoped_lock lock{ _startupMutex };
            _startupStatus = status;
            _startupComplete = true;
        }
        _startupCondition.notify_one();

        if ( status != 0 ) {
            if ( loopInitialized ) {
                uv_loop_close( loop_ );
            }
            return;
        }

        uv_run( loop_, UV_RUN_DEFAULT );

        if ( !uv_is_closing( reinterpret_cast<uv_handle_t *>( &asyncWakeup_ ) ) ) {
            uv_close( reinterpret_cast<uv_handle_t *>( &asyncWakeup_ ), nullptr );
        }
        uv_run( loop_, UV_RUN_DEFAULT );

        const auto closeStatus = uv_loop_close( loop_ );
        if ( closeStatus != 0 ) {
            log::error()( "Failed to close libuv loop: {}", uv_strerror( closeStatus ) );
        }
    } );

    std::unique_lock lock{ _startupMutex };
    _startupCondition.wait( lock, [this]() -> bool { return _startupComplete; } );
    const auto startupStatus = _startupStatus;
    lock.unlock();

    if ( startupStatus != 0 ) {
        thread_->join();
        std::free( loop_ );
        loop_ = nullptr;
        throw std::runtime_error{ uv_strerror( startupStatus ) };
    }
}

SchedulerBase::~SchedulerBase() {
    stop();
    thread_->join();
    std::free( loop_ );
}

auto SchedulerBase::asyncCallback( uv_async_t *handle ) -> void {
    auto *context = static_cast<SchedulerBase *>( handle->data );

    while ( true ) {
        auto item = AsyncJob{};
        if ( !context->dequeue( item ) ) {
            break;
        }
        auto [task, payload] = std::move( item );
        try {
            task( context->loop_, payload );
        } catch ( const std::exception &exception ) {
            log::error()( "Scheduled libuv job failed: {}", exception.what() );
        } catch ( ... ) {
            log::error()( "Scheduled libuv job failed with an unknown exception" );
        }
    }
}

auto SchedulerBase::stop() -> void {
    auto unrefTask = [this]( uv_loop_t *, AsyncJobPayload * ) -> void {
        uv_unref( reinterpret_cast<uv_handle_t *>( &asyncWakeup_ ) );
    };
    enqueue( std::make_pair( std::move( unrefTask ), nullptr ) );
    uv_async_send( &asyncWakeup_ );
}

auto SchedulerBase::enqueue( AsyncJob job ) -> void {
    std::scoped_lock lock{ _jobMutex };
    _jobs.push_back( std::move( job ) );
}

auto SchedulerBase::dequeue( AsyncJob &job ) -> bool {
    std::scoped_lock lock{ _jobMutex };
    if ( _jobs.empty() ) {
        return false;
    }

    job = std::move( _jobs.front() );
    _jobs.pop_front();
    return true;
}

auto SchedulerBase::isLoopThread() const noexcept -> bool {
    return thread_ != nullptr && std::this_thread::get_id() == thread_->get_id();
}

}  // namespace tire
