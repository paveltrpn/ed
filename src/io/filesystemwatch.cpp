#include "io/filesystemwatch.h"

#include <utility>

namespace tire {

FileWatchAwaitable::FileWatchAwaitable( Scheduler& scheduler, std::string path )
    : _scheduler{ &scheduler }
    , _path{ std::move( path ) } {
}

auto FileWatchAwaitable::eventCallback( uv_fs_event_t* handle, const char*, int events, int status ) -> void {
    //
    auto* awaitable = static_cast<FileWatchAwaitable*>( handle->data );
    if ( awaitable->_closing || awaitable->_completed ) {
        return;
    }

    awaitable->_result._events = events;
    awaitable->close( status );
}

auto FileWatchAwaitable::close( int status ) -> void {
    if ( _closing || _completed ) {
        return;
    }

    _closing = true;
    _result._status = status;
    uv_fs_event_stop( &_watcher );
    uv_close( reinterpret_cast<uv_handle_t*>( &_watcher ), []( uv_handle_t* handle ) -> void {
        auto* awaitable = static_cast<FileWatchAwaitable*>( handle->data );
        handle->data = nullptr;
        awaitable->_initialized = false;

        const auto coroutine = awaitable->complete( awaitable->_result._status );
        if ( coroutine ) {
            coroutine.resume();
        }
    } );
}

auto FileWatchAwaitable::await_ready() const noexcept -> bool {
    //
    return false;
}

auto FileWatchAwaitable::await_suspend( std::coroutine_handle<> coroutine ) noexcept -> void {
    _coroutine = coroutine;

    _scheduler->schedule( [this]( uv_loop_t* loop ) -> void {
        //
        const auto initStatus = uv_fs_event_init( loop, &_watcher );
        if ( initStatus != 0 ) {
            const auto coroutine = complete( initStatus );
            if ( coroutine ) {
                coroutine.resume();
            }
            return;
        }

        _initialized = true;
        _watcher.data = this;

        const auto startStatus = uv_fs_event_start( &_watcher, eventCallback, _path.c_str(), 0 );
        if ( startStatus != 0 ) {
            close( startStatus );
        }
    } );
}

auto FileWatchAwaitable::await_resume() const noexcept -> FileWatchResult {
    //
    return _result;
}

auto Scheduler::watchFile( const std::string& path ) -> FileWatchAwaitable {
    //
    return FileWatchAwaitable{ *this, path };
}

}  // namespace tire
