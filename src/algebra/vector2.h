
#pragma once

#include <array>
#include <cmath>
#include <cstddef>

#include "concepts.h"
#include "vector.h"

namespace tire::algebra {

// ============================================================================
// ====================== vector2 =============================================
// ============================================================================

template <typename T>
requires Algebraic<T> struct vector2 final {
    using value_type = T;
    using self = vector2<value_type>;

    vector2() = default;

    vector2( value_type x, value_type y )
        : _data{ x, y } {}

    vector2( const std::array<value_type, 2>& arr )
        : _data{ arr[0], arr[1] } {}

    vector2( const self& rhs ) = default;

    auto x() -> value_type { return _data[0]; }

    auto y() -> value_type { return _data[1]; }

    auto plus( const self& b ) -> void { _data += b._data; }

    auto minus( const self& b ) -> void { _data -= b._data; }

    template <typename U>
    auto scaleSelf( U factor ) -> void {
        _data[0] *= factor;
        _data[1] *= factor;
    }

    template <typename U>
    auto scale( U factor ) -> self {
        const auto x = _data[0] * factor;
        const auto y = _data[1] * factor;
        return { value_type( x ), value_type( y ) };
    }

    auto dot( const self& b ) const -> value_type { return algebra::dot( _data, b._data ); }

    auto sqLength() const -> value_type { return dot( *this ); }

    auto length() -> value_type const {
        // TODO: call proper square root function for float and double
        return algebra::length( _data );
    }

    auto normalizeSelf() -> void {
        const value_type len = length();

        if ( len > 0.0 ) {
            _data = algebra::normalize( _data );
        }
    }

    friend auto operator+( const self& lhs, const self& rhs ) -> self {
        auto rt = lhs;
        rt.plus( rhs );
        return rt;
    }

    auto operator+=( const self& rhs ) -> self& {
        ( *this ).plus( rhs );
        return *this;
    }

    friend auto operator-( const self& lhs, const self& rhs ) -> self {
        auto rt = lhs;
        rt.minus( rhs );
        return rt;
    }

    auto operator-=( const self& rhs ) -> self& {
        ( *this ).minus( rhs );
        return *this;
    }

    auto data() -> value_type* { return &_data[0]; }

    auto data() const -> const value_type* { return &_data[0]; }

private:
    vector<value_type, 2> _data{};
};

using vector2l = vector2<long long>;
using vector2f = vector2<float>;
using vector2d = vector2<double>;

}  // namespace tire::algebra
