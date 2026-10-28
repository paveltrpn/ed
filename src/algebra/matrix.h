// Portions of this code belongs to GML linear algebra library by Markus Ilmola.
// Distributed under the Boost Software License, Version 1.0.

#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include "concepts.h"
#include "quaternion.h"
#include "vector.h"

namespace tire::algebra {

// ============================================================================
// ======================= matrix =============================================
// ============================================================================

/**
 * A matrix with C Columns and R rows
 * @tparam C Number of columns
 * @tparam R Number of rows (length of column vectors)
 */
template <typename T, int C, int R>
requires Numerical<T>&& Positive<C>&& Positive<R> class matrix {
public:
    /**
     * The type of a single component (Not the type of single column!)
     */
    using value_type = T;

    /**
     * Initialize all components to zero.
     */
    matrix() = default;

    /**
     * Initialize the diagonal to given value and 0 elsewhere
     */
    explicit matrix( const T& a ) {
        for ( int i = 0; i < std::min( C, R ); ++i ) {
            _data[i][i] = a;
        }
    }

    /**
     * Initialize all columns to v.
     */
    explicit matrix( const vector<T, R>& v ) {
        for ( int i = 0; i < C; ++i ) {
            _data[i] = v;
        }
    }

    /**
     * Initialize from C column vectors with R components each.
     */
    template <typename... Args>
        requires( sizeof...( Args ) == C ) && ( std::convertible_to<Args, vector<T, R>> && ... )
                                                  matrix( const Args&... args )
        : _data{ args... } {}

    /**
     * Initialize from a c-array.
     * The array must have at least C*R components or behaviour is undefined.
     * If data is null assertion error will occur.
     * @param data Pointer to the first element.
     * @param columnMajor Are the components in the input in column major order
     */
    matrix( const T* data, bool columnMajor ) {
        assert( data != nullptr );
        if ( columnMajor ) {
            for ( int i = 0; i < C; ++i ) {
                _data[i] = vector<T, R>( &data[i * R] );
            }
        } else {
            for ( int i = 0; i < C; ++i ) {
                for ( int j = 0; j < R; ++j ) {
                    _data[i][j] = data[j * C + i];
                }
            }
        }
    }

    /**
     * Initialize from a smaller matrix by adding given value to the diagonal
     * and zero elsewhere.
     */
    template <int CC, int RR>
        requires Positive<CC>&& Positive<RR> &&
        ( CC < C || RR < R ) explicit matrix( const matrix<T, CC, RR>& m, const T& a = T{ 0 } ) {
        for ( int i = 0; i < std::min( C, CC ); ++i ) {
            _data[i] = vector<T, R>{ m[i] };
        }
        for ( int i = std::min( CC, RR ); i < std::min( C, R ); ++i ) {
            _data[i][i] = a;
        }
    }

    /**
     * Initialize from bigger matrix by dropping trailing rows and columns.
     */
    template <int CC, int RR>
        requires Positive<CC>&& Positive<RR> &&
        ( ( CC != C || RR != R ) && CC >= C && RR >= R ) explicit matrix( const matrix<T, CC, RR>& m ) {
        for ( int i = 0; i < C; ++i ) {
            _data[i] = vector<T, R>{ m[i] };
        }
    }

    /**
     * Creates a sub matrix by removing a row and a column
     * @param m Input matrix.
     * @param col Zero based index of the column to remove
     * @param row Zero based index of the row to remove
     */
    matrix( const matrix<T, C + 1, R + 1>& m, int col, int row ) {
        assert( col <= C && row <= R );
        for ( int i = 0; i < C; ++i ) {
            if ( i < col ) {
                _data[i] = vector<T, R>{ m[i], row };
            } else {
                _data[i] = vector<T, R>{ m[i + 1], row };
            }
        }
    }

    matrix( const matrix& ) = default;

    matrix( matrix&& ) = default;

    auto operator=( const matrix& ) -> matrix& = default;

    auto operator=( matrix&& ) -> matrix& = default;

    /**
     * Returns a reference to the i:th column vector.
     * If i is not in the range [0, C) and assertion failure will occur.
     */
    auto operator[]( int i ) const noexcept -> const vector<T, R>& {
        assert( i >= 0 && i < C );
        return _data[i];
    }

    /**
     * Returns a reference to the i:th column vector.
     * If i is not in the range [0, C) and assertion failure will occur.
     */
    auto operator[]( int i ) noexcept -> vector<T, R>& {
        assert( i >= 0 && i < C );
        return _data[i];
    }

    /**
     * Component-wise sum
     */
    auto operator+( const matrix<T, C, R>& m ) const -> matrix<T, C, R> {
        matrix<T, C, R> temp{ *this };
        temp += m;
        return temp;
    }

    /**
     * Component-wise subtraction
     */
    auto operator-( const matrix<T, C, R>& m ) const -> matrix<T, C, R> {
        matrix<T, C, R> temp{ *this };
        temp -= m;
        return temp;
    }

    /**
     * Returns the product of matrices
     */
    template <int N>
    requires Positive<N> auto operator*( const matrix<T, N, C>& m ) const -> matrix<T, N, R> {
        matrix<T, N, R> temp;
        for ( int i = 0; i < N; ++i ) {
            for ( int r = 0; r < R; ++r ) {
                for ( int c = 0; c < C; ++c ) {
                    temp[i][r] += _data[c][r] * m[i][c];
                }
            }
        }
        return temp;
    }

    /**
     * The product of a matrix and a vector as if the vector was a column of a matrix.
     * Note: you can't multiply 4x4-matrix and 3-vector. Use transform instead.
     */
    auto operator*( const vector<T, C>& v ) const -> vector<T, R> {
        vector<T, R> temp;
        for ( int r = 0; r < R; ++r ) {
            for ( int c = 0; c < C; ++c ) {
                temp[r] += _data[c][r] * v[c];
            }
        }
        return temp;
    }

    /**
     * Returns the product of a matrix and a scalar
     */
    auto operator*( const T& a ) const -> matrix<T, C, R> {
        matrix<T, C, R> temp{ *this };
        temp *= a;
        return temp;
    }

    /**
     * Component-wise sum
     */
    auto operator+=( const matrix<T, C, R>& m ) -> matrix<T, C, R>& {
        for ( int i = 0; i < C; ++i ) {
            _data[i] += m._data[i];
        }
        return *this;
    }

    /**
     * Component wise subtraction
     */
    auto operator-=( const matrix<T, C, R>& m ) -> matrix<T, C, R>& {
        for ( int i = 0; i < C; ++i ) {
            _data[i] -= m._data[i];
        }
        return *this;
    }

    /**
     * Same as M = M * a
     */
    auto operator*=( const T& a ) -> matrix<T, C, R>& {
        for ( int i = 0; i < C; ++i ) {
            _data[i] *= a;
        }
        return *this;
    }

    /**
     * Same as M = M * m
     */
    auto operator*=( const matrix<T, C, R>& m ) -> matrix<T, C, R>& requires( C == R ) {
        *this = *this * m;
        return *this;
    }

    /**
     * Matrices are equal if all corresponding components are equal.
     */
    auto operator==( const matrix<T, C, R>& m ) const -> bool {
        for ( int i = 0; i < C; ++i ) {
            if ( _data[i] != m._data[i] ) {
                return false;
            }
        }
        return true;
    }

    /**
     * Matrices are not equal if any of the corresponding components are not equal
     */
    auto operator!=( const matrix<T, C, R>& m ) const -> bool {
        for ( int i = 0; i < C; ++i ) {
            if ( _data[i] != m._data[i] ) {
                return true;
            }
        }
        return false;
    }

    /**
     * Returns pointer to the first component.
     * The matrix data is in column major order
     */
    auto data() const noexcept -> const T* { return _data[0].data(); }

    /**
     * Returns the number of columns in the matrix
     * (NOT the number of components)
     * This is the largest value that can be given to the [] -operator.
     */
    static auto size() noexcept -> int { return C; }

    /**
     * Iterator to the first column
     */
    auto begin() noexcept -> vector<T, R>* { return _data.data(); }

    auto begin() const noexcept -> const vector<T, R>* { return _data.data(); }

    /**
     * Iterator to the one past the last column
     */
    auto end() noexcept -> vector<T, R>* { return _data.data() + C; }

    auto end() const noexcept -> const vector<T, R>* { return _data.data() + C; }

private:
    std::array<vector<T, R>, C> _data{};
};

/**
 * Multiplies all components of the matrix with a scalar
 */
template <typename T, int C, int R>
requires Numerical<T>&& Positive<C>&& Positive<R> auto operator*( const T& a, const matrix<T, C, R>& m )
    -> matrix<T, C, R> {
    matrix<T, C, R> temp;
    for ( int i = 0; i < C; ++i ) {
        temp[i] = a * m[i];
    }
    return temp;
}

/**
 * Prints the matrix to a stream inside brackets columns separated by a comma.
 */
template <typename T, int C, int R>
requires Numerical<T>&& Positive<C>&& Positive<R> auto operator<<( std::ostream& os, const matrix<T, C, R>& m )
    -> std::ostream& {
    os << '(';
    for ( int i = 0; i < C; ++i ) {
        if ( i > 0 ) {
            os << ',';
        }
        os << m[i];
    }
    os << ')';
    return os;
}

/**
 * Read matrix from a stream.
 * The matrix must be inside brackets columns separeted by a comma.
 */
template <typename T, int C, int R>
requires Numerical<T>&& Positive<C>&& Positive<R> auto operator>>( std::istream& is, matrix<T, C, R>& m )
    -> std::istream& {
    char tmp;
    is >> tmp;
    for ( int i = 0; i < C; ++i ) {
        is >> m[i];
        is >> tmp;
    }
    return is;
}

/**
 * Converts a matrix to std::string.
 */
template <typename T, int C, int R>
requires Numerical<T>&& Positive<C>&& Positive<R> auto toString( const matrix<T, C, R>& m ) -> std::string {
    std::stringstream ss{};
    ss << m;
    return ss.str();
}

/**
 * Returns the transpose of the matrix.
 */
template <typename T, int C, int R>
requires Numerical<T>&& Positive<C>&& Positive<R> auto transpose( const matrix<T, C, R>& m ) -> matrix<T, R, C> {
    matrix<T, R, C> temp;
    for ( int i = 0; i < C; ++i ) {      // C
        for ( int j = 0; j < R; ++j ) {  // R
            temp[j][i] = m[i][j];
        }
    }
    return temp;
}

/**
 * Component wise multiplication of matrices
 */
template <typename T, int C, int R>
requires Numerical<T>&& Positive<C>&& Positive<R> auto matrixCompMult( const matrix<T, C, R>& m1,
                                                                       const matrix<T, C, R>& m2 ) -> matrix<T, C, R> {
    matrix<T, C, R> temp;
    for ( int i = 0; i < C; ++i ) {
        temp[i] = m1[i] * m2[i];
    }
    return temp;
}

/**
 * Treats the first vector as matrix with one column and second as matrix with one row
 */
template <typename T, int C, int R>
requires Numerical<T>&& Positive<C>&& Positive<R> auto outerProduct( const vector<T, R>& v1, const vector<T, C>& v2 )
    -> matrix<T, C, R> {
    matrix<T, C, R> temp;
    for ( int i = 0; i < C; ++i ) {
        for ( int j = 0; j < R; ++j ) {
            temp[i][j] = v1[j] * v2[i];
        }
    }
    return temp;
}

/**
 * Calculates the first minor (determinant of a submatrix)
 * @param col Index of the column to remove (must be [0, N - 1])
 * @param row Index of the row to remove (must be [0, N - 1])
 */
template <typename T, int N>
    requires Numerical<T> && ( N > 1 ) auto firstMinor( const matrix<T, N, N>& m, int col, int row ) -> T {
    return determinant( matrix<T, N - 1, N - 1>{ m, col, row } );
}

template <typename T>
requires Numerical<T> auto firstMinor( const matrix<T, 3, 3>& m, int col, int row ) -> T {
    assert( col >= 0 && col < 3 );
    assert( row >= 0 && row < 3 );

    int c0 = 0;
    if ( col == 0 ) {
        c0 = 1;
    }
    int c1 = 2;
    if ( col == 2 ) {
        c1 = 1;
    }
    int r0 = 0;
    if ( row == 0 ) {
        r0 = 1;
    }
    int r1 = 2;
    if ( row == 2 ) {
        r1 = 1;
    }

    return m[c0][r0] * m[c1][r1] - m[c1][r0] * m[c0][r1];
}

template <typename T>
requires Numerical<T> auto firstMinor( const matrix<T, 2, 2>& m, int col, int row ) -> T {
    assert( col >= 0 && col < 2 );
    assert( row >= 0 && row < 2 );

    return m[!col][!row];
}

/**
 * Computes the determinant of a matrix
 */
template <typename T, int N>
    requires Numerical<T> && ( N > 1 ) auto determinant( const matrix<T, N, N>& m ) -> T {
    T det = 0;
    for ( int i = 0; i < N; ++i ) {
        if ( i % 2 == 0 ) {
            det += m[i][0] * firstMinor( m, i, 0 );
        } else {
            det -= m[i][0] * firstMinor( m, i, 0 );
        }
    }
    return det;
}

/**
 * Computes the determinant of a 1x1 matrix
 */
template <typename T>
requires Numerical<T> auto determinant( const matrix<T, 1, 1>& m ) -> T {
    return m[0][0];
}

/**
 * Computes the determinant of a 2x2 matrix
 */
template <typename T>
requires Numerical<T> auto determinant( const matrix<T, 2, 2>& m ) -> T {
    return m[0][0] * m[1][1] - m[1][0] * m[0][1];
}

/**
 * Computes the inverse of a matrix
 */
template <typename T, int N>
    requires Numerical<T> && ( N > 1 ) auto inverse( const matrix<T, N, N>& m ) -> matrix<T, N, N> {
    matrix<T, N, N> temp;
    const T a = determinant( m );
    for ( int i = 0; i < N; ++i ) {
        for ( int j = 0; j < N; ++j ) {
            if ( ( i + j ) % 2 == 0 ) {
                temp[j][i] = firstMinor( m, i, j ) / a;
            } else {
                temp[j][i] = -firstMinor( m, i, j ) / a;
            }
        }
    }
    return temp;
}

/**
 * Computes the inverse of a 1x1 matrix
 */
template <typename T>
requires Numerical<T> auto inverse( const matrix<T, 1, 1>& m ) -> matrix<T, 1, 1> {
    return matrix<T, 1, 1>{ T{ 1 } / m[0][0] };
}

/**
 * Generates a translation matrix so that transform(translate(v), p) = p + v
 * This the same matrix that would generated by glTranslate
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto translate( const vector<T, N>& v ) -> matrix<T, N + 1, N + 1> {
    matrix<T, N + 1, N + 1> result{ T{ 1 } };
    for ( int i = 0; i < N; ++i ) {
        result[N][i] = v[i];
    }
    return result;
}

/**
 * Generates a scaling matrix so that transform(scale(v), p) = v * p
 * This the same matrix that would generated by glScale
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto scale( const vector<T, N>& v ) -> matrix<T, N + 1, N + 1> {
    matrix<T, N + 1, N + 1> result{ T{ 1 } };
    for ( int i = 0; i < N; ++i ) {
        result[i][i] = v[i];
    }
    return result;
}

/**
 * Generates rotations matrix so that
 */
template <typename T>
requires Numerical<T> auto rotate( const T& angle ) -> matrix<T, 3, 3> {
    using std::cos;
    using std::sin;

    const T s = sin( angle );
    const T c = cos( angle );

    const std::array<T, 9> data = { c, -s, T{ 0 }, s, c, T{ 0 }, T{ 0 }, T{ 0 }, T{ 1 } };
    return matrix<T, 3, 3>{ data.data(), false };
}

/**
 * Generates rotation matrix from Eular angles
 */
template <typename T>
requires Numerical<T> auto rotate( const vector<T, 3>& angle ) -> matrix<T, 4, 4> {
    using std::cos;
    using std::sin;

    const T sy = sin( angle[2] );
    const T cy = cos( angle[2] );
    const T sp = sin( angle[1] );
    const T cp = cos( angle[1] );
    const T sr = sin( angle[0] );
    const T cr = cos( angle[0] );

    const std::array<T, 16> data = { cp * cy,
                         sr * sp * cy + cr * -sy,
                         cr * sp * cy + -sr * -sy,
                         T{ 0 },
                         cp * sy,
                         sr * sp * sy + cr * cy,
                         cr * sp * sy + -sr * cy,
                         T{ 0 },
                         -sp,
                         sr * cp,
                         cr * cp,
                         T{ 0 },
                         T{ 0 },
                         T{ 0 },
                         T{ 0 },
                         T{ 1 } };
    return matrix<T, 4, 4>{ data.data(), false };
}

/**
 * Generates rotation matrix from angle and axis
 */
template <typename T>
requires Numerical<T> auto rotate( const T& angle, const vector<T, 3>& axis ) -> matrix<T, 4, 4> {
    using std::cos;
    using std::sin;

    const T rcos = cos( angle );
    const T rsin = sin( angle );
    const T rcos_1 = T{ 1 } - rcos;
    const T xz = axis[0] * axis[2];
    const T yz = axis[1] * axis[2];
    const T xy = axis[0] * axis[1];
    const T xx = axis[0] * axis[0];
    const T yy = axis[1] * axis[1];
    const T zz = axis[2] * axis[2];

    const std::array<T, 16> data = { rcos + xx * rcos_1,
                         -axis[2] * rsin + xy * rcos_1,
                         axis[1] * rsin + xz * rcos_1,
                         T{ 0 },
                         axis[2] * rsin + xy * rcos_1,
                         rcos + yy * rcos_1,
                         -axis[0] * rsin + yz * rcos_1,
                         T{ 0 },
                         -axis[1] * rsin + xz * rcos_1,
                         axis[0] * rsin + yz * rcos_1,
                         rcos + zz * rcos_1,
                         T{ 0 },
                         T{ 0 },
                         T{ 0 },
                         T{ 0 },
                         T{ 1 } };
    return matrix<T, 4, 4>{ data.data(), false };
}

/**
 * Generates rotation matrix from a rotation quaternion
 */
template <typename T>
requires Numerical<T> auto rotate( const quaternion<T>& q ) -> matrix<T, 4, 4> {
    const T xx = q.imag()[0] * q.imag()[0];
    const T xy = q.imag()[0] * q.imag()[1];
    const T xz = q.imag()[0] * q.imag()[2];
    const T xw = q.imag()[0] * q.real();
    const T yy = q.imag()[1] * q.imag()[1];
    const T yz = q.imag()[1] * q.imag()[2];
    const T yw = q.imag()[1] * q.real();
    const T zz = q.imag()[2] * q.imag()[2];
    const T zw = q.imag()[2] * q.real();

    const std::array<T, 16> data = { T{ 1 } - T{ 2 } * ( yy + zz ),
                         T{ 2 } * ( xy - zw ),
                         T{ 2 } * ( xz + yw ),
                         T{ 0 },
                         T{ 2 } * ( xy + zw ),
                         T{ 1 } - T{ 2 } * ( xx + zz ),
                         T{ 2 } * ( yz - xw ),
                         T{ 0 },
                         T{ 2 } * ( xz - yw ),
                         T{ 2 } * ( yz + xw ),
                         T{ 1 } - T{ 2 } * ( xx + yy ),
                         T{ 0 },
                         T{ 0 },
                         T{ 0 },
                         T{ 0 },
                         T{ 1 } };
    return matrix<T, 4, 4>{ data.data(), false };
}

/**
 * Generates a TRS matrix. Same as translate(translation) *
 * rotate(angle, axis) * scale(scaling), but potentially faster.
 */
template <typename T>
requires Numerical<T> auto translateRotateScale( const vector<T, 3>& translation, T angle, const vector<T, 3>& axis,
                                                 const vector<T, 3>& scaling ) -> matrix<T, 4, 4> {
    return translateRotateScale( translation, quaternionRotate( angle, axis ), scaling );
}

/**
 * Generates a TRS matrix. Same as translate(translation) *
 * rotate(rotation) * scale(scaling), but potentially faster.
 */
template <typename T>
requires Numerical<T> auto translateRotateScale( const vector<T, 3>& translation, const quaternion<T>& rotation,
                                                 const vector<T, 3>& scaling ) -> matrix<T, 4, 4> {
    matrix<T, 4, 4> m = rotate( rotation );

    m[0] *= scaling[0];
    m[1] *= scaling[1];
    m[2] *= scaling[2];

    m[3][0] = translation[0];
    m[3][1] = translation[1];
    m[3][2] = translation[2];

    return m;
}

/**
 * Decomposes a rotation matrix to a rotation quaternion.
 * The input matrix is assumed to be a valid rotation matrix.
 */
template <typename T>
requires Numerical<T> auto decomposeRotationQuaternion( const matrix<T, 4, 4>& m ) -> quaternion<T> {
    using std::sqrt;

    const T one = static_cast<T>( 1 );
    const T two = static_cast<T>( 2 );

    const T trace = m[0][0] + m[1][1] + m[2][2];

    if ( trace > static_cast<T>( 0 ) ) {
        const T s = static_cast<T>( 0.5 ) / sqrt( trace + one );

        return quaternion<T>{
            static_cast<T>( 0.25 ) / s,
            vector<T, 3>{ ( m[1][2] - m[2][1] ) * s, ( m[2][0] - m[0][2] ) * s, ( m[0][1] - m[1][0] ) * s } };
    } else if ( m[0][0] > m[1][1] && m[0][0] > m[2][2] ) {
        const T s = two * sqrt( one + m[0][0] - m[1][1] - m[2][2] );

        return quaternion<T>{
            ( m[1][2] - m[2][1] ) / s,
            vector<T, 3>{ static_cast<T>( 0.25 ) * s, ( m[1][0] + m[0][1] ) / s, ( m[2][0] + m[0][2] ) / s } };
    } else if ( m[1][1] > m[2][2] ) {
        const T s = two * sqrt( one + m[1][1] - m[0][0] - m[2][2] );

        return quaternion<T>{
            ( m[2][0] - m[0][2] ) / s,
            vector<T, 3>{ ( m[1][0] + m[0][1] ) / s, static_cast<T>( 0.25 ) * s, ( m[2][1] + m[1][2] ) / s } };
    } else {
        const T s = two * sqrt( one + m[2][2] - m[0][0] - m[1][1] );

        return quaternion<T>{
            ( m[0][1] - m[1][0] ) / s,
            vector<T, 3>{ ( m[2][0] + m[0][2] ) / s, ( m[2][1] + m[1][2] ) / s, static_cast<T>( 0.25 ) * s } };
    }
}

/**
 * Decomposes a rotation matrix to an angle and a axis.
 * The input matrix is assumed to be a valid rotation matrix.
 */
template <typename T>
requires Numerical<T> auto decomposeRotate( const matrix<T, 4, 4>& m ) -> std::tuple<T, vector<T, 3>> {
    return decomposeRotate( decomposeRotationQuaternion( m ) );
}

/**
 * Decomposes translate, rotate, scale -matrix to translation, angle, axis and scale.
 */
template <typename T>
requires Numerical<T> auto decomposeTrs( const matrix<T, 4, 4>& m )
    -> std::tuple<vector<T, 3>, T, vector<T, 3>, vector<T, 3>> {
    std::tuple<vector<T, 3>, T, vector<T, 3>, vector<T, 3>> temp{};

    quaternion<T> q{};
    std::tie( std::get<0>( temp ), q, std::get<3>( temp ) ) = decomposeTrsQuaternion( m );

    std::tie( std::get<1>( temp ), std::get<2>( temp ) ) = decomposeRotate( q );

    return temp;
}

/**
 * Decomposes translate, rotate, scale -matrix to translation, rotation quaternion and scale.
 */
template <typename T>
requires Numerical<T> auto decomposeTrsQuaternion( const matrix<T, 4, 4>& m )
    -> std::tuple<vector<T, 3>, quaternion<T>, vector<T, 3>> {
    matrix<T, 4, 4> temp = m;

    T s = static_cast<T>( 1 );
    if ( firstMinor( m, 3, 3 ) < 0 ) {
        s = static_cast<T>( -1 );
    }

    vector<T, 3> scaling{ s * length( m[0] ), length( m[1] ), length( m[2] ) };

    temp[0] /= scaling[0];
    temp[1] /= scaling[1];
    temp[2] /= scaling[2];

    return std::make_tuple( vector<T, 3>{ m[3] }, decomposeRotationQuaternion( temp ), scaling );
}

/**
 * Generates a perspective projection matrix.
 * This is the same matrix that glFrustum would generate.
 */
template <typename T>
requires Numerical<T> auto frustum( const T& left, const T& right, const T& bottom, const T& top, const T& zNear,
                                    const T& zFar ) -> matrix<T, 4, 4> {
    const T dX = right - left;
    const T dY = top - bottom;
    const T dZ = zFar - zNear;
    const std::array<T, 16> data = { ( T{ 2 } * zNear ) / dX,
                         T{ 0 },
                         ( right + left ) / dX,
                         T{ 0 },
                         T{ 0 },
                         ( T{ 2 } * zNear ) / dY,
                         ( top + bottom ) / dY,
                         T{ 0 },
                         T{ 0 },
                         T{ 0 },
                         -( zFar + zNear ) / dZ,
                         ( T{ -2 } * zFar * zNear ) / dZ,
                         T{ 0 },
                         T{ 0 },
                         T{ -1 },
                         T{ 0 } };
    return matrix<T, 4, 4>{ data.data(), false };
}

/**
 * Returns a perspective projection matrix.
 * This is the same matrix that gluPerspective would generate.
 */
template <typename T>
requires Numerical<T> auto perspectiveRadians( const T& fovy, const T& aspect, const T& zNear, const T& zFar )
    -> matrix<T, 4, 4> {
    using std::tan;
    const T ymax = zNear * tan( fovy / T{ 2 } );
    const T ymin = -ymax;
    const T xmin = ymin * aspect;
    const T xmax = ymax * aspect;
    return frustum( xmin, xmax, ymin, ymax, zNear, zFar );
}

/**
 * Returns an orthographic projection matrix
 * This is the same matrix that glOrtho would generate.
 */
template <typename T>
requires Numerical<T> auto ortho( const T& left, const T& right, const T& bottom, const T& top, const T& zNear,
                                  const T& zFar ) -> matrix<T, 4, 4> {
    const T dX = right - left;
    const T dY = top - bottom;
    const T dZ = zFar - zNear;
    const std::array<T, 16> data = {
        T{ 2 } / dX, T{ 0 }, T{ 0 },       -( right + left ) / dX, T{ 0 }, T{ 2 } / dY, T{ 0 }, -( top + bottom ) / dY,
        T{ 0 },      T{ 0 }, T{ -2 } / dZ, -( zFar + zNear ) / dZ, T{ 0 }, T{ 0 },      T{ 0 }, T{ 1 } };
    return matrix<T, 4, 4>{ data.data(), false };
}

/**
 * Returns an orthographic projection matrix
 */
template <typename T>
requires Numerical<T> auto ortho2D( const T& left, const T& right, const T& bottom, const T& top ) -> matrix<T, 4, 4> {
    return ortho( left, right, bottom, top, T{ -1 }, T{ 1 } );
}

/**
 * Returns same matrix as gluLookAt would generate
 */
template <typename T>
requires Numerical<T> auto lookAt( const vector<T, 3>& eye, const vector<T, 3>& center, const vector<T, 3>& up )
    -> matrix<T, 4, 4> {
    const vector<T, 3> z = normalize( center - eye );
    const vector<T, 3> x = normalize( cross( z, up ) );
    const vector<T, 3> y = cross( x, z );

    const std::array<T, 16> data = { x[0],  x[1],  x[2],  T{ 0 }, y[0],   y[1],   y[2],   T{ 0 },
                         -z[0], -z[1], -z[2], T{ 0 }, T{ 0 }, T{ 0 }, T{ 0 }, T{ 1 } };
    return matrix<T, 4, 4>{ data.data(), false } * translate( -eye );
}

/**
 * Map object coordinates to window coordinates (same as gluProject).
 * @param v Object coordinates to project.
 * @param modelView The model view matrix
 * @param proj The projection matrix
 * @param viewportOrigin Lower left corner of the viewport.
 * @param viewportSize Size of the viewport
 */
template <typename T, typename TI, typename TS>
requires Numerical<T>&& Numerical<TI>&& Numerical<TS> auto project( const vector<T, 3>& v,
                                                                    const matrix<T, 4, 4>& modelView,
                                                                    const matrix<T, 4, 4>& proj,
                                                                    const vector<TI, 2>& viewportOrigin,
                                                                    const vector<TS, 2>& viewportSize )
    -> vector<T, 3> {
    return project( v, proj * modelView, viewportOrigin, viewportSize );
}

/**
 * Map object coordinates to window coordinates (same as gluProject).
 * @param v Object coordinates to project.
 * @param modelViewProj The model view projection matrix (proj * modelView).
 * @param viewportOrigin Lower left corner of the viewport.
 * @param viewportSize Size of the viewport
 */
template <typename T, typename TI, typename TS>
requires Numerical<T>&& Numerical<TI>&& Numerical<TS> auto project( const vector<T, 3>& v,
                                                                    const matrix<T, 4, 4>& modelViewProj,
                                                                    const vector<TI, 2>& viewportOrigin,
                                                                    const vector<TS, 2>& viewportSize )
    -> vector<T, 3> {
    vector<T, 4> in = modelViewProj * vector<T, 4>{ v, static_cast<T>( 1 ) };

    in[0] /= in[3];
    in[1] /= in[3];
    in[2] /= in[3];

    const T half = static_cast<T>( 0.5 );

    in[0] = in[0] * half + half;
    in[1] = in[1] * half + half;
    in[2] = in[2] * half + half;

    in[0] = in[0] * static_cast<T>( viewportSize[0] ) + static_cast<T>( viewportOrigin[0] );
    in[1] = in[1] * static_cast<T>( viewportSize[1] ) + static_cast<T>( viewportOrigin[1] );

    return vector<T, 3>{ in, 3u };
}

/**
 * Map window coordinates to object coordinates (same as gluUnProject).
 * @param v Window coordinates to map.
 * @param modelView The model view matrix.
 * @param proj The projection matrix.
 * @param viewportOrigin Lower left corner of the viewport.
 * @param viewportSize Size of the viewport
 */
template <typename T, typename TI, typename TS>
requires Numerical<T>&& Numerical<TI>&& Numerical<TS> auto unproject( const vector<T, 3>& v,
                                                                      const matrix<T, 4, 4>& modelView,
                                                                      const matrix<T, 4, 4>& proj,
                                                                      const vector<TI, 2>& viewportOrigin,
                                                                      const vector<TS, 2>& viewportSize )
    -> vector<T, 3> {
    return unproject( v, inverse( proj * modelView ), viewportOrigin, viewportSize );
}

/**
 * Map window coordinates to object coordinates (same as gluUnProject).
 * @param v Window coordinates to map.
 * @param invModelViewProj Inverse model view projection matrix (proj * modelView)^-1.
 * @param viewportOrigin Lower left corner of the viewport.
 * @param viewportSize Size of the viewport
 */
template <typename T, typename TI, typename TS>
requires Numerical<T>&& Numerical<TI>&& Numerical<TS> auto unproject( const vector<T, 3>& v,
                                                                      const matrix<T, 4, 4>& invModelViewProj,
                                                                      const vector<TI, 2>& viewportOrigin,
                                                                      const vector<TS, 2>& viewportSize )
    -> vector<T, 3> {
    vector<T, 4> in{ v, static_cast<T>( 1 ) };

    in[0] = ( in[0] - static_cast<T>( viewportOrigin[0] ) ) / static_cast<T>( viewportSize[0] );
    in[1] = ( in[1] - static_cast<T>( viewportOrigin[1] ) ) / static_cast<T>( viewportSize[1] );

    const T one = static_cast<T>( 1 );
    const T two = static_cast<T>( 2 );

    in[0] = in[0] * two - one;
    in[1] = in[1] * two - one;
    in[2] = in[2] * two - one;

    vector<T, 4> out = invModelViewProj * in;

    out[0] /= out[3];
    out[1] /= out[3];
    out[2] /= out[3];

    return vector<T, 3>{ out, 3u };
}

/**
 * Multiply 4x4 matrix by 3-vector by adding 1 as last component to the vector.
 */
template <typename T, int N>
    requires Numerical<T> &&
    ( N > 1 ) auto transform( const matrix<T, N, N>& m, const vector<T, N - 1>& v ) -> vector<T, N - 1> {
    vector<T, N - 1> temp{ m[N - 1] };
    for ( int c = 0; c < N - 1; ++c ) {
        for ( int r = 0; r < N - 1; ++r ) {
            temp[r] += m[c][r] * v[c];
        }
    }
    return temp;
}

/**
 * Static cast each component from T2 to T1.
 */
template <typename T1, typename T2, int C, int R>
requires Numerical<T1>&& Numerical<T2>&& Positive<C>&& Positive<R> auto staticMatrixCast( const matrix<T2, C, R>& m )
    -> matrix<T1, C, R> {
    matrix<T1, C, R> temp;
    for ( int i = 0; i < C; ++i ) {
        temp[i] = staticVectorCast<T1>( m[i] );
    }
    return temp;
}

/**
 * Returns the trace of a matrix (sum of the diagonal elements).
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto trace( const matrix<T, N, N>& m ) -> T {
    T temp = m[0][0];
    for ( int i = 1; i < N; ++i ) {
        temp += m[i][i];
    }
    return temp;
}

/**
 * Returns the transpose of the inverse of the upper leftmost 3x3 of the matrix
 */
template <typename T>
requires Numerical<T> auto normalMatrix( const matrix<T, 4, 4>& m ) -> matrix<T, 3, 3> {
    return transpose( inverse( matrix<T, 3, 3>{ m } ) );
}

// ============================================================================
// ======================= matrix_base ========================================
// ============================================================================

template <typename T, size_t row, size_t column>
    requires Numerical<T>&& Positive<row>&& Positive<column> &&
    ( row <= static_cast<size_t>( std::numeric_limits<int>::max() ) ) &&
    ( column <= static_cast<size_t>( std::numeric_limits<int>::max() ) ) struct matrix_base {
    using self = matrix_base<T, row, column>;
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    [[nodiscard]] auto operator[]( size_t index ) -> reference { return data()[index]; }

    [[nodiscard]] auto operator[]( size_t index ) const -> const_reference { return data()[index]; }

    [[nodiscard]] auto size() -> size_t { return _data.size(); }

    [[nodiscard]] auto data() -> pointer { return _data.data(); }

    [[nodiscard]] auto data() const -> const_pointer { return _data.data(); }

    [[nodiscard]] consteval auto rows() -> size_t { return row; }

    [[nodiscard]] consteval auto columnes() -> size_t { return column; }

protected:
    std::array<value_type, row * column> _data{};
};

// ============================================================================
// ====================== matrix_sqr_base =====================================
// ============================================================================

template <typename T, size_t rng>
    requires Numerical<T>&& Positive<rng> &&
    ( rng <= static_cast<size_t>( std::numeric_limits<int>::max() ) ) struct matrix_sqr_base
    : matrix_base<T, rng, rng> {
    using base_type = matrix_base<T, rng, rng>;
    using self = matrix_sqr_base<T, rng>;
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    using base_type::_data;

    [[nodiscard]] consteval auto range() -> size_t { return rng; }

    auto transpose() -> void {
        value_type tmp;
        auto rt = *this;

        for ( size_t i = 0; i < rng; ++i ) {
            for ( size_t j = 0; j < i; ++j ) {
                tmp = rt[j * rng + i];
                rt[j * rng + i] = rt[i * rng + j];
                rt[i * rng + j] = tmp;
            }
        }

        *this = rt;
    }

    auto lu() -> std::pair<self, self> {
        self lm, um;
        size_t i, j, k;
        T sum;

        for ( i = 0; i < rng; i++ ) {
            for ( k = i; k < rng; k++ ) {
                sum = 0;
                for ( j = 0; j < i; j++ ) {
                    sum += ( lm[i * rng + j] * um[j * rng + k] );
                }
                um[i * rng + k] = ( *this )[i * rng + k] - sum;
            }

            for ( k = i; k < rng; k++ ) {
                if ( i == k ) {
                    lm[i * rng + i] = T{ 1 };
                } else {
                    sum = 0;
                    for ( j = 0; j < i; j++ ) {
                        sum += lm[k * rng + j] * um[j * rng + i];
                    }
                    lm[k * rng + i] = ( ( *this )[k * rng + i] - sum ) / um[i * rng + i];
                }
            }
        }

        return { lm, um };
    }

    auto determinant_lu() -> value_type {
        T det{ 1 };

        auto [_, u] = lu();
        for ( size_t i = 0; i < rng; i++ ) {
            det *= u[i * rng + i];
        }

        return det;
    }
};

namespace detail {

template <typename T, size_t pSize_>
auto matrix_sqr_set_zero( matrix_sqr_base<T, pSize_>& a ) -> void {
    for ( size_t i = 0; i < pSize_; i++ ) {
        for ( size_t j = 0; j < pSize_; j++ ) {
            a[i * pSize_ + j] = T{};
        }
    }
}

template <typename T, size_t pSize_>
auto matrix_sqr_set_idtt( matrix_sqr_base<T, pSize_>& a ) -> void {
    for ( size_t i = 0; i < pSize_; i++ ) {
        for ( size_t j = 0; j < pSize_; j++ ) {
            if ( i == j ) {
                a[i * pSize_ + j] = static_cast<T>( 1.0 );
            } else {
                a[i * pSize_ + j] = T{};
            }
        }
    }
}

template <typename T, size_t pSize_>
auto matrix_sqr_mult( const matrix_sqr_base<T, pSize_>& a, const matrix_sqr_base<T, pSize_>& b )
    -> matrix_sqr_base<T, pSize_> {
    matrix_sqr_base<T, pSize_> rt{};

    for ( size_t i = 0; i < pSize_; i++ ) {
        for ( size_t j = 0; j < pSize_; j++ ) {
            for ( size_t k = 0; k < pSize_; k++ ) {
                rt[i * pSize_ + j] += a[k * pSize_ + j] * b[i * pSize_ + k];
            }
        }
    }

    return rt;
}

template <typename T, size_t pSize_>
auto matrix_sqr_sum( const matrix_sqr_base<T, pSize_>& a, const matrix_sqr_base<T, pSize_>& b )
    -> matrix_sqr_base<T, pSize_> {
    matrix_sqr_base<T, pSize_> rt{};

    for ( size_t i = 0; i < pSize_; i++ ) {
        for ( size_t j = 0; j < pSize_; j++ ) {
            rt[i * pSize_ + j] = a[i * pSize_ + j] + b[i * pSize_ + j];
        }
    }

    return rt;
}

template <typename T, size_t pSize_>
auto matrix_sqr_sub( const matrix_sqr_base<T, pSize_>& a, const matrix_sqr_base<T, pSize_>& b )
    -> matrix_sqr_base<T, pSize_> {
    matrix_sqr_base<T, pSize_> rt{};

    for ( size_t i = 0; i < pSize_; i++ ) {
        for ( size_t j = 0; j < pSize_; j++ ) {
            rt[i * pSize_ + j] = a[i * pSize_ + j] - b[i * pSize_ + j];
        }
    }

    return rt;
}

template <typename T, size_t pSize_>
auto matrix_sqr_transpose( const matrix_sqr_base<T, pSize_>& m ) -> matrix_sqr_base<T, pSize_> {
    T tmp;
    auto rt = m;

    for ( size_t i = 0; i < pSize_; ++i ) {
        for ( size_t j = 0; j < i; ++j ) {
            tmp = rt[i * pSize_ + j];
            rt[i * pSize_ + j] = rt[j * pSize_ + i];
            rt[j * pSize_ + i] = tmp;
        }
    }

    return rt;
}

template <typename T, size_t pSize_>
auto matrix_sqr_multVec( const matrix_sqr_base<T, pSize_>& m, const vector<T, static_cast<int>(pSize_)>& v )
    -> matrix_sqr_base<T, pSize_> {
    std::decay_t<decltype( m )> rt;

    for ( size_t i = 0; i < pSize_; i++ ) {
        for ( size_t j = 0; j < pSize_; j++ ) {
            rt[i] += m[i * pSize_ + j] * v[j];
        }
    }

    return rt;
}

template <typename T, size_t pSize_>
auto matrix_sqr_lu( const matrix_sqr_base<T, pSize_>& m )
    -> std::pair<matrix_sqr_base<T, pSize_>, matrix_sqr_base<T, pSize_>> {
    std::decay_t<decltype( m )> lm, um;
    size_t i, j, k;
    T sum;

    for ( i = 0; i < pSize_; i++ ) {
        for ( k = i; k < pSize_; k++ ) {
            sum = 0;
            for ( j = 0; j < i; j++ ) {
                sum += ( lm[i * pSize_ + j] * um[j * pSize_ + k] );
            }
            um[i * pSize_ + k] = m[i * pSize_ + k] - sum;
        }

        for ( k = i; k < pSize_; k++ ) {
            if ( i == k ) {
                lm[i * pSize_ + i] = T{ 1 };
            } else {
                sum = 0;
                for ( j = 0; j < i; j++ ) {
                    sum += lm[k * pSize_ + j] * um[j * pSize_ + i];
                }
                lm[k * pSize_ + i] = ( m[k * pSize_ + i] - sum ) / um[i * pSize_ + i];
            }
        }
    }

    return { lm, um };
}

template <typename T, size_t pSize_>
auto matrix_sqr_det_lu( const matrix_sqr_base<T, pSize_>& m ) -> T {
    T det{ 1 };

    auto [_, u] = __matrix_sqr_lu( m );
    for ( size_t i = 0; i < pSize_; i++ ) {
        det *= u[i * pSize_ + i];
    }

    return det;
}

template <typename T, size_t pSize_>
auto matrix_sqr_ldlt( const matrix_sqr_base<T, pSize_>& m )
    -> std::pair<matrix_sqr_base<T, pSize_>, matrix_sqr_base<T, pSize_>> {
    std::decay_t<decltype( m )> lm;
    vector<T, static_cast<int>(pSize_)> dv;
    size_t i, j, k;
    T sum;

    for ( i = 0; i < pSize_; i++ ) {
        for ( j = i; j < pSize_; j++ ) {
            sum = m[j * pSize_ + i];
            for ( k = 0; k < i; k++ ) {
                sum = sum - lm[i * pSize_ + k] * dv[k] * lm[j * pSize_ + k];
                if ( i == j ) {
                    if ( sum <= 0 ) {
                        std::cout << "matrix_sqr_ldlt(): matrix is not "
                                     "positive deﬁnite";
                        return { matrix_sqr_base<T, pSize_>{}, vector<T, static_cast<int>(pSize_)>{} };
                    }
                    dv[i] = sum;
                    lm[i * pSize_ + i] = 1.0;
                } else {
                    lm[j * pSize_ + i] = sum / dv[i];
                }
            }
        }
    }

    return { lm, dv };
}

template <typename T, size_t pSize_>
auto matrix_sqr_solve_gauss( const matrix_sqr_base<T, pSize_>& m, const vector<T, static_cast<int>(pSize_)>& v )
    -> vector<T, static_cast<int>(pSize_)> {
    size_t i, j, k;
    T t;
    std::array<T, pSize_*( pSize_ + 1 )> a;
    vector<T, static_cast<int>(pSize_)> rt;

    for ( i = 0; i < pSize_; i++ ) {
        for ( j = 0; j < pSize_; j++ ) {
            a[i * ( pSize_ + 1 ) + j] = m[i * pSize_ + j];
            a[i * ( pSize_ + 1 ) + pSize_] = v[i];
        }
    }

    /* Pivotisation */
    for ( i = 0; i < pSize_; i++ ) {
        for ( k = i + 1; k < pSize_; k++ ) {
            if ( fabs( a[i * ( pSize_ + 1 ) + i] ) < fabs( a[k * ( pSize_ + 1 ) + i] ) ) {
                for ( j = 0; j <= pSize_; j++ ) {
                    t = a[i * ( pSize_ + 1 ) + j];
                    a[i * ( pSize_ + 1 ) + j] = a[k * ( pSize_ + 1 ) + j];
                    a[k * ( pSize_ + 1 ) + j] = t;
                }
            }
        }
    }

    /* прямой ход */
    for ( k = 1; k < pSize_; k++ ) {
        for ( j = k; j < pSize_; j++ ) {
            t = a[j * ( pSize_ + 1 ) + k - 1] / a[( k - 1 ) * ( pSize_ + 1 ) + k - 1];
            for ( i = 0; i < pSize_ + 1; i++ ) {
                a[j * ( pSize_ + 1 ) + i] = a[j * ( pSize_ + 1 ) + i] - t * a[( k - 1 ) * ( pSize_ + 1 ) + i];
            }
        }
    }

    /* обратный ход */
    for ( i = pSize_ - 1; i > 0; i-- ) {
        rt[i] = a[i * ( pSize_ + 1 ) + pSize_] / a[i * ( pSize_ + 1 ) + i];
        for ( j = pSize_ - 1; j > i; j-- ) {
            rt[i] = rt[i] - a[i * ( pSize_ + 1 ) + j] * rt[j] / a[i * ( pSize_ + 1 ) + i];
        }
    }

    return rt;
}

template <typename T, size_t pSize_>
auto matrix_sqr_insert_cmn( matrix_sqr_base<T, pSize_>& m, const matrix_sqr_base<T, pSize_>& v ) -> void {
    size_t i, j = 0;

    auto rt = m;

    for ( i = pSize_; i < pSize_ * pSize_; i += pSize_ ) {
        rt[i] = v[j];
        j++;
    }

    m = rt;
}

template <typename T, size_t pSize_>
auto matrix_sqr_solve_kramer( const matrix_sqr_base<T, pSize_>& m, const vector<T, static_cast<int>(pSize_)>& v )
    -> vector<T, static_cast<int>(pSize_)> {
    T det;
    std::decay_t<decltype( m )> kr_mtrx;
    vector<T, static_cast<int>(pSize_)> rt;

    det = matrix_sqr_det_lu( m );

    if ( fabs( det ) < std::numeric_limits<float>::epsilon() ) {
        std::cout << "matrix_sqr_solve_kramer(): system has no solve\n";
        return vector<T, static_cast<int>(pSize_)>{};
    }

    for ( size_t i = 0; i < pSize_; i++ ) {
        matrix_sqr_insert_cmn( kr_mtrx, v, i );
        rt[i] = matrix_sqr_det_lu( kr_mtrx ) / det;
    }

    return rt;
}

}  // namespace detail

}  // namespace tire::algebra
