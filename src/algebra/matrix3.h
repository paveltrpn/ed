
#pragma once

#include <cmath>
#include <cstddef>
#include <initializer_list>

#include "concepts.h"
#include "matrix.h"
#include "vector3.h"

namespace tire::algebra {

// ============================================================================
// ======================== matrix3 ===========================================
// ============================================================================

template <typename T>
requires Numerical<T> struct matrix3 final {
    using value_type = T;
    using self = matrix3<value_type>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    matrix3()
        : _data{ value_type{ 1 } } {}

    matrix3( std::initializer_list<value_type> list )
        : _data{ value_type{ 0 } } {
        size_t i = 0;
        for ( const auto e : list ) {
            _data[static_cast<int>( i / 3 )][static_cast<int>( i % 3 )] = e;
            ++i;
        }
    }

    [[nodiscard]] auto operator[]( size_t index ) -> reference {
        return _data[static_cast<int>( index / 3 )][static_cast<int>( index % 3 )];
    }

    [[nodiscard]] auto operator[]( size_t index ) const -> const_reference {
        return _data[static_cast<int>( index / 3 )][static_cast<int>( index % 3 )];
    }

    [[nodiscard]] auto size() const -> size_t { return 9; }

    [[nodiscard]] auto data() -> pointer { return &_data[0][0]; }

    [[nodiscard]] auto data() const -> const_pointer { return &_data[0][0]; }

    auto zero() -> void { _data = matrix<value_type, 3, 3>{}; }

    auto idtt() -> void { _data = matrix<value_type, 3, 3>{ value_type{ 1 } }; }

    // Storage convention: row-major, data()[i*3 + j] is row i, column j.
    [[nodiscard]] auto transpose() -> self { return self{ algebra::transpose( _data ) }; }

    auto transposeSelf() -> void { _data = algebra::transpose( _data ); }

    auto multiply( const self& rhs ) -> void { _data = rhs._data * _data; }

    auto operator*( const self& rhs ) const -> self {
        auto tmp{ *this };
        tmp.multiply( rhs );
        return tmp;
    }

    auto mult_vector3( const vector3<value_type>& v ) -> vector3<value_type> {
        const auto canonicalVector = vector<value_type, 3>{ v[0], v[1], v[2] };
        const auto result = algebra::transpose( _data ) * canonicalVector;
        return { result[0], result[1], result[2] };
    }

    // NOTE: LLM generated.
    [[nodiscard]] auto determinant() -> value_type { return algebra::determinant( _data ); }

    auto rotation( value_type yaw, value_type pitch, value_type roll ) -> void {
        auto siny = std::sin( yaw );
        auto cosy = std::cos( yaw );
        auto sinp = std::sin( pitch );
        auto cosp = std::cos( pitch );
        auto sinr = std::sin( roll );
        auto cosr = std::cos( roll );

        _data[0][0] = cosy * cosr - siny * cosp * sinr;
        _data[0][1] = -cosy * sinr - siny * cosp * cosr;
        _data[0][2] = siny * sinp;

        _data[1][0] = siny * cosr + cosy * cosp * sinr;
        _data[1][1] = -siny * sinr + cosy * cosp * cosr;
        _data[1][2] = -cosy * sinp;

        _data[2][0] = sinp * sinr;
        _data[2][1] = sinp * cosr;
        _data[2][2] = cosp;
    }

private:
    explicit matrix3( const matrix<value_type, 3, 3>& data )
        : _data{ data } {}

    matrix<value_type, 3, 3> _data;
};

using matrix3l = matrix3<long long>;
using matrix3f = matrix3<float>;
using matrix3d = matrix3<double>;

}  // namespace tire::algebra
