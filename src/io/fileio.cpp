#include "io/fileio.h"

#include <utility>

namespace tire {

FileOpenAwaitable::FileOpenAwaitable( Scheduler& scheduler, std::string path )
    : _scheduler{ &scheduler }
    , _path{ std::move( path ) } {
}

auto FileOpenAwaitable::openCallback( uv_fs_t* request ) -> void {
    //
    auto* awaitable = static_cast<FileOpenAwaitable*>( request->data );
    request->data = nullptr;

    const auto result = request->result;
    uv_fs_req_cleanup( request );

    const auto coroutine = awaitable->complete( static_cast<int>( result ) );
    if ( coroutine ) {
        coroutine.resume();
    }
}

auto FileOpenAwaitable::start( uv_loop_t* loop ) -> void {
    _request.data = this;

    const auto status = uv_fs_open( loop, &_request, _path.c_str(), O_RDONLY, 0, openCallback );
    if ( status < 0 ) {
        _request.data = nullptr;
        uv_fs_req_cleanup( &_request );

        const auto coroutine = complete( status );
        if ( coroutine ) {
            coroutine.resume();
        }
    }
}

auto FileOpenAwaitable::await_ready() const noexcept -> bool {
    //
    return false;
}

auto FileOpenAwaitable::await_suspend( std::coroutine_handle<> coroutine ) noexcept -> void {
    _coroutine = coroutine;

    _scheduler->schedule( [this]( uv_loop_t* loop ) -> void {
        //
        start( loop );
    } );
}

auto FileOpenAwaitable::await_resume() const noexcept -> int {
    //
    return _result._status;
}

auto Scheduler::openFile( const std::string& path ) -> FileOpenAwaitable {
    //
    return FileOpenAwaitable{ *this, path };
}

}  // namespace tire
