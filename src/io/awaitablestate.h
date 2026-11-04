#pragma once

#include <coroutine>
#include <memory>
#include <utility>

namespace tire {

// ======================================================================================
// ==================== AwaitableState ==================================================
// ======================================================================================

// CRTP base for all awaitable operation states. It owns the state members
// shared by every awaitable operation - the suspended coroutine handle, the
// result object and the completion flag - and implements the single
// completion idiom finish(status): complete at most once, store the status
// into the result, then resume the awaiting coroutine if one is suspended.
//
// Derived classes carry only the members unique to their operation and add
// operation-specific helpers.

// The enable_shared_from_this base is inherited through this template, so
// every derived state is always managed by std::shared_ptr and may safely
// capture itself in scheduled callbacks via shared_from_this().

template <typename Derived, typename Result>
struct AwaitableState : std::enable_shared_from_this<Derived> {
    auto finish( int status ) -> void {

        if ( _completed ) {
            return;
        }

        _completed = true;
        _result->_status = status;


        const auto coroutine = std::exchange( _coroutine, nullptr );
        if ( coroutine ) {
            coroutine.resume();
        }
    }

public:
    std::coroutine_handle<> _coroutine{};
    std::shared_ptr<Result> _result{ std::make_shared<Result>() };
    bool _completed{ false };
};

}  // namespace tire
