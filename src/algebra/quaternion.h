// Portions of this code belongs to GML linear algebra library by Markus Ilmola.
// Distributed under the Boost Software License, Version 1.0.

#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <istream>
#include <numbers>
#include <ostream>
#include <sstream>
#include <string>
#include <tuple>

#include "common.h"
#include "concepts.h"
#include "vector.h"
#include "vector3.h"

namespace tire::algebra {

// ============================================================================
// ======================= quaternion =========================================
// ============================================================================

/**
 * Complex number with scalar as real part and 3-vector as imaginary part.
 * Used for rotations and for storing orientation in 3d space.
 */
template <Numerical T>
class quaternion {
public:
    /**
     * Type of the real part and a single component of the imaginary part.
     */
    using value_type = T;

    /**
     * Quaternion with zero real an imaginary parts.
     */
    quaternion()
        : _real{ T{ 0 } }
        , _imag{ T{ 0 } } {}

    /**
     * A quaternion with given real part and zero imaginary part
     */
    explicit quaternion( const T& real )
        : _real{ real }
        , _imag{ T{ 0 } } {}

    /**
     * A quaternion with zero real part and given imaginary part
     */
    explicit quaternion( const vector<T, 3>& imag )
        : _real{ T{ 0 } }
        , _imag{ imag } {}

    /**
     * A quaternion with given real and imaginary parts.
     */
    quaternion( T real, const vector<T, 3>& imag )
        : _real{ real }
        , _imag{ imag } {}

    quaternion( const quaternion& ) = default;

    quaternion( quaternion&& ) = default;

    auto operator=( const quaternion& ) -> quaternion& = default;

    auto operator=( quaternion&& ) -> quaternion& = default;

    /**
     * The real part of the quaternion
     */
    [[nodiscard]] auto real() -> T& { return _real; }

    /**
     * The real part of the quaternion
     */
    [[nodiscard]] auto real() const -> const T& { return _real; }

    /**
     * The imaginary part of the quaternion
     */
    [[nodiscard]] auto imag() -> vector<T, 3>& { return _imag; }

    /**
     * The imaginary part of the quaternion
     */
    [[nodiscard]] auto imag() const -> const vector<T, 3>& { return _imag; }

    /**
     * Component-wise addition
     */
    [[nodiscard]] auto operator+( const quaternion<T>& q ) const -> quaternion<T> {
        return quaternion<T>{ _real + q._real, _imag + q._imag };
    }

    /**
     * Component-wise addition by interpreting v as imaginary part of a quaternion.
     */
    [[nodiscard]] auto operator+( const vector<T, 3>& v ) const -> quaternion<T> {
        return quaternion<T>{ _real, _imag + v };
    }

    /**
     * Component-wise addition.
     */
    [[nodiscard]] auto operator+( const T& a ) const -> quaternion<T> { return quaternion<T>{ _real + a, _imag }; }

    /**
     * Components wise subtraction
     */
    [[nodiscard]] auto operator-( const quaternion<T>& q ) const -> quaternion<T> {
        return quaternion<T>{ _real - q._real, _imag - q._imag };
    }

    /**
     * Components wise subtraction
     */
    [[nodiscard]] auto operator-( const vector<T, 3>& v ) const -> quaternion<T> {
        return quaternion<T>{ _real, _imag - v };
    }

    /**
     * Components wise subtraction
     */
    [[nodiscard]] auto operator-( const T& a ) const -> quaternion<T> { return quaternion<T>{ _real - a, _imag }; }

    /**
     * Quaternion multiplication.
     * Not done component wise.
     */
    [[nodiscard]] auto operator*( const quaternion<T>& q ) const -> quaternion<T> {
        return quaternion<T>{ _real * q._real - dot( _imag, q._imag ),
                              _real * q._imag + q._real * _imag + cross( _imag, q._imag ) };
    }

    /**
     * Multiply quaternion and 3-vector (aka quaternion with zero real part).
     * Note that vector v is not rotated by quaternion q by q*v but q*v*conj(q)
     * (or use transform).
     */
    [[nodiscard]] auto operator*( const vector<T, 3>& v ) const -> quaternion<T> {
        return quaternion<T>{ -dot( _imag, v ), _real * v + cross( _imag, v ) };
    }

    /**
     * Multiply with scalar as it were the real part of a quaternion.
     */
    [[nodiscard]] auto operator*( const T& a ) const -> quaternion<T> { return quaternion<T>{ _real * a, _imag * a }; }

    [[nodiscard]] auto operator/( const T& a ) const -> quaternion<T> { return quaternion<T>{ _real / a, _imag / a }; }

    auto operator+=( const quaternion<T>& q ) -> quaternion<T>& {
        _real += q._real;
        _imag += q._imag;
        return *this;
    }

    auto operator-=( const quaternion<T>& q ) -> quaternion<T>& {
        _real -= q._real;
        _imag -= q._imag;
        return *this;
    }

    auto operator*=( const quaternion<T>& q ) -> quaternion<T>& {
        *this = *this * q;
        return *this;
    }

    /**
     * Quaternions are equal if both parts are equal.
     */
    [[nodiscard]] auto operator==( const quaternion<T>& q ) const -> bool {
        if ( _real != q._real ) {
            return false;
        }
        if ( _imag != q._imag ) {
            return false;
        }
        return true;
    }

    [[nodiscard]] auto operator==( const T& a ) const -> bool {
        if ( _real != a ) {
            return false;
        }
        if ( _imag != vector<T, 3>{ T{ 0 } } ) {
            return false;
        }
        return true;
    }

    [[nodiscard]] auto operator==( const vector<T, 3>& v ) const -> bool {
        if ( _real != T{ 0 } ) {
            return false;
        }
        if ( _imag != v ) {
            return false;
        }
        return true;
    }

    /**
     * Quaternions are not equal if either of the parts are not equal.
     */
    [[nodiscard]] auto operator!=( const quaternion<T>& q ) const -> bool {
        if ( _real != q._real ) {
            return true;
        }
        if ( _imag != q._imag ) {
            return true;
        }
        return false;
    }

private:
    T _real;
    vector<T, 3> _imag;
};

/**
 * Multiplies scalar (aka quaternion with zero imaginary part) and quaternion
 */
template <Numerical T>
[[nodiscard]] auto operator*( const T& a, const quaternion<T>& q ) -> quaternion<T> {
    return quaternion<T>{ a * q.real(), a * q.imag() };
}

/**
 * Multiply quaternion and vector as it were the imaginary part of a quaternion.
 */
template <Numerical T>
[[nodiscard]] auto operator*( const vector<T, 3>& v, const quaternion<T>& q ) -> quaternion<T> {
    return quaternion<T>{ -dot( v, q.imag() ), q.real() * v + cross( v, q.imag() ) };
}

/**
 * Add quaternion and vector as it were the imaginary part of a quaternion.
 */
template <Numerical T>
[[nodiscard]] auto operator+( vector<T, 3> v, const quaternion<T>& q ) -> quaternion<T> {
    return quaternion<T>{ q.real(), v + q.imag() };
}

template <Numerical T>
[[nodiscard]] auto operator+( const T& a, const quaternion<T>& q ) -> quaternion<T> {
    return quaternion<T>{ a + q.real(), q.imag() };
}

template <Numerical T>
[[nodiscard]] auto operator-( const quaternion<T>& q ) -> quaternion<T> {
    return quaternion<T>{ -q.real(), -q.imag() };
}

template <Numerical T>
[[nodiscard]] auto operator-( const T& a, const quaternion<T>& q ) -> quaternion<T> {
    return quaternion<T>{ a - q.real(), -q.imag() };
}

template <Numerical T>
[[nodiscard]] auto operator-( const vector<T, 3>& v, const quaternion<T>& q ) -> quaternion<T> {
    return quaternion<T>{ -q.real(), v - q.imag() };
}

/**
 * Write a quaternion to a stream inside brackets parts separated by a comma.
 */
template <Numerical T>
auto operator<<( std::ostream& os, const quaternion<T>& q ) -> std::ostream& {
    os << '(' << q.real() << ',' << q.imag() << ')';
    return os;
}

/**
 * Reads a quaternion from a stream.
 * The parts must be inside brackets separated be a comma.
 */
template <Numerical T>
auto operator>>( std::istream& is, quaternion<T>& q ) -> std::istream& {
    char ch = 0;
    is >> ch >> q.real() >> ch >> q.imag() >> ch;
    return is;
}

/**
 * Converts a quaternion to std::string.
 */
template <Numerical T>
[[nodiscard]] auto toString( const quaternion<T>& q ) -> std::string {
    std::stringstream ss{};
    ss << q;
    return ss.str();
}

/**
 * Returns the squared magnitude of the quaternion q
 */
template <Numerical T>
[[nodiscard]] auto norm( const quaternion<T>& q ) -> T {
    return q.real() * q.real() + dot( q.imag(), q.imag() );
}

/**
 * Returns the magnitude of the quaternion q.
 */
template <Numerical T>
[[nodiscard]] auto abs( const quaternion<T>& q ) -> T {
    using std::sqrt;
    return sqrt( norm( q ) );
}

/**
 * Makes the quaternion a unit quaternion.
 */
template <Numerical T>
[[nodiscard]] auto normalize( const quaternion<T>& q ) -> quaternion<T> {
    return q / abs( q );
}

/**
 * Quaternion conjugate (negates imaginary part)
 */
template <Numerical T>
[[nodiscard]] auto conj( const quaternion<T>& q ) -> quaternion<T> {
    return quaternion<T>{ q.real(), -q.imag() };
}

/**
 * Generates rotation quaternion from axis and angle
 * @param angle Rotation angle in radians
 * @param axis Unit length rotation axis.
 */
template <Numerical T>
[[nodiscard]] auto quaternionRotate( const T& angle, const vector<T, 3>& axis ) -> quaternion<T> {
    using std::cos;
    using std::sin;

    const T a = angle / T{ 2 };
    return quaternion<T>{ cos( a ), sin( a ) * axis };
}

/**
 * Generates rotation quaternion from Euler angles
 * @param angle Euler angles in radians.
 */
template <Numerical T>
[[nodiscard]] auto quaternionRotate( const vector<T, 3>& angle ) -> quaternion<T> {
    using std::cos;
    using std::sin;

    const T a1 = angle[0] / T{ 2 };
    const T a2 = angle[1] / T{ 2 };
    const T a3 = angle[2] / T{ 2 };

    const T sx = sin( a1 );
    const T cx = cos( a1 );
    const T sy = sin( a2 );
    const T cy = cos( a2 );
    const T sz = sin( a3 );
    const T cz = cos( a3 );

    return quaternion<T>{
        cx * cy * cz + sx * sy * sz,
        vector<T, 3>{ cy * cz * sx - cx * sy * sz, cx * cz * sy + cy * sx * sz, -cz * sx * sy + cx * cy * sz } };
}

/**
 * Decomposes a rotation quaternion to angle and axis.
 */
template <Numerical T>
[[nodiscard]] auto decomposeRotate( const quaternion<T>& q ) -> std::tuple<T, vector<T, 3>> {
    using std::acos;
    using std::max;
    using std::sqrt;

    const T zero = static_cast<T>( 0 );
    const T one = static_cast<T>( 1 );
    const T two = static_cast<T>( 2 );

    return std::make_tuple( two * acos( std::min<T>( std::max<T>( q.real(), -one ), one ) ),
                            q.imag() / sqrt( max( zero, one - q.real() * q.real() ) ) );
}

/**
 * Linear interpolation between quaternions.
 * the resulting quaternion is NOT normalized
 */
template <Numerical T>
[[nodiscard]] auto mix( const quaternion<T>& q1, const quaternion<T>& q2, const T& a ) -> quaternion<T> {
    return ( T{ 1 } - a ) * q1 + a * q2;
}

/**
 * Rotates 3-vector v with quaternion q (computes q*v*conj(q))
 * Note q has to be a unit quaternion.
 */
template <Numerical T>
[[nodiscard]] auto transform( const quaternion<T>& q, const vector<T, 3>& v ) -> vector<T, 3> {
    const vector<T, 3> temp = T{ 2 } * cross( q.imag(), v );
    return v + q.real() * temp + cross( q.imag(), temp );
}

/**
 * Inverse of quaternion.
 */
template <Numerical T>
[[nodiscard]] auto inverse( const quaternion<T>& q ) -> quaternion<T> {
    return conj( q ) / norm( q );
}

/**
 * Static cast each component from T2 to T1.
 */
template <Numerical T1, Numerical T2>
[[nodiscard]] auto staticQuaternionCast( const quaternion<T2>& q ) -> quaternion<T1> {
    return quaternion<T1>{ static_cast<T1>( q.real() ), staticVectorCast<T1>( q.imag() ) };
}

}  // namespace tire::algebra
