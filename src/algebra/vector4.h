
#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <tuple>
#include <utility>

#include "concepts.h"
#include "vector.h"
#include "vector3.h"

namespace tire::algebra {

// ============================================================================
// ======================= vector4 ============================================
// ============================================================================

template <typename T>
requires Numerical<T> struct vector4 final {
    using value_type = T;
    using self = vector4<value_type>;

    vector4() = default;

    vector4( value_type x, value_type y, value_type z, value_type w )
        : _data{ x, y, z, w } {}

    vector4( const std::array<value_type, 4>& arr )
        : _data{ arr[0], arr[1], arr[2], arr[3] } {}

    vector4( const std::array<value_type, 3>& arr, value_type w = 0.0 )
        : _data{ arr[0], arr[1], arr[2], w } {}

    vector4( const vector3<value_type> other, value_type w = 0.0 )
        : _data{ other.x(), other.y(), other.z(), w } {}

    vector4( const self& rhs ) = default;

    vector4( vector4&& other ) = default;

    auto operator=( const vector4& other ) -> vector4& = default;
    auto operator=( vector4&& other ) -> vector4& = default;

    auto operator=( const vector3<value_type>& other ) -> vector4& {
        _data[0] = other.x();
        _data[1] = other.y();
        _data[2] = other.z();
        _data[3] = 0.0;

        return *this;
    }

    auto operator=( vector3<value_type>&& other ) -> vector4& {
        _data[0] = other.x();
        _data[1] = other.y();
        _data[2] = other.z();
        _data[3] = 0.0;

        return *this;
    }

    ~vector4() = default;

    auto x() const -> value_type { return _data[0]; }

    auto y() const -> value_type { return _data[1]; }

    auto z() const -> value_type { return _data[2]; }

    auto w() const -> value_type { return _data[3]; }

    auto plus( const self& b ) -> void { _data += b._data; }

    auto minus( const self& b ) -> void { _data -= b._data; }

    template <typename U>
    auto scaleSelf( U factor ) -> void {
        _data[0] *= factor;
        _data[1] *= factor;
        _data[2] *= factor;
        _data[3] *= factor;
    }

    template <typename U>
    auto scale( U factor ) -> self {
        const auto x = _data[0] * factor;
        const auto y = _data[1] * factor;
        const auto z = _data[2] * factor;
        const auto w = _data[3] * factor;
        return { value_type( x ), value_type( y ), value_type( z ), value_type( w ) };
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

    auto data() -> value_type* { return &_data[0]; }

    auto data() const -> const value_type* { return &_data[0]; }

    // std::get overload for structured binding vector4 types
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
    vector<value_type, 4> _data{};
};

using vector4f = vector4<float>;
using vector4d = vector4<double>;

}  // namespace tire::algebra

// NOTE: namespace std!!!
namespace std {

// Structured binding for vector3f
// ============================================================================
// ======================= tuple_size =========================================
// ============================================================================

template <>
struct tuple_size<tire::algebra::vector4f> : std::integral_constant<size_t, 4> {};

// Structured binding for vector3d
// ============================================================================
// ======================= tuple_size =========================================
// ============================================================================

template <>
struct tuple_size<tire::algebra::vector4d> : std::integral_constant<size_t, 4> {};

// ============================================================================
// ====================== tuple_element =======================================
// ============================================================================

template <size_t I, typename T>
requires tire::algebra::Numerical<T> struct tuple_element<I, tire::algebra::vector4<T>> {
    using type = T;
};

}  // namespace std
