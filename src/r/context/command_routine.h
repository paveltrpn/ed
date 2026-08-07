#pragma once

#include <coroutine>
#include <exception>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

namespace tire {

struct CommandRoutine {
    using value_type = VkCommandBuffer;

    struct promise_type;

    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        auto get_return_object() -> CommandRoutine {
            //
            return CommandRoutine{ handle_type::from_promise( *this ) };
        }

        auto initial_suspend() -> std::suspend_never {
            //
            return {};
        }

        auto final_suspend() noexcept -> std::suspend_never {
            //
            return {};
        }

        auto unhandled_exception() -> void {
            //
            exception_ = std::current_exception();
        }

        auto yield_value( VkCommandBuffer value ) -> std::suspend_always {
            value_ = value;
            return {};
        }

        auto return_void() -> void {
            //
        }

        value_type value_{};
        std::exception_ptr exception_{};
    };

    CommandRoutine() = delete;
    CommandRoutine( handle_type h )
        : h_( h ) {}

    CommandRoutine( const CommandRoutine &other ) = delete;
    CommandRoutine( CommandRoutine &&other ) = delete;

    auto operator=( const CommandRoutine &other ) = delete;
    auto operator=( CommandRoutine &&other ) = delete;

    ~CommandRoutine() {
        //
        post();
    }

    [[nodiscard]] auto buf() const -> VkCommandBuffer { return h_.promise().value_; }

private:
    void post() {
        h_.resume();
        if ( h_.promise().exception_ ) {
            std::rethrow_exception( h_.promise().exception_ );
        }
    }

private:
    handle_type h_;
};

}  // namespace tire
