#pragma once

#include <coroutine>
#include <utility>

namespace tire {

// ======================================================================================
// ==================== AwaitableState ==================================================
// ======================================================================================

template <typename Result>
struct AwaitableState {
    auto complete( int status ) -> std::coroutine_handle<> {
        if ( _completed ) {
            return {};
        }

        _completed = true;
        _result._status = status;

        return std::exchange( _coroutine, nullptr );
    }

public:
    std::coroutine_handle<> _coroutine{};
    Result _result{};
    bool _completed{ false };
};

}  // namespace tire
