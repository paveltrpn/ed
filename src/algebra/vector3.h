
#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <tuple>
#include <utility>

#include "concepts.h"
#include "vector.h"

namespace tire::algebra {

// ============================================================================
// ======================= vector3 ============================================
// ============================================================================

template <typename T>
requires Numerical<T> struct vector3 final {
    using value_type = T;
    using self = vector3<value_type>;

    vector3() = default;

    vector3( value_type x, value_type y, value_type z )
        : _data{ x, y, z } {}

    vector3( const std::array<value_type, 3>& arr )
        : _data{ arr[0], arr[1], arr[2] } {}

    vector3( const self& rhs ) = default;

    auto x() const -> value_type { return _data[0]; }

    auto y() const -> value_type { return _data[1]; }

    auto z() const -> value_type { return _data[2]; }

    auto plus( const self& b ) -> void { _data += b._data; }

    auto minus( const self& b ) -> void { _data -= b._data; }

    auto inverse() -> self { return { -_data[0], -_data[1], -_data[2] }; }

    template <typename U>
    auto scaleSelf( U factor ) -> void {
        _data[0] *= factor;
        _data[1] *= factor;
        _data[2] *= factor;
    }

    template <typename U>
    auto scale( U factor ) -> self {
        const auto x = _data[0] * factor;
        const auto y = _data[1] * factor;
        const auto z = _data[2] * factor;
        return { value_type( x ), value_type( y ), value_type( z ) };
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

    auto cross( const self& other ) const -> self {
        const auto result = algebra::cross( _data, other._data );
        return { result[0], result[1], result[2] };
    }

    auto operator[]( size_t index ) -> value_type& { return _data[static_cast<int>( index )]; }

    auto operator[]( size_t index ) const -> const value_type& { return _data[static_cast<int>( index )]; }

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

    // Internal data pointer getter
    auto data() -> value_type* { return &_data[0]; }

    auto data() const -> const value_type* { return &_data[0]; }

    // std::get overload for structured binding vector3 types
    template <size_t I>
    auto get() & -> value_type& {
        return _data[I];
    }

    template <size_t I>
    auto get() const& -> const value_type& {
        return _data[I];
    }

    template <size_t I>
    auto get() && -> value_type&& {
        return std::move( _data[I] );
    }

private:
    vector<value_type, 3> _data{};
};

using vector3f = vector3<float>;
using vector3d = vector3<double>;

}  // namespace tire::algebra

// NOTE: namespace std!!!
namespace std {

// Structured binding for vector3f
// ============================================================================
// ======================= tuple_size =========================================
// ============================================================================

template <>
struct tuple_size<tire::algebra::vector3f> : std::integral_constant<size_t, 3> {};

// Structured binding for vector3d
// ============================================================================
// ======================= tuple_size =========================================
// ============================================================================

template <>
struct tuple_size<tire::algebra::vector3d> : std::integral_constant<size_t, 3> {};

// ============================================================================
// ====================== tuple_element =======================================
// ============================================================================

template <size_t I, typename T>
requires tire::algebra::Numerical<T> struct tuple_element<I, tire::algebra::vector3<T>> {
    using type = T;
};

}  // namespace std
