#include "io/fileio.h"

#include <utility>

namespace {

using OpenState = tire::FileOpenAwaitableState;

auto openCallback( uv_fs_t* request ) -> void {
    //
    auto* lifetime = static_cast<std::shared_ptr<OpenState>*>( request->data );
    auto state = *lifetime;
    delete lifetime;
    request->data = nullptr;
    state->_started = false;

    const auto result = request->result;
    uv_fs_req_cleanup( request );

    state->finish( static_cast<int>( result ) );
}

auto startOpen( const std::shared_ptr<OpenState>& state, uv_loop_t* loop ) -> void {
    if ( state->_completed ) {
        return;
    }

    state->_lifetime = new std::shared_ptr<OpenState>{ state };
    state->_request.data = state->_lifetime;
    state->_started = true;

    const auto status = uv_fs_open( loop, &state->_request, state->_path.c_str(), O_RDONLY, 0, openCallback );
    if ( status < 0 ) {
        delete state->_lifetime;
        state->_lifetime = nullptr;
        state->_request.data = nullptr;
        state->_started = false;
        state->finish( status );
    }
}

auto cancelOpen( const std::shared_ptr<OpenState>& state ) -> void {
    if ( state->_completed ) {
        return;
    }

    state->finish( UV_ECANCELED );
}

}  // namespace

namespace tire {

FileOpenAwaitableState::FileOpenAwaitableState( Scheduler& scheduler, std::string path )
    : _scheduler{ &scheduler }
    , _path{ std::move( path ) } {
}

auto FileOpenAwaitableState::cancel() -> void {
    _scheduler->schedule( [state = shared_from_this()]( uv_loop_t* ) -> void {
        //
        cancelOpen( state );
    } );
}

FileOpenAwaitable::FileOpenAwaitable( std::shared_ptr<FileOpenAwaitableState> state )
    : _state{ std::move( state ) } {
}

auto FileOpenAwaitable::await_ready() const noexcept -> bool {
    //
    return false;
}

auto FileOpenAwaitable::await_suspend( std::coroutine_handle<> coroutine ) noexcept -> void {
    _state->_coroutine = coroutine;

    _state->_scheduler->schedule( [state = _state]( uv_loop_t* loop ) -> void {
        //
        startOpen( state, loop );
    } );
}

auto FileOpenAwaitable::await_resume() const noexcept -> int {
    //
    return _state->_result->_status;
}

auto Scheduler::openFile( const std::string& path ) -> FileOpenAwaitable {
    //
    return FileOpenAwaitable{ std::make_shared<FileOpenAwaitableState>( *this, path ) };
}

}  // namespace tire
