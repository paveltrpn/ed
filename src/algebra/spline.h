// Portions of this code belongs to GML linear algebra library by Markus Ilmola.
// Distributed under the Boost Software License, Version 1.0.

#pragma once

#include "concepts.h"
#include "matrix.h"
#include "vector.h"

namespace tire::algebra {

namespace detail {

template <Algebraic T, int N>
requires Positive<N> auto bezierImpl( const vector<T, N>* p, int n, T t1, T t2, int stride = 1 ) -> vector<T, N> {
    if ( n == 1 ) {
        return *p;
    }
    if ( n == 2 ) {
        return t1 * p[0] + t2 * p[stride];
    }
    return t1 * bezierImpl( p, n - 1, t1, t2, stride ) + t2 * bezierImpl( p + stride, n - 1, t1, t2, stride );
}

}  // namespace detail

// Evaluates a bezier curve with D control points at t.
// @tparam D The number of control points.
// @param p Control points.
// @param t Interpolation value [0-1].
template <int D, Algebraic T, int N>
requires Positive<D>&& Positive<N> auto bezier( const vector<T, N> ( &p )[D], T t ) -> vector<T, N> {
    return detail::bezierImpl( &p[0], D, static_cast<T>( 1 ) - t, t );
}

// Evaluates a bezier surface with D0 X D1 control points at t.
// @tparam D0 The number of control points along t[0] direction.
// @tparam D1 The number of control points along t[1] direction.
// @param p Control points.
// @param t Interpolation value [0-1].
template <int D0, int D1, Algebraic T, int N>
requires Positive<D0>&& Positive<D1>&& Positive<N> auto bezier2( const vector<T, N> ( &p )[D1][D0],
                                                                 const vector<T, 2>& t ) -> vector<T, N> {
    vector<T, N> temp[D1];
    for ( int i = 0; i < D1; ++i ) {
        temp[i] = bezier( p[i], t[0] );
    }
    return bezier( temp, t[1] );
}

namespace detail {

template <int O, int D, Algebraic T, int N>
    requires( O >= 0 ) && Positive<D>&& Positive<N> auto bezierDerivativeImpl( const vector<T, N> ( &p )[D], T t )
                              -> vector<T, N> {
    if constexpr ( O == 0 ) {
        return bezier( p, t );
    } else if constexpr ( D == 1 ) {
        return vector<T, N>{ static_cast<T>( 0 ) };
    } else {
        vector<T, N> temp[D - 1];
        for ( int i = 0; i < D - 1; ++i ) {
            temp[i] = static_cast<T>( D - 1 ) * ( p[i + 1] - p[i] );
        }

        return bezierDerivativeImpl<O - 1>( temp, t );
    }
}

}  // namespace detail

// Evaluates Oth order derivative of a bezier curve with D control points at t.
// In the case of O=1 the derivative is tangential to the curve at t.
// @tparam O The order of the derivative. 1 for the first order.
// @tparam D The number of control points.
// @param p Control points.
// @param t Interpolation value [0-1].
template <int O, int D, Algebraic T, int N>
requires Positive<O>&& Positive<D>&& Positive<N> auto bezierDerivative( const vector<T, N> ( &p )[D], T t )
    -> vector<T, N> {
    return detail::bezierDerivativeImpl<O>( p, t );
}

// Evaluates the Oth order Jacobian matrix of a bezier surface at point t.
// In the case of O=1 the two column vectors are tangential to the surface at t
// and in the case of N=3 the cross product of the columns is perpendicular to
// the surface.
// @tparam O The order of the Jacobian matrix. 1 = first order.
// @tparam D0 The number of control points along the t[0] direction.
// @tparam D1 The number of control points along the t[1] direction.
// @param p Control points.
// @param t Interpolation value [0-1].
template <int O, int D0, int D1, Algebraic T, int N>
requires Positive<O>&& Positive<D0>&& Positive<D1>&& Positive<N> auto bezier2Jacobian(
    const vector<T, N> ( &p )[D1][D0], const vector<T, 2>& t ) -> matrix<T, 2, N> {
    vector<T, N> temp0[D0];
    for ( int i = 0; i < D0; ++i ) {
        temp0[i] = detail::bezierImpl( &p[0][i], D1, static_cast<T>( 1 ) - t[1], t[1], D0 );
    }

    vector<T, N> temp1[D1];
    for ( int i = 0; i < D1; ++i ) {
        temp1[i] = bezier( p[i], t[0] );
    }

    return matrix<T, 2, N>{ bezierDerivative<O>( temp0, t[0] ), bezierDerivative<O>( temp1, t[1] ) };
}

}  // namespace tire::algebra
