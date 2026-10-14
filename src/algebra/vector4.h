
#pragma once

#include <cstddef>
#include <cmath>
#include <array>

#include "concepts.h"

#include "vector3.h"

namespace tire::algebra {

template <typename T>
requires Algebraic<T> struct vector4 final {
    using value_type = T;
    using self = vector4<value_type>;

    vector4() {
        data_[0] = value_type{};
        data_[1] = value_type{};
        data_[2] = value_type{};
        data_[3] = value_type{};
    }

    vector4( value_type x, value_type y, value_type z, value_type w ) {
        data_[0] = x;
        data_[1] = y;
        data_[2] = z;
        data_[3] = w;
    }

    vector4( const std::array<value_type, 4> &arr ) {
        data_[0] = arr[0];
        data_[1] = arr[1];
        data_[2] = arr[2];
        data_[3] = arr[3];
    }

    vector4( const std::array<value_type, 3> &arr, value_type w = 0.0 ) {
        data_[0] = arr[0];
        data_[1] = arr[1];
        data_[2] = arr[2];
        data_[3] = w;
    }

    vector4( const vector3<value_type> other, value_type w = 0.0 ) {
        data_[0] = other.x();
        data_[1] = other.y();
        data_[2] = other.z();
        data_[3] = w;
    }

    vector4( const self &rhs ) {
        data_[0] = rhs.data_[0];
        data_[1] = rhs.data_[1];
        data_[2] = rhs.data_[2];
        data_[3] = rhs.data_[3];
    }

    vector4( vector4 &&other ) = default;

    auto operator=( const vector4 &other ) -> vector4 & = default;
    auto operator=( vector4 &&other ) -> vector4 & = default;

    auto operator=( const vector3<value_type> &other ) -> vector4 & {
        //
        data_[0] = other.x();
        data_[1] = other.y();
        data_[2] = other.z();
        data_[3] = 0.0;

        return *this;
    }

    auto operator=( vector3<value_type> &&other ) -> vector4 & {
        //
        data_[0] = other.x();
        data_[1] = other.y();
        data_[2] = other.z();
        data_[3] = 0.0;

        return *this;
    }

    ~vector4() = default;

    auto x() const -> value_type {
        //
        return data_[0];
    }

    auto y() const -> value_type {
        //
        return data_[1];
    }

    auto z() const -> value_type {
        //
        return data_[2];
    }

    auto w() const -> value_type {
        //
        return data_[3];
    }

    auto plus( const self &b ) -> void {
        for ( size_t i = 0; i < 4; ++i ) {
            data_[i] += b.data_[i];
        }
    };

    auto minus( const self &b ) -> void {
        for ( size_t i = 0; i < 4; ++i ) {
            data_[i] -= b.data_[i];
        }
    };

    template <typename U>
    void scaleSelf( U factor ) {
        data_[0] *= factor;
        data_[1] *= factor;
        data_[2] *= factor;
        data_[3] *= factor;
    }

    template <typename U>
    auto scale( U factor ) -> self {
        const auto x = data_[0] * factor;
        const auto y = data_[1] * factor;
        const auto z = data_[2] * factor;
        const auto w = data_[3] * factor;
        return { value_type( x ), value_type( y ), value_type( z ), value_type( w ) };
    }

    auto dot( const self &b ) const -> value_type {
        value_type rt{};

        for ( size_t i = 0; i < 4; ++i ) rt += data_[i] * b.data_[i];

        return rt;
    }

    auto sqLength() const -> value_type {
        //
        return dot( *this );
    }

    auto length() -> value_type const {
        // TODO: call proper square root function for float and double
        return std::sqrt( sqLength() );
    }

    auto normalizeSelf() -> void {
        value_type len = length();

        if ( len > 0.0 ) {
            for ( size_t i{}; i < 4; ++i ) {
                data_[i] /= len;
            }
        }
    }

    auto operator[]( size_t index ) -> value_type & {
        //
        return data_[index];
    }

    auto operator[]( size_t index ) const -> const value_type & {
        //
        return data_[index];
    }

    friend auto operator+( const self &lhs, const self &rhs ) -> self {
        auto rt = lhs;
        rt.plus( rhs );
        return rt;
    }

    auto operator+=( const self &rhs ) -> self & {
        ( *this ).plus( rhs );
        return *this;
    }

    friend auto operator-( const self &lhs, const self &rhs ) -> self {
        auto rt = lhs;
        rt.minus( rhs );
        return rt;
    }

    auto operator-=( const self &rhs ) -> self & {
        ( *this ).minus( rhs );
        return *this;
    }

    auto data() -> value_type * {
        //
        return data_.data();
    }

    auto data() const -> const value_type * {
        //
        return data_.data();
    }

    // std::get overload for structured binding vector4 types
    template <size_t I>
    auto get() -> value_type & {
        if constexpr ( I == 0 )
            return data_[0];
        else if constexpr ( I == 1 )
            return data_[1];
        else if constexpr ( I == 2 )
            return data_[2];
        else if constexpr ( I == 3 )
            return data_[3];
    }

    template <size_t I>
    auto get() const -> value_type const & {
        if constexpr ( I == 0 )
            return data_[0];
        else if constexpr ( I == 1 )
            return data_[1];
        else if constexpr ( I == 2 )
            return data_[2];
        else if constexpr ( I == 3 )
            return data_[3];
    }

    template <size_t I>
    auto get() -> value_type && {
        if constexpr ( I == 0 )
            return std::move( data_[0] );
        else if constexpr ( I == 1 )
            return std::move( data_[1] );
        else if constexpr ( I == 2 )
            return std::move( data_[2] );
        else if constexpr ( I == 3 )
            return std::move( data_[3] );
        ;
    }

private:
    std::array<value_type, 4> data_{};
};

using vector4f = vector4<float>;
using vector4d = vector4<double>;

}  // namespace tire::algebra

// NOTE: namespace std!!!
namespace std {

// Structured binding for vector3f
template <>
struct tuple_size<tire::algebra::vector4f> : std::integral_constant<size_t, 4> {};

template <>
struct tuple_element<0, tire::algebra::vector4f> {
    using type = float;
};

template <>
struct tuple_element<1, tire::algebra::vector4f> {
    using type = float;
};

template <>
struct tuple_element<2, tire::algebra::vector4f> {
    using type = float;
};

template <>
struct tuple_element<3, tire::algebra::vector4f> {
    using type = float;
};

// Structured binding for vector3d
template <>
struct tuple_size<tire::algebra::vector4d> : std::integral_constant<size_t, 4> {};

template <>
struct tuple_element<0, tire::algebra::vector4d> {
    using type = double;
};

template <>
struct tuple_element<1, tire::algebra::vector4d> {
    using type = double;
};

template <>
struct tuple_element<2, tire::algebra::vector4d> {
    using type = double;
};

template <>
struct tuple_element<3, tire::algebra::vector4d> {
    using type = double;
};

}  // namespace std
