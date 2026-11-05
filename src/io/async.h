#pragma once

#include <string>
#include <cassert>
#include <coroutine>
#include <exception>
#include <memory>
#include <utility>
#include <uv.h>

#include "log/log.h"

namespace tire {

// ======================================================================================
// ==================== Task ============================================================
// ======================================================================================

template <typename T>
struct Task {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct FinalAwaiter {
        [[nodiscard]] auto await_ready() const noexcept -> bool {
            //
            return false;
        }

        auto await_suspend( handle_type handle ) noexcept -> std::coroutine_handle<>;

        auto await_resume() const noexcept -> void {
            //
        }
    };

    struct promise_type {
        auto get_return_object() -> Task {
            //
            return Task{ handle_type::from_promise( *this ) };
        }

        auto initial_suspend() noexcept -> std::suspend_always {
            //
            return std::suspend_always{};
        }

        auto final_suspend() noexcept -> FinalAwaiter {
            //
            return FinalAwaiter{};
        }

        auto return_value( T value ) -> void {
            //
            _value = std::move( value );
        }

        auto unhandled_exception() -> void {
            //
            exception_ = std::current_exception();
        }

        T _value{};
        std::exception_ptr exception_{ nullptr };
        std::coroutine_handle<> _continuation{};
        bool _done{ false };
        bool _started{ false };
    };

    explicit Task( handle_type h )
        : handle_{ h } {}

    Task() = default;

    Task( Task &&t ) noexcept
        : handle_{ t.handle_ } {
        t.handle_ = nullptr;
    }

    auto operator=( Task &&other ) noexcept -> Task & {
        if ( std::addressof( other ) != this ) {
            release();

            handle_ = other.handle_;
            other.handle_ = nullptr;
        }

        return *this;
    }

    Task( const Task & ) = delete;
    auto operator=( const Task & ) -> Task & = delete;

    ~Task() {
        release();
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        //
        return handle_ == nullptr;
    }

    constexpr explicit operator bool() const noexcept {
        //
        return !empty();
    }

    // Start the deferred coroutine, for tasks that are never co_awaited.
    auto start() noexcept -> void {
        if ( handle_ && !handle_.promise()._started ) {
            handle_.promise()._started = true;
            handle_.resume();
        }
    }

    [[nodiscard]] auto await_ready() const noexcept -> bool {
        //
        return handle_ && handle_.promise()._done;
    }

    auto await_suspend( std::coroutine_handle<> continuation ) noexcept -> std::coroutine_handle<> {
        if ( !handle_ ) {
            return continuation;
        }

        handle_.promise()._continuation = continuation;
        handle_.promise()._started = true;

        return handle_;
    }

    auto await_resume() -> T {
        //
        return get();
    }

    [[nodiscard]] auto get() -> T {
        //
        return std::move( handle_.promise()._value );
    }

    [[nodiscard]] auto exception() const -> std::exception_ptr {
        if ( !handle_ ) {
            return std::exception_ptr{ nullptr };
        }

        return handle_.promise().exception_;
    }

private:
    auto release() noexcept -> void {
        if ( handle_ ) {
            // A root Task may be destroyed only before start or after completion.
            assert( !handle_.promise()._started || handle_.promise()._done );
            handle_.destroy();
            handle_ = nullptr;
        }
    }

    handle_type handle_{ nullptr };
};

template <typename T>
auto Task<T>::FinalAwaiter::await_suspend( handle_type handle ) noexcept -> std::coroutine_handle<> {
    auto &promise = handle.promise();
    promise._done = true;

    const auto continuation = std::exchange( promise._continuation, nullptr );
    if ( continuation ) {
        return continuation;
    }

    return std::noop_coroutine();
}

template <>
struct Task<void> {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        auto get_return_object() -> Task;

        auto initial_suspend() noexcept -> std::suspend_never;

        auto final_suspend() noexcept -> std::suspend_always;

        auto return_void() -> void;

        auto unhandled_exception() -> void;

        std::exception_ptr exception_{ nullptr };
    };

    explicit Task( handle_type h );

    Task() = default;

    Task( Task &&t ) noexcept;

    auto operator=( Task &&other ) noexcept -> Task &;

    Task( const Task & ) = delete;
    auto operator=( const Task & ) -> Task & = delete;

    ~Task();

    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        //
        return handle_ == nullptr;
    }

    constexpr explicit operator bool() const noexcept {
        //
        return !empty();
    }

private:
    handle_type handle_{ nullptr };
};

}  // namespace tire
