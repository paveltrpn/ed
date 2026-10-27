
#pragma once

#include <cstddef>
#include <initializer_list>

#include "concepts.h"
#include "matrix.h"

namespace tire::algebra {

// ============================================================================
// ======================== matrix2 ===========================================
// ============================================================================

template <typename T>
requires Algebraic<T> struct matrix2 final {
    using value_type = T;
    using self = matrix2<value_type>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    matrix2()
        : _data{ value_type{ 1 } } {}

    matrix2( std::initializer_list<value_type> list )
        : _data{ value_type{ 0 } } {
        size_t i = 0;
        for ( const auto e : list ) {
            data()[i] = e;
            ++i;
        }
    }

    [[nodiscard]] auto operator[]( size_t index ) -> reference { return data()[index]; }

    [[nodiscard]] auto operator[]( size_t index ) const -> const_reference { return data()[index]; }

    [[nodiscard]] auto size() -> size_t { return 4; }

    [[nodiscard]] auto data() -> pointer { return &_data[0][0]; }

    [[nodiscard]] auto data() const -> const_pointer { return &_data[0][0]; }

    // Storage convention: row-major, data()[i*2 + j] is row i, column j.
    [[nodiscard]] auto transpose() -> self { return self{ algebra::transpose( _data ) }; }

    auto transposeSelf() -> void { _data = algebra::transpose( _data ); }

    auto multiply( const self& rhs ) -> void { _data = rhs._data * _data; }

    [[nodiscard]] auto operator*( const self& rhs ) const -> self {
        auto tmp{ *this };
        tmp.multiply( rhs );
        return tmp;
    }

    [[nodiscard]] auto determinant() -> value_type { return algebra::determinant( _data ); }

private:
    explicit matrix2( const matrix<value_type, 2, 2>& data )
        : _data{ data } {}

    matrix<value_type, 2, 2> _data;
};

using matrix2l = matrix2<long long>;
using matrix2f = matrix2<float>;
using matrix2d = matrix2<double>;

}  // namespace tire::algebra
