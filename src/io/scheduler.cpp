#include "io/scheduler.h"

#include <cstdlib>
#include <new>
#include <stdexcept>
#include <utility>

#include "log/log.h"

namespace tire {

Scheduler::Scheduler()
    : _loop{ static_cast<uv_loop_t*>( std::malloc( sizeof( uv_loop_t ) ) ) } {
    if ( _loop == nullptr ) {
        throw std::bad_alloc{};
    }

    _thread = std::make_unique<std::thread>( [this]() -> void {
        auto status = uv_loop_init( _loop );
        auto loopInitialized = status == 0;

        if ( status == 0 ) {
            _asyncWakeup.data = this;
            status = uv_async_init( _loop, &_asyncWakeup, asyncCallback );
        }

        {
            const std::scoped_lock lock{ _startupMutex };
            _startupStatus = status;
            _startupComplete = true;
        }

        _startupCondition.notify_one();

        if ( status != 0 ) {
            if ( loopInitialized ) {
                uv_loop_close( _loop );
            }

            return;
        }

        // Далее стандартная идиома корректного завершения libuv loop:
        // первый uv_run — рабочая фаза...
        uv_run( _loop, UV_RUN_DEFAULT );

        if ( !uv_is_closing( reinterpret_cast<uv_handle_t*>( &_asyncWakeup ) ) ) {
            uv_close( reinterpret_cast<uv_handle_t*>( &_asyncWakeup ), nullptr );
        }

        // ... второй uv_run — фаза «сброса» pending close-колбэков.
        uv_run( _loop, UV_RUN_DEFAULT );

        const auto closeStatus = uv_loop_close( _loop );
        if ( closeStatus != 0 ) {
            log::error()( "Failed to close libuv loop: {}", uv_strerror( closeStatus ) );
        }
    } );

    std::unique_lock lock{ _startupMutex };

    // Sleep until uv loop thread fully initialzed.
    _startupCondition.wait( lock, [this]() -> bool {
        //
        return _startupComplete;
    } );

    const auto startupStatus = _startupStatus;
    lock.unlock();

    // When uv loop thread fully initialized and error occured - we throw!.
    if ( startupStatus != 0 ) {
        _thread->join();

        std::free( _loop );

        _loop = nullptr;

        throw std::runtime_error{ uv_strerror( startupStatus ) };
    }
}

Scheduler::~Scheduler() {
    stop();

    _thread->join();

    std::free( _loop );
}

auto Scheduler::asyncCallback( uv_async_t* handle ) -> void {
    auto* context = static_cast<Scheduler*>( handle->data );

    while ( true ) {
        auto item = std::function<void( uv_loop_t* )>{};

        if ( !context->dequeue( item ) ) {
            break;
        }

        try {
            item( context->_loop );
        } catch ( const std::exception& exception ) {
            log::error()( "Scheduled libuv job failed: {}", exception.what() );
        }
    }
}

auto Scheduler::stop() -> void {
    auto unrefTask = [this]( uv_loop_t* ) -> void {
        //
        uv_unref( reinterpret_cast<uv_handle_t*>( &_asyncWakeup ) );
    };

    schedule( std::move( unrefTask ) );
}

auto Scheduler::dequeue( std::function<void( uv_loop_t* )>& job ) -> bool {
    std::scoped_lock lock{ _jobMutex };

    if ( _jobs.empty() ) {
        return false;
    }

    job = std::move( _jobs.front() );
    _jobs.pop_front();

    return true;
}

auto Scheduler::schedule( std::function<void( uv_loop_t* )> task ) -> void {
    {
        std::scoped_lock lock{ _jobMutex };
        _jobs.push_back( std::move( task ) );
    }

    // Wakeup event loop and process task queue.
    uv_async_send( &_asyncWakeup );
}

}  // namespace tire
