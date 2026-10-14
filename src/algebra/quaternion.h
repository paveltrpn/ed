
#pragma once

#include <cmath>
#include <array>
#include <numbers>

#include "common.h"
#include "concepts.h"
#include "vector3.h"

namespace tire::algebra {

template <typename T>
    requires Algebraic<T>
struct quaternion_base {
    using value_type = T;
    using self = quaternion_base<value_type>;
    using reference = self&;
    using const_reference = const self&;
    using pointer = self*;
    using const_pointer = const self*;

    quaternion_base() {
        data_[0] = value_type{};
        data_[1] = value_type{};
        data_[2] = value_type{};
        data_[3] = value_type{};
    }

    explicit quaternion_base( const quaternion_base& rhs ) = default;

    explicit quaternion_base( const std::array<T, 4>& rhs ) {
        data_[0] = rhs[0];
        data_[1] = rhs[1];
        data_[2] = rhs[2];
        data_[3] = rhs[3];
    }

    quaternion_base( value_type x, value_type y, value_type z, value_type w ) {
        data_[0] = x;
        data_[1] = y;
        data_[2] = z;
        data_[3] = w;
    }

    quaternion_base( const algebra::vector3<value_type>& a,
                     const algebra::vector3<value_type>& b ) {
        byTwoVectors( a, b );
    }

    ~quaternion_base() = default;

    reference operator[]( size_t id ) { return data_[id]; }

    const_reference operator[]( size_t id ) const { return data_[id]; }

    value_type x() const { return data_[0]; }

    value_type y() const { return data_[1]; }

    value_type z() const { return data_[2]; }

    value_type w() const { return data_[3]; }

    value_type length() {
        auto len = data_[0] * data_[0] + data_[1] * data_[1] +
                   data_[2] * data_[2] + data_[3] * data_[3];

        // TODO: call proper square root function for float and double
        return std::sqrt( len );
    }

    void normalizeSelf() {
        auto len = length();

        if ( len > 0.0 ) {
            data_[0] /= len;
            data_[1] /= len;
            data_[2] /= len;
            data_[3] /= len;
        }
    }

    self multiply( const self& other ) {
        value_type A, B, C, D, E, F, G, H;

        A = ( this->w() + this->x() ) * ( other.w() + other.x() );
        B = ( this->z() - this->y() ) * ( other.y() - other.z() );
        C = ( this->x() - this->w() ) * ( other.y() + other.z() );
        D = ( this->y() + this->z() ) * ( other.x() - other.w() );
        E = ( this->x() + this->z() ) * ( other.x() + other.y() );
        F = ( this->x() - this->z() ) * ( other.x() - other.y() );
        G = ( this->w() + this->y() ) * ( other.w() - other.z() );
        H = ( this->w() - this->y() ) * ( other.w() + other.z() );

        value_type w = B + ( -E - F + G + H ) * 0.5;
        value_type x = A - ( E + F + G + H ) * 0.5;
        value_type y = -C + ( E - F + G - H ) * 0.5;
        value_type z = -D + ( E - F - G + H ) * 0.5;

        return { x, y, z, w };
    }

    // Assume that a and b is normalized
    void byTwoVectors( const algebra::vector3<value_type>& a,
                       const algebra::vector3<value_type>& b ) {
        algebra::vector3<value_type> cpv = a.cross( b );

        data_[0] = cpv.x();
        data_[1] = cpv.y();
        data_[2] = cpv.z();
        data_[3] = sqrt( ( a.sqLength() ) * ( b.sqLength() ) ) + a.dot( b );

        if ( length() <= 0.0 ) {
            // Antiparallel vectors: pick any axis orthogonal to a, 180 degree turn.
            algebra::vector3<value_type> axis = std::fabs( a.x() ) < value_type{ 0.9 }
                                                    ? a.cross( { value_type{ 1 }, value_type{ 0 }, value_type{ 0 } } )
                                                    : a.cross( { value_type{ 0 }, value_type{ 1 }, value_type{ 0 } } );
            axis.normalizeSelf();
            data_[0] = axis.x();
            data_[1] = axis.y();
            data_[2] = axis.z();
            data_[3] = value_type{ 0 };
            return;
        }

        normalizeSelf();
    }

    std::array<value_type, 3> toEuler() const {
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

    quaternion_base& operator=( const quaternion_base& rhs ) {
        if ( this != &rhs ) {
            data_[0] = rhs.data_[0];
            data_[1] = rhs.data_[1];
            data_[2] = rhs.data_[2];
            data_[3] = rhs.data_[3];
        }

        return *this;
    };

    quaternion_base operator+( const quaternion_base& rhs ) {
        return quaternion_base{ data_[0] + rhs[0], data_[1] + rhs[1],
                                data_[2] + rhs[2], data_[3] + rhs[3] };
    }

    quaternion_base& operator+=( const quaternion_base& rhs ) {
        data_[0] += rhs.data_[0];
        data_[1] += rhs.data_[1];
        data_[2] += rhs.data_[2];
        data_[3] += rhs.data_[3];
        return *this;
    }

    quaternion_base operator-( const quaternion_base& rhs ) {
        return quaternion_base{ data_[0] - rhs[0], data_[1] - rhs[1],
                                data_[2] - rhs[2], data_[3] - rhs[3] };
    }

    quaternion_base& operator-=( const quaternion_base& rhs ) {
        data_[0] -= rhs.data_[0];
        data_[1] -= rhs.data_[1];
        data_[2] -= rhs.data_[2];
        data_[3] -= rhs.data_[3];
        return *this;
    }

    quaternion_base& operator=( quaternion_base&& rhs ) = delete;

    [[nodiscard]] pointer data() { return data_.data(); };

    [[nodiscard]] const_pointer data() const { return data_.data(); };

private:
    std::array<value_type, 4> data_{};
};

using quaternionl = quaternion_base<long long>;
using quaternionf = quaternion_base<float>;
using quaterniond = quaternion_base<double>;

}  // namespace tire::algebra
