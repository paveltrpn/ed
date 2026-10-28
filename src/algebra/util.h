// Portions of this code belongs to GML linear algebra library by Markus Ilmola.
// Distributed under the Boost Software License, Version 1.0.

#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>
#include <limits>

#include "common.h"
#include "concepts.h"

namespace tire::algebra {

template <Numerical T>
auto clamp( T val, T minVal, T maxVal ) -> T {
    return std::min<T>( std::max<T>( val, minVal ), maxVal );
}

/**
 * Wraps a value around to the given interval
 * @param val Value to wrap to the given interval.
 * @param min Start point of the interval (inclusive)
 * @param max End point of the interval (exclusive)
 */
template <std::floating_point T>
auto repeat( T val, T min, T max ) -> T {
    using std::fmod;
    T temp = fmod( val - min, max - min );
    if ( temp < static_cast<T>( 0 ) ) {
        temp += max - min;
    }
    return temp + min;
}

/**
 * Wraps a value around to the given interval
 * @param val Value to wrap to the given interval.
 * @param min Start point of the interval (inclusive)
 * @param max End point of the interval (exclusive)
 */
template <std::integral T>
auto repeat( T val, T min, T max ) -> T {
    T temp = ( val - min ) % ( max - min );
    if ( temp < static_cast<T>( 0 ) ) {
        temp += max - min;
    }
    return temp + min;
}

/**
 * Convert an unsigned integral type TI to floating point type TF by mapping it
 * linearly to range [0.0, 1.0].
 * @tparam TF Must be a floating point type.
 * @tparam TI Must be an unsigned integral type.
 */
template <std::floating_point TF, UnsignedIntegral TI>
auto unpackUnorm( TI val ) -> TF {
    return static_cast<TF>( val ) / static_cast<TF>( std::numeric_limits<TI>::max() );
}

/**
 * Convert a floating point type to an unsigned integral type by first clamping
 * it to range [0.0, 1.0] and then mapping it to the full range of the integer.
 * @tparam TI Must be an unsigned integral type.
 * @tparam TF Must be a floating point type.
 */
template <UnsignedIntegral TI, std::floating_point TF>
auto packUnorm( TF val ) -> TI {
    return static_cast<TI>( clamp<TF>( val, static_cast<TF>( 0.0 ), static_cast<TF>( 1.0 ) ) *
                                static_cast<TF>( std::numeric_limits<TI>::max() ) +
                            static_cast<TF>( 0.5 ) );
}

/**
 * Convert a signed integral type TI to floating point type TF by mapping it
 * linearly to range [-1.0, 1.0].
 * @tparam TF Must be a floating point type.
 * @tparam TI Must be a signed integral type.
 */
template <std::floating_point TF, SignedIntegral TI>
auto unpackSnorm( TI val ) -> TF {
    return clamp( static_cast<TF>( val ) / static_cast<TF>( std::numeric_limits<TI>::max() ), static_cast<TF>( -1.0 ),
                  static_cast<TF>( 1.0 ) );
}

/**
 * Convert a floating point type to a signed integral type by first clamping
 * it to range [-1.0, 1.0] and then mapping it to the full range of the integer.
 * @tparam TI Must be a signed integral type.
 * @tparam TF Must be a floating point type.
 */
template <SignedIntegral TI, std::floating_point TF>
auto packSnorm( TF val ) -> TI {
    return static_cast<TI>( clamp<TF>( val, static_cast<TF>( -1.0 ), static_cast<TF>( 1.0 ) ) *
                                static_cast<TF>( std::numeric_limits<TI>::max() ) +
                            static_cast<TF>( 0.5 ) );
}

}  // namespace tire::algebra
