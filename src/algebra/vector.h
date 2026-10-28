// Portions of this code belongs to GML linear algebra library by Markus Ilmola.
// Distributed under the Boost Software License, Version 1.0.

#pragma once

#include <array>
#include <cassert>
#include <cmath>
#include <concepts>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

#include "common.h"
#include "concepts.h"
#include "util.h"

namespace tire::algebra {

// ============================================================================
// ======================== vector ============================================
// ============================================================================

/**
 * A vector with N components.
 * @tparam T Type of a single component
 * @tparam N Number of components. Must not be zero!
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> class vector {
public:
    /**
     * The type of a single component.
     */
    using value_type = T;

    /**
     * Initializes all components to zero
     */
    vector()
        : vector{ T{ 0 } } {}

    /**
     * Initializes all components to the given value
     */
    explicit vector( const T& a ) {
        for ( int i = 0; i < N; ++i ) {
            _data[i] = a;
        }
    }

    /**
     * Initializes components from a C-array.
     * The data MUST have at least N components or behaviour is undefined.
     * If data is null assertion error will occur.
     */
    explicit vector( const T* data ) {
        assert( data != nullptr );
        for ( int i = 0; i < N; ++i ) {
            _data[i] = data[i];
        }
    }

    /**
     * Initializes components from N values directly.
     */
    template <typename... Args>
    requires( sizeof...( Args ) == N && ( std::convertible_to<Args, T> && ... ) ) vector( const Args&... args )
        : _data{ args... } {}

    /**
     * Initializes from a smaller vector by padding with given value.
     */
    template <int M>
        requires Positive<M> && ( M < N ) explicit vector( const vector<T, M>& v, const T& a = T{ 0 } ) {
        for ( int i = 0; i < M; ++i ) {
            _data[i] = v[i];
        }
        for ( int i = M; i < N; ++i ) {
            _data[i] = a;
        }
    }

    // Initializes from a from a bigger vector by dropping trailing components.
    template <int M>
        requires Positive<M> && ( M > N ) explicit vector( const vector<T, M>& v ) {
        for ( int i = 0; i < N; ++i ) {
            _data[i] = v[i];
        }
    }

    // Initializes from a vector by dropping component with index i.
    // If i is larger than N assertion error will occure.
    vector( const vector<T, N + 1>& v, int i ) {
        assert( i <= N );
        for ( int j = 0; j < N; ++j ) {
            if ( j < i ) {
                _data[j] = v[j];
            } else {
                _data[j] = v[j + 1];
            }
        }
    }

    vector( const vector<T, N>& ) = default;

    vector( vector<T, N>&& ) = default;

    auto operator=( const vector<T, N>& ) -> vector<T, N>& = default;

    auto operator=( vector<T, N>&& ) -> vector<T, N>& = default;

    // Returns a reference to the i:th component of the vector
    auto operator[]( int i ) noexcept -> T& {
        assert( i >= 0 && i < N );
        return _data[i];
    }

    // Returns a reference to the i:th component of the vector
    auto operator[]( int i ) const noexcept -> const T& {
        assert( i >= 0 && i < N );
        return _data[i];
    }

    /**
     * Component-wise sum
     */
    auto operator+( const vector<T, N>& v ) const -> vector<T, N> {
        vector<T, N> temp{ *this };
        temp += v;
        return temp;
    }

    /**
     * Component-wise sum
     */
    auto operator+( const T& a ) const -> vector<T, N> {
        vector<T, N> temp{ *this };
        temp += a;
        return temp;
    }

    /**
     * Component-wise subtraction
     */
    auto operator-( const vector<T, N>& v ) const -> vector<T, N> {
        vector<T, N> temp{ *this };
        temp -= v;
        return temp;
    }

    /**
     * Component-wise subtraction
     */
    auto operator-( const T& a ) const -> vector<T, N> {
        vector<T, N> temp{ *this };
        temp -= a;
        return temp;
    }

    /**
     * Component-wise multiplication
     */
    auto operator*( const vector<T, N>& v ) const -> vector<T, N> {
        vector<T, N> temp{ *this };
        temp *= v;
        return temp;
    }

    /**
     * Component-wise multiplication
     */
    auto operator*( const T& a ) const -> vector<T, N> {
        vector<T, N> temp{ *this };
        temp *= a;
        return temp;
    }

    /**
     * Component-wise division
     */
    auto operator/( const vector<T, N>& v ) const -> vector<T, N> {
        vector<T, N> temp{ *this };
        temp /= v;
        return temp;
    }

    /**
     * Component-wise division
     */
    auto operator/( const T& a ) const -> vector<T, N> {
        vector<T, N> temp{ *this };
        temp /= a;
        return temp;
    }

    /**
     * Component-wise sum
     */
    auto operator+=( const vector<T, N>& v ) -> vector<T, N>& {
        for ( int i = 0; i < N; ++i ) {
            _data[i] += v._data[i];
        }
        return *this;
    }

    /**
     * Component-wise sum
     */
    auto operator+=( const T& a ) -> vector<T, N>& {
        for ( int i = 0; i < N; ++i ) {
            _data[i] += a;
        }
        return *this;
    }

    /**
     * Component-wise subtraction
     */
    auto operator-=( const vector<T, N>& v ) -> vector<T, N>& {
        for ( int i = 0; i < N; ++i ) {
            _data[i] -= v._data[i];
        }
        return *this;
    }

    /**
     * Component-wise subtraction
     */
    auto operator-=( const T& a ) -> vector<T, N>& {
        for ( int i = 0; i < N; ++i ) {
            _data[i] -= a;
        }
        return *this;
    }

    /**
     * Component-wise multiplication
     */
    auto operator*=( const vector<T, N>& v ) -> vector<T, N>& {
        for ( int i = 0; i < N; ++i ) {
            _data[i] *= v._data[i];
        }
        return *this;
    }

    /**
     * Component-wise multiplication
     */
    auto operator*=( const T& a ) -> vector<T, N>& {
        for ( int i = 0; i < N; ++i ) {
            _data[i] *= a;
        }
        return *this;
    }

    /**
     * Component-wise division
     */
    auto operator/=( const vector<T, N>& v ) -> vector<T, N>& {
        for ( int i = 0; i < N; ++i ) {
            _data[i] /= v._data[i];
        }
        return *this;
    }

    /**
     * Component-wise division
     */
    auto operator/=( const T& a ) -> vector<T, N>& {
        for ( int i = 0; i < N; ++i ) {
            _data[i] /= a;
        }
        return *this;
    }

    // Vectors are equal if all of the corresponding components are equal
    auto operator==( const vector<T, N>& v ) const -> bool {
        for ( int i = 0; i < N; ++i ) {
            if ( _data[i] != v._data[i] ) {
                return false;
            }
        }
        return true;
    }

    // Vectors are not equal if any of the corresponding components are not equal
    auto operator!=( const vector<T, N>& v ) const -> bool {
        for ( int i = 0; i < N; ++i ) {
            if ( _data[i] != v._data[i] ) {
                return true;
            }
        }
        return false;
    }

    /**
     * Lexicographical less-than comparison.
     * Used to make vector function as map key
     */
    auto operator<( const vector<T, N>& v ) const -> bool {
        for ( int i = 0; i < N; ++i ) {
            if ( _data[i] < v._data[i] ) {
                return true;
            }
            if ( v._data[i] < _data[i] ) {
                return false;
            }
        }
        return false;
    }

    /**
     * Returns a pointer to the first component
     */
    auto data() const noexcept -> const T* { return _data.data(); }

    /**
     * Returns the number of components in the vector.
     * The maximum value that can be given to []-operator.
     */
    static auto size() noexcept -> int { return N; }

    /**
     * Iterator to the first component.
     */
    auto begin() noexcept -> T* { return _data.data(); }

    auto begin() const noexcept -> const T* { return _data.data(); }

    /**
     * Iterator to one past last component.
     */
    auto end() noexcept -> T* { return _data.data() + N; }

    auto end() const noexcept -> const T* { return _data.data() + N; }

private:
    std::array<T, N> _data{};
};

/**
 * Negates all components
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto operator-( const vector<T, N>& v ) -> vector<T, N> {
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = -v[i];
    }
    return temp;
}

/**
 * Component-wise sum
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto operator+( const T& a, const vector<T, N>& v ) -> vector<T, N> {
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = a + v[i];
    }
    return temp;
}

/**
 * Component-wise subtraction
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto operator-( const T& a, const vector<T, N>& v ) -> vector<T, N> {
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = a - v[i];
    }
    return temp;
}

/**
 * Component-wise multiplication
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto operator*( const T& a, const vector<T, N>& v ) -> vector<T, N> {
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = a * v[i];
    }
    return temp;
}

/**
 * Component-wise division
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto operator/( const T& a, const vector<T, N>& v ) -> vector<T, N> {
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = a / v[i];
    }
    return temp;
}

/**
 * Prints the vector to a stream inside brackets components separated by a comma.
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto operator<<( std::ostream& os, const vector<T, N>& v ) -> std::ostream& {
    os << '(';
    for ( int i = 0; i < N; ++i ) {
        if ( i > 0 ) {
            os << ',';
        }
        os << v[i];
    }
    os << ')';
    return os;
}

/**
 * Read a vector from a stream.
 * The vector must be inside brackets components separated by a comma.
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto operator>>( std::istream& is, vector<T, N>& v ) -> std::istream& {
    char tmp;
    is >> tmp;
    for ( int i = 0; i < N; ++i ) {
        is >> v[i];
        is >> tmp;
    }
    return is;
}

/**
 * Converts a vector to std::string.
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto toString( const vector<T, N>& v ) -> std::string {
    std::stringstream ss{};
    ss << v;
    return ss.str();
}

/**
 * Dot products of vectors v1 and v2
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto dot( const vector<T, N>& v1, const vector<T, N>& v2 ) -> T {
    T temp = v1[0] * v2[0];
    for ( int i = 1; i < N; ++i ) {
        temp += v1[i] * v2[i];
    }
    return temp;
}

/**
 * Dot product where v1 is replaced by vector rotated 90 degrees counter clockwise
 */
template <typename T>
requires Numerical<T> auto perpDot( const vector<T, 2>& v1, const vector<T, 2>& v2 ) -> T {
    return v1[0] * v2[1] - v1[1] * v2[0];
}

/**
 * Cross product of two 3 component vectors
 */
template <typename T>
requires Numerical<T> auto cross( const vector<T, 3>& v1, const vector<T, 3>& v2 ) -> vector<T, 3> {
    return vector<T, 3>{ v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0] };
}

/**
 * Returns a perpendicular vector (counter clockwise rotation by 90 degrees)
 * See: http://reference.wolfram.com/language/ref/Cross.html
 */
template <typename T>
requires Numerical<T> auto cross( const vector<T, 2>& v ) -> vector<T, 2> {
    return vector<T, 2>{ -v[1], v[0] };
}

/**
 * Returns the geometric length of the vector.
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto length( const vector<T, N>& v ) -> T {
    using std::sqrt;
    return sqrt( dot( v, v ) );
}

/**
 * Returns a unit length copy of the vector
 * The vector must not have a zero length!
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto normalize( const vector<T, N>& v ) -> vector<T, N> {
    return v / length( v );
}

/**
 * Returns the length of the vector from p1 to p2
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto distance( const vector<T, N>& p1, const vector<T, N>& p2 ) -> T {
    return length( p2 - p1 );
}

/**
 * Computes the area between the 3 points
 */
template <typename T>
requires Numerical<T> auto area( const vector<T, 3>& p1, const vector<T, 3>& p2, const vector<T, 3>& p3 ) -> T {
    return length( cross( p2 - p1, p3 - p1 ) ) / T{ 2 };
}

/**
 * Calculate the reflection direction for an incident vector.
 * n must be normalized
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto reflect( const vector<T, N>& v, const vector<T, N>& n ) -> vector<T, N> {
    return v - T{ 2 } * dot( v, n ) * n;
}

/**
 * Projects one vector on to another.
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto project( const vector<T, N>& v, const vector<T, N>& u ) -> vector<T, N> {
    return dot( v, u ) / dot( u, u ) * u;
}

/**
 * Calculate the refraction direction for an incident vector
 * @param v The incident vector
 * @param n The normal vector
 * @param eta Index of refraction
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto refract( const vector<T, N>& v, const vector<T, N>& n, const T& eta )
    -> vector<T, N> {
    using std::sqrt;
    T d = dot( n, v );
    T k = T{ 1 } - eta * eta * ( T{ 1 } - d * d );
    if ( k < T{ 0 } ) {
        return vector<T, N>{ T{ 0 } };
    }
    return eta * v - ( eta * d + sqrt( k ) ) * n;
}

/**
 * Linear interpolation between v1 and v2 using a as factor
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto mix( const vector<T, N>& v1, const vector<T, N>& v2, const T& a )
    -> vector<T, N> {
    return ( T{ 1 } - a ) * v1 + a * v2;
}

/**
 * Linear interpolation between v1 and v2 using a as factor
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto mix( const vector<T, N>& v1, const vector<T, N>& v2, const vector<T, N>& a )
    -> vector<T, N> {
    return ( vector<T, N>{ 1 } - a ) * v1 + a * v2;
}

/**
 * Linear spherical interpolation between v1 and v2 using a as factor
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto slerp( const vector<T, N>& v1, const vector<T, N>& v2, const T& a )
    -> vector<T, N> {
    using std::sin;
    const T theta = angle( v1, v2 );
    const T sine = sin( theta );
    return sin( ( T{ 1 } - a ) * theta ) / sine * v1 + sin( a * theta ) / sine * v2;
}

/**
 * Returns the angle (in radians) between vectors v1 and v2
 * If v1 or v2 is zero length zero is returned.
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto angle( const vector<T, N>& v1, const vector<T, N>& v2 ) -> T {
    using std::acos;
    using std::numeric_limits;
    using std::sqrt;

    const T len = sqrt( dot( v1, v1 ) * dot( v2, v2 ) );
    if ( len <= numeric_limits<T>::epsilon() ) {
        return T{ 0 };
    }
    return acos( clamp( dot( v1, v2 ) / len, T{ -1 }, T{ 1 } ) );
}

/**
 * Component-wise min
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto min( const vector<T, N>& v1, const vector<T, N>& v2 ) -> vector<T, N> {
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        if ( v1[i] < v2[i] ) {
            temp[i] = v1[i];
        } else {
            temp[i] = v2[i];
        }
    }
    return temp;
}

/**
 * Component-wise min
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto min( const vector<T, N>& v, const T& a ) -> vector<T, N> {
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        if ( v[i] < a ) {
            temp[i] = v[i];
        } else {
            temp[i] = a;
        }
    }
    return temp;
}

/**
 * Component-wise max
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto max( const vector<T, N>& v1, const vector<T, N>& v2 ) -> vector<T, N> {
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        if ( v1[i] > v2[i] ) {
            temp[i] = v1[i];
        } else {
            temp[i] = v2[i];
        }
    }
    return temp;
}

/**
 * Component-wise max
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto max( const vector<T, N>& v, const T& a ) -> vector<T, N> {
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        if ( v[i] > a ) {
            temp[i] = v[i];
        } else {
            temp[i] = a;
        }
    }
    return temp;
}

/**
 * Constrain components to lie between given values
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto clamp( const vector<T, N>& v, const vector<T, N>& minVal,
                                                const vector<T, N>& maxVal ) -> vector<T, N> {
    return min( max( v, minVal ), maxVal );
}

/**
 * Constrain components to lie between given values
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto clamp( const vector<T, N>& v, const T& minVal, const T& maxVal )
    -> vector<T, N> {
    return min( max( v, minVal ), maxVal );
}

/**
 * Computes the unit length normal of a triangle with vertices p1, p2 and p3
 * The points must not lie on the same line!
 */
template <typename T>
requires Numerical<T> auto normal( const vector<T, 3>& p1, const vector<T, 3>& p2, const vector<T, 3>& p3 )
    -> vector<T, 3> {
    return normalize( cross( p2 - p1, p3 - p1 ) );
}

/**
 * Component-wise sin
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto sin( const vector<T, N>& v ) -> vector<T, N> {
    using std::sin;
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = sin( v[i] );
    }
    return temp;
}

/**
 * Component-wise cos
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto cos( const vector<T, N>& v ) -> vector<T, N> {
    using std::cos;
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = cos( v[i] );
    }
    return temp;
}

/**
 * Component-wise tan
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto tan( const vector<T, N>& v ) -> vector<T, N> {
    using std::tan;
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = tan( v[i] );
    }
    return temp;
}

/**
 * Component-wise asin
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto asin( const vector<T, N>& v ) -> vector<T, N> {
    using std::asin;
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = asin( v[i] );
    }
    return temp;
}

/**
 * Component-wise acos
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto acos( const vector<T, N>& v ) -> vector<T, N> {
    using std::acos;
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = acos( v[i] );
    }
    return temp;
}

/**
 * Component-wise atan
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto atan( const vector<T, N>& v ) -> vector<T, N> {
    using std::atan;
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = atan( v[i] );
    }
    return temp;
}

/**
 * Component-wise sqrt
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto sqrt( const vector<T, N>& v ) -> vector<T, N> {
    using std::sqrt;
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = sqrt( v[i] );
    }
    return temp;
}

/**
 * Component-wise abs
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto abs( const vector<T, N>& v ) -> vector<T, N> {
    using std::abs;
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = abs( v[i] );
    }
    return temp;
}

/**
 * Component-wise pow
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto pow( const vector<T, N>& v1, const vector<T, N>& v2 ) -> vector<T, N> {
    using std::pow;
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = pow( v1[i], v2[i] );
    }
    return temp;
}

/**
 * Component-wise exp
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto exp( const vector<T, N>& v ) -> vector<T, N> {
    using std::exp;
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = exp( v[i] );
    }
    return temp;
}

/**
 * Component-wise log
 */
template <typename T, int N>
requires Numerical<T>&& Positive<N> auto log( const vector<T, N>& v ) -> vector<T, N> {
    using std::log;
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = log( v[i] );
    }
    return temp;
}

/**
 * Component-wise degToRad
 */
template <std::floating_point T, int N>
requires Positive<N> auto degToRad( const vector<T, N>& v ) -> vector<T, N> {
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = degToRad( v[i] );
    }
    return temp;
}

/**
 * Component-wise radToDeg
 */
template <std::floating_point T, int N>
requires Positive<N> auto radToDeg( const vector<T, N>& v ) -> vector<T, N> {
    vector<T, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = radToDeg( v[i] );
    }
    return temp;
}

/**
 * Static cast each component from T2 to T1.
 */
template <typename T1, typename T2, int N>
requires Numerical<T1>&& Numerical<T2>&& Positive<N> auto staticVectorCast( const vector<T2, N>& v ) -> vector<T1, N> {
    vector<T1, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = static_cast<T1>( v[i] );
    }
    return temp;
}

/**
 * Component-wise unpackUnorm
 */
template <typename TF, typename TI, int N>
requires std::floating_point<TF>&& UnsignedIntegral<TI>&& Positive<N> auto unpackUnorm( const vector<TI, N>& v )
    -> vector<TF, N> {
    vector<TF, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = unpackUnorm<TF, TI>( v[i] );
    }
    return temp;
}

/**
 * Component-wise packUnorm
 */
template <typename TI, typename TF, int N>
requires UnsignedIntegral<TI>&& std::floating_point<TF>&& Positive<N> auto packUnorm( const vector<TF, N>& v )
    -> vector<TI, N> {
    vector<TI, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = packUnorm<TI, TF>( v[i] );
    }
    return temp;
}

/**
 * Component-wise unpackSnorm
 */
template <typename TF, typename TI, int N>
requires std::floating_point<TF>&& SignedIntegral<TI>&& Positive<N> auto unpackSnorm( const vector<TI, N>& v )
    -> vector<TF, N> {
    vector<TF, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = unpackSnorm<TF, TI>( v[i] );
    }
    return temp;
}

/**
 * Component-wise packSnorm
 */
template <typename TI, typename TF, int N>
requires SignedIntegral<TI>&& std::floating_point<TF>&& Positive<N> auto packSnorm( const vector<TF, N>& v )
    -> vector<TI, N> {
    vector<TI, N> temp;
    for ( int i = 0; i < N; ++i ) {
        temp[i] = packSnorm<TI, TF>( v[i] );
    }
    return temp;
}

/**
 * Pack a linear srgb color value to (non-linear) fixed-point.
 */
template <typename TI, typename TF>
requires UnsignedIntegral<TI>&& std::floating_point<TF> auto packSrgb( const vector<TF, 3>& v ) -> vector<TI, 3> {
    const auto delinearize = []( TF cl ) -> TF {
        using std::pow;
        if ( cl <= static_cast<TF>( 0 ) ) {
            return static_cast<TF>( 0 );
        }
        if ( cl >= static_cast<TF>( 1 ) ) {
            return static_cast<TF>( 1 );
        }
        if ( cl < static_cast<TF>( 0.0031308 ) ) {
            return static_cast<TF>( 12.92 ) * cl;
        }
        return static_cast<TF>( 1.055 ) * pow( cl, static_cast<TF>( 0.41666 ) ) - static_cast<TF>( 0.055 );
    };
    return packUnorm<TI, TF>( vector<TF, 3>( delinearize( v[0] ), delinearize( v[1] ), delinearize( v[2] ) ) );
}

/**
 * Pack a linear srgba color value to (non-linear) fixed-point.
 * Alpha is unpacked as is and is not delinearized.
 */
template <typename TI, typename TF>
requires UnsignedIntegral<TI>&& std::floating_point<TF> auto packSrgba( const vector<TF, 4>& v ) -> vector<TI, 4> {
    const vector<TI, 3> temp = packSrgb<TI, TF>( vector<TF, 3>( v[0], v[1], v[2] ) );
    return vector<TI, 4>( temp[0], temp[1], temp[2], packUnorm<TI, TF>( v[3] ) );
}

/**
 * Unpack a linear srgb color value from (non-linear) fixed-point.
 */
template <typename TF, typename TI>
requires std::floating_point<TF>&& UnsignedIntegral<TI> auto unpackSrgb( const vector<TI, 3>& v ) -> vector<TF, 3> {
    const auto linearize = []( TF cs ) -> TF {
        using std::pow;
        if ( cs <= static_cast<TF>( 0.04045 ) ) {
            return cs / static_cast<TF>( 12.92 );
        }
        return pow( ( cs + static_cast<TF>( 0.055 ) ) / static_cast<TF>( 1.055 ), static_cast<TF>( 2.4 ) );
    };
    const vector<TF, 3> temp = unpackUnorm<TF, TI>( v );
    return vector<TF, 3>( linearize( temp[0] ), linearize( temp[1] ), linearize( temp[2] ) );
}

/**
 * Unpack a linear srgb color value from (non-linear) fixed-point.
 * Alpha is unpacked as is and is not linearized.
 */
template <typename TF, typename TI>
requires std::floating_point<TF>&& UnsignedIntegral<TI> auto unpackSrgba( const vector<TI, 4>& v ) -> vector<TF, 4> {
    const vector<TF, 3> temp = unpackSrgb<TF, TI>( vector<TI, 3>( v[0], v[1], v[2] ) );
    return vector<TF, 4>( temp[0], temp[1], temp[2], unpackUnorm<TF, TI>( v[3] ) );
}

}  // namespace tire::algebra
