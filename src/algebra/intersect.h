// Portions of this code belongs to GML linear algebra library by Markus Ilmola.
// Distributed under the Boost Software License, Version 1.0.

#pragma once

#include <cmath>
#include <limits>
#include <tuple>

#include "concepts.h"
#include "matrix.h"
#include "vector.h"

namespace tire::algebra {

/**
 * Compute a pickray for mouse coordinates.
 * @param mpos The mouse coordinates relative to the BOTTOM left corner of the screen.
 * @param view The view matrix
 * @param proj The projection matrix
 * @param viewportOrigin Bottom left corner of the viewport
 * @param viewportSize Size of the viewport
 * @return Ray origin and unit length ray direction vector.
 */
template <Algebraic T, Algebraic TI, Algebraic TS>
auto pickRay( const vector<TI, 2>& mpos, const matrix<T, 4, 4>& view, const matrix<T, 4, 4>& proj,
              const vector<TI, 2>& viewportOrigin, const vector<TS, 2>& viewportSize )
    -> std::tuple<vector<T, 3>, vector<T, 3>> {
    return pickRay( mpos, inverse( proj * view ), viewportOrigin, viewportSize );
}

/**
 * Compute a pickray for mouse coordinates.
 * @param mpos The mouse coordinates relative to the BOTTOM left corner of the screen.
 * @param invProjView Inverse view projection matrix: (proj*view)^-1
 * @param viewportOrigin Bottom left corner of the viewport
 * @param viewportSize Size of the viewport
 * @return Ray origin and unit length ray direction vector.
 */
template <Algebraic T, Algebraic TI, Algebraic TS>
auto pickRay( const vector<TI, 2>& mpos, const matrix<T, 4, 4>& invProjView, const vector<TI, 2>& viewportOrigin,
              const vector<TS, 2>& viewportSize ) -> std::tuple<vector<T, 3>, vector<T, 3>> {
    const T zero = static_cast<T>( 0 );
    const T one = static_cast<T>( 1 );
    const T half = static_cast<T>( 0.5 );

    const vector<T, 2> fmpos{ static_cast<T>( mpos[0] ) + half, static_cast<T>( mpos[1] ) + half };

    const vector<T, 3> frontPos = unproject( vector<T, 3>{ fmpos, zero }, invProjView, viewportOrigin, viewportSize );

    const vector<T, 3> backPos = unproject( vector<T, 3>{ fmpos, one }, invProjView, viewportOrigin, viewportSize );

    return std::make_tuple( frontPos, normalize( backPos - frontPos ) );
}

/**
 * Computes the intersection of a ray and a N-plane.
 * A plane if N == 3, a line if N == 2.
 * @param origin Origin of the ray
 * @param direction Unit length direction of the ray.
 * @param center Any point on the plane
 * @param normal Unit length normal vector of the plane
 * @return Returns a tuple consisting of a bool denoting whether there was an
 * intersection (true) or not (false, when the ray is parallel to the plane)
 * and a scalar denoting the distance to the intersection point (positive or
 * negative depending if the point is in front of or behind the origin).
 */
template <Algebraic T, int N>
requires Positive<N> auto intersectRayPlane( const vector<T, N>& origin, const vector<T, N>& direction,
                                             const vector<T, N>& center, const vector<T, N>& normal )
    -> std::tuple<bool, T> {
    using std::abs;

    const T divisor = dot( direction, normal );

    if ( abs( divisor ) < std::numeric_limits<T>::epsilon() ) {
        return std::make_tuple( false, static_cast<T>( 0 ) );
    }

    const T d = dot( center, normal );

    return std::make_tuple( true, ( d - dot( origin, normal ) ) / divisor );
}

/**
 * Computes the intersection of a ray and N-sphere.
 * A sphere if N == 3, circle if N == 2.
 * @param origin Origin of the ray
 * @param direction Unit length direction of the ray.
 * @param center Position of the center of the sphere.
 * @param radius Radius of the sphere.
 * @return Returns a tuple consisting of a bool denoting whether there was an
 * intersection (true) or not (false) and two scalars denoting the distances to
 * the front and back intersection points (positive or negative depending if
 * the point is in front of or behind the origin).
 */
template <Algebraic T, int N>
requires Positive<N> auto intersectRaySphere( const vector<T, N>& origin, const vector<T, N>& direction,
                                              const vector<T, N>& center, T radius ) -> std::tuple<bool, T, T> {
    using std::sqrt;

    const vector<T, N> L = center - origin;
    const T tca = dot( L, direction );
    const T d2 = dot( L, L ) - tca * tca;

    const T radius2 = radius * radius;
    if ( d2 > radius2 ) {
        return std::make_tuple( false, static_cast<T>( 0 ), static_cast<T>( 0 ) );
    }

    const T thc = sqrt( radius2 - d2 );

    return std::make_tuple( true, tca - thc, tca + thc );
}

}  // namespace tire::algebra
