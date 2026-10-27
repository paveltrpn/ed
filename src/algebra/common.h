
#pragma once

#include <cmath>
#include <concepts>
#include <numbers>
#include <type_traits>

#include "concepts.h"

namespace tire::algebra {

template <std::floating_point T>
constexpr T _half_circle = T{ 180 };

// Converts degrees to radians
template <std::floating_point T>
constexpr auto degToRad( T deg ) -> T {
    return deg * ( std::numbers::pi_v<T> / _half_circle<T> );
}

// Converts radians to degrees
template <std::floating_point T>
constexpr auto radToDeg( T rad ) -> T {
    return rad * ( _half_circle<T> / std::numbers::pi_v<T> );
}

}  // namespace tire::algebra
