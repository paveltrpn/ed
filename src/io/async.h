#pragma once

#include <string>
#include <coroutine>
#include <exception>
#include <uv.h>

#include "log/log.h"

namespace tire {

template <typename T>
struct Task;

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

    [[nodiscard]] constexpr auto empty() const noexcept -> bool;

    constexpr explicit operator bool() const noexcept;

private:
    handle_type handle_{ nullptr };
};

}  // namespace tire
