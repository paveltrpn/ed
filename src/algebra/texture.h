// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cassert>
#include <cmath>
#include <concepts>
#include <utility>

#include "concepts.h"
#include "vector.h"

namespace tire::algebra {

/// Calculates the texture coordinate of the center of a texel with given index
/// in a 1D texture with given size. The return value is in the range [0, 1] if
/// the index is in the range [0, size - 1].
/// @tparam TF Type of the coordinate to return. Must be a floating point type.
/// @tparam TI Type of the index. Must be an integral type.
/// @tparam TS Type of the size. Must be an integral type.
/// @param index Texel index in the texture.
/// @param size Size of the texture in texels.
template <std::floating_point TF, std::integral TI, std::integral TS>
auto texelCenter( TI index, TS size ) -> TF {
    if ( size == static_cast<TS>( 0 ) ) {
        return static_cast<TF>( 0 );
    }
    return ( static_cast<TF>( index ) + static_cast<TF>( 0.5 ) ) / static_cast<TF>( size );
}

/// Calculates the texture coordinate of the center of a texel with given index
/// in a N-D texture with given size. The return value is in the range [0, 1] if
/// the index is in the range [0, size - 1].
/// @tparam TF Type of the coordinate to return. Must be a floating point type.
/// @tparam TI Type of the index. Must be an integral type.
/// @tparam TS Type of the size. Must be an integral type.
/// @param index Texel index in the texture.
/// @param size Size of the texture in texels.
template <std::floating_point TF, std::integral TI, std::integral TS, int N>
    requires Positive<N>
auto texelCenter( const vector<TI, N>& index, const vector<TS, N>& size ) -> vector<TF, N> {
    vector<TF, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = texelCenter<TF>( index[i], size[i] );
    }
    return temp;
}

/// Calculates the index of the texel with center nearest to the given coordinate
/// in a 1D texture with given size.
/// The return value is in the range [0, size - 1] if input is in range [0, 1[.
/// @tparam TI Type of the index. Must be an integral type.
/// @tparam TF Type of the coordinate. Must be a floating point type.
/// @tparam TS Type of the size. Must be an integral type.
/// @param coord The texture coordinate.
/// @param size Size of the texture in texels.
template <std::integral TI, std::floating_point TF, std::integral TS>
auto nearestTexel( TF coord, TS size ) -> TI {
    const TF temp = coord * static_cast<TF>( size );
    return static_cast<TI>( temp ) - static_cast<TI>( temp < static_cast<TF>( 0 ) );
}

/// Calculates the index of the texel with center nearest to the given coordinate
/// in a N-D texture with given size.
/// The return value is in the range [0, size - 1] if input is in range [0, 1[.
/// @tparam TI Type of the index. Must be an integral type.
/// @tparam TF Type of the coordinate. Must be a floating point type.
/// @tparam TS Type of the size. Must be an integral type.
/// @param coord The texture coordinate.
/// @param size Size of the texture in texels.
template <std::integral TI, std::floating_point TF, std::integral TS, int N>
    requires Positive<N>
auto nearestTexel( const vector<TF, N>& coord, const vector<TS, N>& size ) -> vector<TI, N> {
    vector<TI, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = nearestTexel<TI>( coord[i], size[i] );
    }
    return temp;
}

/// Returns the index of a cubemap face at given direction.
/// @param direction The direction vector. Need not be normalized.
/// @return 0: +X, 1: -X, 2: +Y, 3: -Y, 4: +Z, 5: -Z.
template <std::floating_point TF>
auto faceAt( const vector<TF, 3>& direction ) -> int {
    const vector<TF, 3> a = abs( direction );

    const TF zero = static_cast<TF>( 0 );

    if ( a[0] >= a[1] ) {
        if ( a[0] >= a[2] ) {
            if ( direction[0] < zero ) {
                return 1;
            } else {
                return 0;
            }
        } else {
            if ( direction[2] < zero ) {
                return 5;
            } else {
                return 4;
            }
        }
    } else {
        if ( a[1] >= a[2] ) {
            if ( direction[1] < zero ) {
                return 3;
            } else {
                return 2;
            }
        } else {
            if ( direction[2] < zero ) {
                return 5;
            } else {
                return 4;
            }
        }
    }

    return 0;
}

/// Convert a cube map direction vector to face index texture coordinate pair
/// using the OpenGL conventions.
/// @param direction The direction vector. Need not be normalized.
/// @return 0: +X, 1: -X, 2: +Y, 3: -Y, 4: +Z, 5: -Z.
template <std::floating_point TF>
auto cubeTexCoord( const vector<TF, 3>& direction ) -> std::pair<int, vector<TF, 2>> {
    const auto fn = []( TF sc, TF tc, TF ma ) -> vector<TF, 2> {
        using std::abs;
        ma = abs( ma );
        const TF half = static_cast<TF>( 1 ) / static_cast<TF>( 2 );
        return vector<TF, 2>{ half * ( sc / ma + static_cast<TF>( 1 ) ),
                              half * ( tc / ma + static_cast<TF>( 1 ) ) };
    };

    // OpenGL spec
    // direction     target                           sc     tc   ma
    // ----------    ------------------------------   --     --   --
    // +rx          GL_TEXTURE_CUBE_MAP_POSITIVE_X   -rz    -ry   rx
    // -rx          GL_TEXTURE_CUBE_MAP_NEGATIVE_X   +rz    -ry   rx
    // +ry          GL_TEXTURE_CUBE_MAP_POSITIVE_Y   +rx    +rz   ry
    // -ry          GL_TEXTURE_CUBE_MAP_NEGATIVE_Y   +rx    -rz   ry
    // +rz          GL_TEXTURE_CUBE_MAP_POSITIVE_Z   +rx    -ry   rz
    // -rz          GL_TEXTURE_CUBE_MAP_NEGATIVE_Z   -rx    -ry   rz

    const int face = faceAt( direction );
    switch ( face ) {
        case 0:  // +x
            return std::make_pair( face, fn( -direction[2], -direction[1], direction[0] ) );
        case 1:  // -x
            return std::make_pair( face, fn( direction[2], -direction[1], direction[0] ) );
        case 2:  // +y
            return std::make_pair( face, fn( direction[0], direction[2], direction[1] ) );
        case 3:  // -y
            return std::make_pair( face, fn( direction[0], -direction[2], direction[1] ) );
        case 4:  // +z
            return std::make_pair( face, fn( direction[0], -direction[1], direction[2] ) );
        case 5:  // -z
            return std::make_pair( face, fn( -direction[0], -direction[1], direction[2] ) );
    }

    // Should not happen
    return std::make_pair( 0, vector<TF, 2>{ static_cast<TF>( 1.0 ), static_cast<TF>( 0.0 ) } );
}

/// Convert a face index texture coordinate pair to cube map direction vector
/// using the OpenGL conventions.
/// @param faceIndex 0: +X, 1: -X, 2: +Y, 3: -Y, 4: +Z, 5: -Z. If the param is
/// not in the range [0, 5] an assertion failure will occur.
/// @param texCoord Texture coordinate in the given face.
/// @return Direction vector. The vector is not normalized.
template <std::floating_point TF>
auto cubeDirection( int faceIndex, const vector<TF, 2>& texCoord ) -> vector<TF, 3> {
    assert( faceIndex >= 0 && faceIndex <= 5 );

    const vector<TF, 2> temp = static_cast<TF>( 2.0 ) * texCoord - static_cast<TF>( 1.0 );

    const TF one = static_cast<TF>( 1 );

    switch ( faceIndex ) {
        case 0:  // +x
            return vector<TF, 3>{ one, -temp[1], -temp[0] };
        case 1:  // -x
            return vector<TF, 3>{ -one, -temp[1], temp[0] };
        case 2:  // +y
            return vector<TF, 3>{ temp[0], one, temp[1] };
        case 3:  // -y
            return vector<TF, 3>{ temp[0], -one, -temp[1] };
        case 4:  // +z
            return vector<TF, 3>{ temp[0], -temp[1], one };
        case 5:  // -z
            return vector<TF, 3>{ -temp[0], -temp[1], -one };
    }

    // Should not happen
    return vector<TF, 3>{ one, static_cast<TF>( 0.0 ), static_cast<TF>( 0.0 ) };
}

}  // namespace tire::algebra
