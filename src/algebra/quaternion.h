// Copyright 2015 Markus Ilmola
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

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
template <Algebraic T>
class quaternion {
public:
    /// Type of the real part and a single component of the imaginary part.
    using value_type = T;

    /// Quaternion with zero real an imaginary parts.
    quaternion() : _real{ T{ 0 } }, _imag{ T{ 0 } } {}

    /// A quaternion with given real part and zero imaginary part
    explicit quaternion( const T& real ) : _real{ real }, _imag{ T{ 0 } } {}

    /// A quaternion with zero real part and given imaginary part
    explicit quaternion( const vector<T, 3>& imag ) : _real{ T{ 0 } }, _imag{ imag } {}

    /// A quaternion with given real and imaginary parts.
    quaternion( T real, const vector<T, 3>& imag ) : _real{ real }, _imag{ imag } {}

    quaternion( const quaternion& ) = default;

    quaternion( quaternion&& ) = default;

    auto operator=( const quaternion& ) -> quaternion& = default;

    auto operator=( quaternion&& ) -> quaternion& = default;

    /// The real part of the quaternion
    [[nodiscard]] auto real() -> T& {
        return _real;
    }

    /// The real part of the quaternion
    [[nodiscard]] auto real() const -> const T& {
        return _real;
    }

    /// The imaginary part of the quaternion
    [[nodiscard]] auto imag() -> vector<T, 3>& {
        return _imag;
    }

    /// The imaginary part of the quaternion
    [[nodiscard]] auto imag() const -> const vector<T, 3>& {
        return _imag;
    }

    /// Component-wise addition
    [[nodiscard]] auto operator+( const quaternion<T>& q ) const -> quaternion<T> {
        return quaternion<T>{ _real + q._real, _imag + q._imag };
    }

    /// Component-wise addition by interpreting v as imaginary part of a quaternion.
    [[nodiscard]] auto operator+( const vector<T, 3>& v ) const -> quaternion<T> {
        return quaternion<T>{ _real, _imag + v };
    }

    /// Component-wise addition.
    [[nodiscard]] auto operator+( const T& a ) const -> quaternion<T> {
        return quaternion<T>{ _real + a, _imag };
    }

    /// Components wise subtraction
    [[nodiscard]] auto operator-( const quaternion<T>& q ) const -> quaternion<T> {
        return quaternion<T>{ _real - q._real, _imag - q._imag };
    }

    /// Components wise subtraction
    [[nodiscard]] auto operator-( const vector<T, 3>& v ) const -> quaternion<T> {
        return quaternion<T>{ _real, _imag - v };
    }

    /// Components wise subtraction
    [[nodiscard]] auto operator-( const T& a ) const -> quaternion<T> {
        return quaternion<T>{ _real - a, _imag };
    }

    /// Quaternion multiplication.
    /// Not done component wise.
    [[nodiscard]] auto operator*( const quaternion<T>& q ) const -> quaternion<T> {
        return quaternion<T>{ _real * q._real - dot( _imag, q._imag ),
                              _real * q._imag + q._real * _imag + cross( _imag, q._imag ) };
    }

    /// Multiply quaternion and 3-vector (aka quaternion with zero real part).
    /// Note that vector v is not rotated by quaternion q by q*v but q*v*conj(q)
    /// (or use transform).
    [[nodiscard]] auto operator*( const vector<T, 3>& v ) const -> quaternion<T> {
        return quaternion<T>{ -dot( _imag, v ), _real * v + cross( _imag, v ) };
    }

    /// Multiply with scalar as it were the real part of a quaternion.
    [[nodiscard]] auto operator*( const T& a ) const -> quaternion<T> {
        return quaternion<T>{ _real * a, _imag * a };
    }

    [[nodiscard]] auto operator/( const T& a ) const -> quaternion<T> {
        return quaternion<T>{ _real / a, _imag / a };
    }

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

    /// Quaternions are equal if both parts are equal.
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

    /// Quaternions are not equal if either of the parts are not equal.
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

/// Multiplies scalar (aka quaternion with zero imaginary part) and quaternion
template <Algebraic T>
[[nodiscard]] auto operator*( const T& a, const quaternion<T>& q ) -> quaternion<T> {
    return quaternion<T>{ a * q.real(), a * q.imag() };
}

/// Multiply quaternion and vector as it were the imaginary part of a quaternion.
template <Algebraic T>
[[nodiscard]] auto operator*( const vector<T, 3>& v, const quaternion<T>& q ) -> quaternion<T> {
    return quaternion<T>{ -dot( v, q.imag() ), q.real() * v + cross( v, q.imag() ) };
}

/// Add quaternion and vector as it were the imaginary part of a quaternion.
template <Algebraic T>
[[nodiscard]] auto operator+( vector<T, 3> v, const quaternion<T>& q ) -> quaternion<T> {
    return quaternion<T>{ q.real(), v + q.imag() };
}

template <Algebraic T>
[[nodiscard]] auto operator+( const T& a, const quaternion<T>& q ) -> quaternion<T> {
    return quaternion<T>{ a + q.real(), q.imag() };
}

template <Algebraic T>
[[nodiscard]] auto operator-( const quaternion<T>& q ) -> quaternion<T> {
    return quaternion<T>{ -q.real(), -q.imag() };
}

template <Algebraic T>
[[nodiscard]] auto operator-( const T& a, const quaternion<T>& q ) -> quaternion<T> {
    return quaternion<T>{ a - q.real(), -q.imag() };
}

template <Algebraic T>
[[nodiscard]] auto operator-( const vector<T, 3>& v, const quaternion<T>& q ) -> quaternion<T> {
    return quaternion<T>{ -q.real(), v - q.imag() };
}

/// Write a quaternion to a stream inside brackets parts separated by a comma.
template <Algebraic T>
auto operator<<( std::ostream& os, const quaternion<T>& q ) -> std::ostream& {
    os << '(' << q.real() << ',' << q.imag() << ')';
    return os;
}

/// Reads a quaternion from a stream.
/// The parts must be inside brackets separated be a comma.
template <Algebraic T>
auto operator>>( std::istream& is, quaternion<T>& q ) -> std::istream& {
    char ch = 0;
    is >> ch >> q.real() >> ch >> q.imag() >> ch;
    return is;
}

/// Converts a quaternion to std::string.
template <Algebraic T>
[[nodiscard]] auto toString( const quaternion<T>& q ) -> std::string {
    std::stringstream ss{};
    ss << q;
    return ss.str();
}

/// Returns the squared magnitude of the quaternion q
template <Algebraic T>
[[nodiscard]] auto norm( const quaternion<T>& q ) -> T {
    return q.real() * q.real() + dot( q.imag(), q.imag() );
}

/// Returns the magnitude of the quaternion q.
template <Algebraic T>
[[nodiscard]] auto abs( const quaternion<T>& q ) -> T {
    using std::sqrt;
    return sqrt( norm( q ) );
}

/// Makes the quaternion a unit quaternion.
template <Algebraic T>
[[nodiscard]] auto normalize( const quaternion<T>& q ) -> quaternion<T> {
    return q / abs( q );
}

/// Quaternion conjugate (negates imaginary part)
template <Algebraic T>
[[nodiscard]] auto conj( const quaternion<T>& q ) -> quaternion<T> {
    return quaternion<T>{ q.real(), -q.imag() };
}

/// Generates rotation quaternion from axis and angle
/// @param angle Rotation angle in radians
/// @param axis Unit length rotation axis.
template <Algebraic T>
[[nodiscard]] auto quaternionRotate( const T& angle, const vector<T, 3>& axis ) -> quaternion<T> {
    using std::cos;
    using std::sin;

    const T a = angle / T{ 2 };
    return quaternion<T>{ cos( a ), sin( a ) * axis };
}

/// Generates rotation quaternion from Euler angles
/// @param angle Euler angles in radians.
template <Algebraic T>
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
        vector<T, 3>{ cy * cz * sx - cx * sy * sz, cx * cz * sy + cy * sx * sz,
                      -cz * sx * sy + cx * cy * sz }
    };
}

/// Decomposes a rotation quaternion to angle and axis.
template <Algebraic T>
[[nodiscard]] auto decomposeRotate( const quaternion<T>& q ) -> std::tuple<T, vector<T, 3>> {
    using std::acos;
    using std::max;
    using std::sqrt;

    const T zero = static_cast<T>( 0 );
    const T one = static_cast<T>( 1 );
    const T two = static_cast<T>( 2 );

    return std::make_tuple(
        two * acos( std::min<T>( std::max<T>( q.real(), -one ), one ) ),
        q.imag() / sqrt( max( zero, one - q.real() * q.real() ) ) );
}

/// Linear interpolation between quaternions.
/// the resulting quaternion is NOT normalized
template <Algebraic T>
[[nodiscard]] auto mix( const quaternion<T>& q1, const quaternion<T>& q2, const T& a ) -> quaternion<T> {
    return ( T{ 1 } - a ) * q1 + a * q2;
}

/// Rotates 3-vector v with quaternion q (computes q*v*conj(q))
/// Note q has to be a unit quaternion.
template <Algebraic T>
[[nodiscard]] auto transform( const quaternion<T>& q, const vector<T, 3>& v ) -> vector<T, 3> {
    const vector<T, 3> temp = T{ 2 } * cross( q.imag(), v );
    return v + q.real() * temp + cross( q.imag(), temp );
}

/// Inverse of quaternion.
template <Algebraic T>
[[nodiscard]] auto inverse( const quaternion<T>& q ) -> quaternion<T> {
    return conj( q ) / norm( q );
}

/// Static cast each component from T2 to T1.
template <Algebraic T1, Algebraic T2>
[[nodiscard]] auto staticQuaternionCast( const quaternion<T2>& q ) -> quaternion<T1> {
    return quaternion<T1>{ static_cast<T1>( q.real() ), staticVectorCast<T1>( q.imag() ) };
}

// ============================================================================
// ======================= quaternion_base ====================================
// ============================================================================

template <typename T>
    requires Algebraic<T>
struct quaternion_base {
    using value_type = T;
    using self = quaternion_base<value_type>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    quaternion_base() {
        _data[0] = value_type{};
        _data[1] = value_type{};
        _data[2] = value_type{};
        _data[3] = value_type{};
    }

    explicit quaternion_base( const quaternion_base& rhs ) = default;

    explicit quaternion_base( const std::array<T, 4>& rhs ) {
        _data[0] = rhs[0];
        _data[1] = rhs[1];
        _data[2] = rhs[2];
        _data[3] = rhs[3];
    }

    quaternion_base( value_type x, value_type y, value_type z, value_type w ) {
        _data[0] = x;
        _data[1] = y;
        _data[2] = z;
        _data[3] = w;
    }

    quaternion_base( const algebra::vector3<value_type>& a,
                     const algebra::vector3<value_type>& b ) {
        byTwoVectors( a, b );
    }

    ~quaternion_base() = default;

    auto operator[]( size_t id ) -> reference { return _data[id]; }

    auto operator[]( size_t id ) const -> const_reference { return _data[id]; }

    auto x() const -> value_type { return _data[0]; }

    auto y() const -> value_type { return _data[1]; }

    auto z() const -> value_type { return _data[2]; }

    auto w() const -> value_type { return _data[3]; }

    auto length() -> value_type {
        // TODO: call proper square root function for float and double
        return algebra::abs( toCanonical() );
    }

    auto normalizeSelf() -> void {
        const auto len = length();

        if ( len > 0.0 ) {
            _data = fromCanonical( algebra::normalize( toCanonical() ) )._data;
        }
    }

    auto multiply( const self& other ) -> self {
        return fromCanonical( toCanonical() * other.toCanonical() );
    }

    // Assume that a and b is normalized
    auto byTwoVectors( const algebra::vector3<value_type>& a,
                       const algebra::vector3<value_type>& b ) -> void {
        algebra::vector3<value_type> cpv = a.cross( b );

        _data[0] = cpv.x();
        _data[1] = cpv.y();
        _data[2] = cpv.z();
        _data[3] = sqrt( ( a.sqLength() ) * ( b.sqLength() ) ) + a.dot( b );

        if ( length() <= 0.0 ) {
            // Antiparallel vectors: pick any axis orthogonal to a, 180 degree turn.
            algebra::vector3<value_type> axis;
            if ( std::fabs( a.x() ) < value_type{ 0.9 } ) {
                axis = a.cross( { value_type{ 1 }, value_type{ 0 }, value_type{ 0 } } );
            } else {
                axis = a.cross( { value_type{ 0 }, value_type{ 1 }, value_type{ 0 } } );
            }
            axis.normalizeSelf();
            _data[0] = axis.x();
            _data[1] = axis.y();
            _data[2] = axis.z();
            _data[3] = value_type{ 0 };
            return;
        }

        normalizeSelf();
    }

    auto toEuler() const -> std::array<value_type, 3> {
        std::array<value_type, 3> angles;  //yaw pitch roll
        const auto x = this->x();
        const auto y = this->y();
        const auto z = this->z();
        const auto w = this->w();

        // roll (x-axis rotation)
        double sinr_cosp = 2.0 * ( w * x + y * z );
        double cosr_cosp = 1.0 - 2.0 * ( x * x + y * y );
        angles[2] = radToDeg( std::atan2( sinr_cosp, cosr_cosp ) );

        // pitch (y-axis rotation)
        double sinp = 2.0 * ( w * y - z * x );
        if ( std::abs( sinp ) >= 1.0 ) {
            angles[1] = radToDeg(
                std::copysign( std::numbers::pi_v<value_type> / 2.0,
                               sinp ) );  // use 90 degrees if out of range
        } else {
            angles[1] = radToDeg( std::asin( sinp ) );
        }

        // yaw (z-axis rotation)
        double siny_cosp = 2.0 * ( w * z + x * y );
        double cosy_cosp = 1.0 - 2.0 * ( y * y + z * z );
        angles[0] = radToDeg( std::atan2( siny_cosp, cosy_cosp ) );

        return angles;
    }

    auto operator=( const quaternion_base& rhs ) -> quaternion_base& {
        if ( this != &rhs ) {
            _data[0] = rhs._data[0];
            _data[1] = rhs._data[1];
            _data[2] = rhs._data[2];
            _data[3] = rhs._data[3];
        }

        return *this;
    };

    auto operator+( const quaternion_base& rhs ) -> quaternion_base {
        return fromCanonical( toCanonical() + rhs.toCanonical() );
    }

    auto operator+=( const quaternion_base& rhs ) -> quaternion_base& {
        _data = fromCanonical( toCanonical() + rhs.toCanonical() )._data;
        return *this;
    }

    auto operator-( const quaternion_base& rhs ) -> quaternion_base {
        return fromCanonical( toCanonical() - rhs.toCanonical() );
    }

    auto operator-=( const quaternion_base& rhs ) -> quaternion_base& {
        _data = fromCanonical( toCanonical() - rhs.toCanonical() )._data;
        return *this;
    }

    auto operator=( quaternion_base&& rhs ) -> quaternion_base& = delete;

    [[nodiscard]] auto data() -> pointer { return _data.data(); };

    [[nodiscard]] auto data() const -> const_pointer { return _data.data(); };

private:
    auto toCanonical() const -> quaternion<value_type> {
        return quaternion<value_type>{ _data[3], vector<value_type, 3>{ _data[0], _data[1], _data[2] } };
    }

    static auto fromCanonical( const quaternion<value_type>& q ) -> self {
        return self{ q.imag()[0], q.imag()[1], q.imag()[2], q.real() };
    }

    std::array<value_type, 4> _data{};
};

using quaternionl = quaternion_base<long long>;
using quaternionf = quaternion_base<float>;
using quaterniond = quaternion_base<double>;

}  // namespace tire::algebra
