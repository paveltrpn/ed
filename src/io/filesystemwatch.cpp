#include "io/filesystemwatch.h"

#include <utility>

namespace tire {

FileWatchAwaitableState::FileWatchAwaitableState( Scheduler& scheduler, std::string path )
    : _scheduler{ &scheduler }
    , _path{ std::move( path ) } {
}

auto FileWatchAwaitableState::close( int status ) -> void {
    if ( _closing ) {
        return;
    }

    _closing = true;
    if ( _initialized && !uv_is_closing( reinterpret_cast<uv_handle_t*>( &_watcher ) ) ) {
        uv_fs_event_stop( &_watcher );
        uv_close( reinterpret_cast<uv_handle_t*>( &_watcher ), []( uv_handle_t* handle ) -> void {
            auto* lifetime = static_cast<std::shared_ptr<FileWatchAwaitableState>*>( handle->data );
            auto state = *lifetime;
            delete lifetime;
            handle->data = nullptr;
            state->_initialized = false;
        } );
    }
    finish( status );
}

auto FileWatchAwaitableState::cancel() -> void {
    _scheduler->schedule( [state = shared_from_this()]( uv_loop_t* ) -> void {
        //
        state->close( UV_ECANCELED );
    } );
}

FileWatchAwaitable::FileWatchAwaitable( std::shared_ptr<FileWatchAwaitableState> state )
    : _state{ std::move( state ) } {
}

auto FileWatchAwaitable::await_ready() const noexcept -> bool {
    //
    return false;
}

auto FileWatchAwaitable::await_suspend( std::coroutine_handle<> coroutine ) noexcept -> void {
    _state->_coroutine = coroutine;

    _state->_scheduler->schedule( [state = _state]( uv_loop_t* loop ) -> void {
        //
        if ( state->_closing || state->_completed ) {
            state->finish( UV_ECANCELED );
            return;
        }

        const auto initStatus = uv_fs_event_init( loop, &state->_watcher );
        if ( initStatus != 0 ) {
            state->finish( initStatus );
            return;
        }

        state->_initialized = true;
        state->_lifetime = new std::shared_ptr<FileWatchAwaitableState>{ state };
        state->_watcher.data = state->_lifetime;

        const auto startStatus = uv_fs_event_start(
            &state->_watcher,
            []( uv_fs_event_t* handle, const char*, int events, int status ) -> void {
                //
                auto* lifetime = static_cast<std::shared_ptr<FileWatchAwaitableState>*>( handle->data );
                auto state = *lifetime;
                state->_result->_events = events;
                state->close( status );
            },
            state->_path.c_str(), 0 );
        if ( startStatus != 0 ) {
            state->close( startStatus );
        }
    } );
}

auto FileWatchAwaitable::await_resume() const noexcept -> FileWatchResult {
    //
    return *_state->_result;
}

auto Scheduler::watchFile( const std::string& path ) -> FileWatchAwaitable {
    //
    return FileWatchAwaitable{ std::make_shared<FileWatchAwaitableState>( *this, path ) };
}

}  // namespace tire
