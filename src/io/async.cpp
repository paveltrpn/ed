#include "io/async.h"

#include <memory>

namespace tire {

auto Task<void>::promise_type::get_return_object() -> Task {
    //
    return Task<void>{ handle_type::from_promise( *this ) };
}

auto Task<void>::promise_type::initial_suspend() noexcept -> std::suspend_never {
    return std::suspend_never{};
}

auto Task<void>::promise_type::final_suspend() noexcept -> std::suspend_always {
    return std::suspend_always{};
}

auto Task<void>::promise_type::return_void() -> void {
    //
}

auto Task<void>::promise_type::unhandled_exception() -> void {
    //
    exception_ = std::current_exception();
}

Task<void>::Task( handle_type h )
    : handle_( h ) {}

Task<void>::Task( Task &&t ) noexcept
    : handle_( t.handle_ ) {
    t.handle_ = nullptr;
}

auto Task<void>::operator=( Task &&other ) noexcept -> Task & {
    if ( std::addressof( other ) != this ) {
        // Destroy this handle, but it allowed only when
        // coroutine is suspended.
        if ( handle_ ) {
            handle_.destroy();
        }

        handle_ = other.handle_;
        other.handle_ = nullptr;
    }

    return *this;
}

Task<void>::~Task() {
    if ( handle_ ) {
        handle_.destroy();
    }
}

constexpr auto Task<void>::empty() const noexcept -> bool {
    //
    return handle_ == nullptr;
}

constexpr Task<void>::operator bool() const noexcept {
    //
    return !empty();
}

}  // namespace tire
