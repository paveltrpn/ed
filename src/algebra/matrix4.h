#pragma once

#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <limits>
#include <type_traits>

#include "common.h"
#include "concepts.h"
#include "matrix.h"
#include "vector3.h"
#include "vector4.h"

namespace tire::algebra {

// Storage convention: OpenGL column-major — data()[c*4 + r] is column c, row r,
// translation lives in data()[12..14]. Matches vsg::mat4 memory layout.
// Column-vector convention: transformed point = M * v.
// ============================================================================
// ======================== matrix4 ===========================================
// ============================================================================

template <Algebraic T>
struct matrix4 final {
    using value_type = T;
    using self = matrix4<value_type>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    matrix4() {
        //
        idtt();
    }

    matrix4( std::initializer_list<value_type> list ) {
        for ( auto i = 0; const auto e : list ) {
            data()[i] = e;
            ++i;
        }
    }

    [[nodiscard]] auto operator[]( size_t index ) -> reference {
        //
        return data()[index];
    }

    [[nodiscard]] auto operator[]( size_t index ) const -> const_reference {
        //
        return data()[index];
    }

    [[nodiscard]] auto size() const -> size_t {
        return 16;
    }

    [[nodiscard]] auto data() -> pointer {
        return &_data[0][0];
    }

    [[nodiscard]] auto data() const -> const_pointer {
        return &_data[0][0];
    }

    auto zero() -> void {
        _data = matrix<value_type, 4, 4>{};
    }

    auto idtt() -> void {
        _data = matrix<value_type, 4, 4>{ value_type{ 1 } };
    }

    [[nodiscard]] auto transpose() const -> self {
        return self{ algebra::transpose( _data ) };
    }

    auto transpose_self() -> void {
        _data = algebra::transpose( _data );
    }

    auto multiply( const self& rhs ) -> void {
        /*
        auto this00 = ( *this )[0, 0];
        auto this01 = ( *this )[0, 1];
        auto this02 = ( *this )[0, 2];

        ( *this )[0, 0] = this00 * rhs[0, 0] + this01 * rhs[1, 0] +
                          this02 * rhs[2, 0] + ( *this )[0, 3] * rhs[3, 0];
        ( *this )[0, 1] = this00 * rhs[0, 1] + this01 * rhs[1, 1] +
                          this02 * rhs[2, 1] + ( *this )[0, 3] * rhs[3, 1];
        ( *this )[0, 2] = this00 * rhs[0, 2] + this01 * rhs[1, 2] +
                          this02 * rhs[2, 2] + ( *this )[0, 3] * rhs[3, 2];
        ( *this )[0, 3] = this00 * rhs[0, 3] + this01 * rhs[1, 3] +
                          this02 * rhs[2, 3] + ( *this )[0, 3] * rhs[3, 3];

        auto this10 = ( *this )[1, 0];
        auto this11 = ( *this )[1, 1];
        auto this12 = ( *this )[1, 2];

        ( *this )[1, 0] = this10 * rhs[0, 0] + this11 * rhs[1, 0] +
                          this12 * rhs[2, 0] + ( *this )[1, 3] * rhs[3, 0];
        ( *this )[1, 1] = this10 * rhs[0, 1] + this11 * rhs[1, 1] +
                          this12 * rhs[2, 1] + ( *this )[1, 3] * rhs[3, 1];
        ( *this )[1, 2] = this10 * rhs[0, 2] + this11 * rhs[1, 2] +
                          this12 * rhs[2, 2] + ( *this )[1, 3] * rhs[3, 2];
        ( *this )[1, 3] = this10 * rhs[0, 3] + this11 * rhs[1, 3] +
                          this12 * rhs[2, 3] + ( *this )[1, 3] * rhs[3, 3];

        auto this20 = ( *this )[2, 0];
        auto this21 = ( *this )[2, 1];
        auto this22 = ( *this )[2, 2];

        ( *this )[2, 0] = this20 * rhs[0, 0] + this21 * rhs[1, 0] +
                          this22 * rhs[2, 0] + ( *this )[2, 3] * rhs[3, 0];
        ( *this )[2, 1] = this20 * rhs[0, 1] + this21 * rhs[1, 1] +
                          this22 * rhs[2, 1] + ( *this )[2, 3] * rhs[3, 1];
        ( *this )[2, 2] = this20 * rhs[0, 2] + this21 * rhs[1, 2] +
                          this22 * rhs[2, 2] + ( *this )[2, 3] * rhs[3, 2];
        ( *this )[2, 3] = this20 * rhs[0, 3] + this21 * rhs[1, 3] +
                          this22 * rhs[2, 3] + ( *this )[2, 3] * rhs[3, 3];

        auto this30 = ( *this )[3, 0];
        auto this31 = ( *this )[3, 1];
        auto this32 = ( *this )[3, 2];

        ( *this )[3, 0] = this30 * rhs[0, 0] + this31 * rhs[1, 0] +
                          this32 * rhs[2, 0] + ( *this )[3, 3] * rhs[3, 0];
        ( *this )[3, 1] = this30 * rhs[0, 1] + this31 * rhs[1, 1] +
                          this32 * rhs[2, 1] + ( *this )[3, 3] * rhs[3, 1];
        ( *this )[3, 2] = this30 * rhs[0, 2] + this31 * rhs[1, 2] +
                          this32 * rhs[2, 2] + ( *this )[3, 3] * rhs[3, 2];
        ( *this )[3, 3] = this30 * rhs[0, 3] + this31 * rhs[1, 3] +
                          this32 * rhs[2, 3] + ( *this )[3, 3] * rhs[3, 3];*/
        _data = _data * rhs._data;
    }

    auto operator*( const self& rhs ) const -> self {
        auto tmp{ *this };
        tmp.multiply( rhs );
        return tmp;
    }

    [[nodiscard]] auto mult_vector3( const vector3<value_type>& v ) const -> vector3<value_type> {
        value_type w{};

        auto rx = v.x() * ( *this )[0] + v.y() * ( *this )[4] + v.z() * ( *this )[8] + ( *this )[12];
        auto ry = v.x() * ( *this )[1] + v.y() * ( *this )[5] + v.z() * ( *this )[9] + ( *this )[13];
        auto rz = v.x() * ( *this )[2] + v.y() * ( *this )[6] + v.z() * ( *this )[10] + ( *this )[14];
        w = v.x() * ( *this )[3] + v.y() * ( *this )[7] + v.z() * ( *this )[11] + ( *this )[15];

        // Normalize if w is different than 1 (convert from homogeneous to Cartesian
        // coordinates).
        if ( w != 1.0f ) {
            rx /= w;
            ry /= w;
            rz /= w;
        }

        return { value_type( rx ), value_type( ry ), value_type( rz ) };
    }

    [[nodiscard]] auto mult_vector4( const vector4<value_type>& v ) const -> vector4<value_type> {
        const auto canonicalVector = vector<value_type, 4>{ v[0], v[1], v[2], v[3] };
        const auto result = _data * canonicalVector;
        return { result[0], result[1], result[2], result[3] };
    }

    // Full 4x4 determinant via cofactor expansion along row 0.
    [[nodiscard]] auto determinant() const -> value_type {
        return algebra::determinant( _data );
    }

    // Angles in radians. Column-major equivalent of the former row-major
    // Rz(yaw)*Rx(pitch)*Rz(roll) composition; transposing flips application
    // order, so the result applies yaw first, then pitch, then roll to a
    // column vector (equivalent to Rz(-roll)*Rx(-pitch)*Rz(-yaw) in the old
    // convention).
    auto rotation( value_type yaw, value_type pitch, value_type roll ) -> void {
        auto siny = std::sin( yaw );
        auto cosy = std::cos( yaw );
        auto sinp = std::sin( pitch );
        auto cosp = std::cos( pitch );
        auto sinr = std::sin( roll );
        auto cosr = std::cos( roll );

        data()[0] = cosy * cosr - siny * cosp * sinr;
        data()[4] = -cosy * sinr - siny * cosp * cosr;
        data()[8] = siny * sinp;
        data()[12] = 0.0;

        data()[1] = siny * cosr + cosy * cosp * sinr;
        data()[5] = -siny * sinr + cosy * cosp * cosr;
        data()[9] = -cosy * sinp;
        data()[13] = 0.0;

        data()[2] = sinp * sinr;
        data()[6] = sinp * cosr;
        data()[10] = cosp;
        data()[14] = 0.0;

        data()[3] = 0.0;
        data()[7] = 0.0;
        data()[11] = 0.0;
        data()[15] = 1.0;
    }

    auto perspective( value_type fov, value_type aspect, value_type near, value_type far ) -> void {
        const auto radFov = degToRad( fov );

        // NOTE: GNU related function sincos()
        // Choose sincos() function according to value_type.
        value_type sinFov{}, cosFov{};
        if constexpr ( std::is_same_v<value_type, double> ) {
            sincos( static_cast<value_type>( 0.5 ) * radFov, &sinFov, &cosFov );
        } else if constexpr ( std::is_same_v<value_type, float> ) {
            sincosf( static_cast<value_type>( 0.5 ) * radFov, &sinFov, &cosFov );
        }

        // value_type tanFov =
        // std::tan( static_cast<value_type>( 0.5 ) * radFov );
        value_type tanFov = sinFov / cosFov;
        value_type focal = static_cast<value_type>( 1.0 ) / tanFov;

        data()[0] = focal / aspect;
        data()[1] = value_type{ 0 };
        data()[2] = value_type{ 0 };
        data()[3] = value_type{ 0 };
        data()[4] = value_type{ 0 };
        data()[5] = focal;
        data()[6] = value_type{ 0 };
        data()[7] = value_type{ 0 };
        data()[8] = value_type{ 0 };
        data()[9] = value_type{ 0 };
        data()[11] = value_type{ -1 };
        data()[12] = value_type{ 0 };
        data()[13] = value_type{ 0 };
        data()[15] = value_type{ 0 };

        if ( far >= std::numeric_limits<value_type>::epsilon() ) {
            value_type nf{ static_cast<value_type>( 1.0 ) / ( far - near ) };
            data()[10] = -( far + near ) * nf;
            data()[14] = -( 2.0 * far * near ) * nf;
        } else {
            data()[10] = value_type{ -1 };
            data()[14] = -2.0 * near;
        }
    }

    auto vperspective( value_type fov, value_type aspect, value_type near, value_type far ) -> void {
        const auto radFov = degToRad( fov );

        // NOTE: GNU related function sincos()
        // Choose sincos() function according to value_type.

        value_type sinFov{}, cosFov{};
        if constexpr ( std::is_same_v<value_type, double> ) {
            sincos( static_cast<value_type>( 0.5 ) * radFov, &sinFov, &cosFov );
        } else if constexpr ( std::is_same_v<value_type, float> ) {
            sincosf( static_cast<value_type>( 0.5 ) * radFov, &sinFov, &cosFov );
        }

        // value_type tanFov = std::tan( value_type{ 0.5 } * radFov );
        value_type tanFov = sinFov / cosFov;
        value_type focal = 1.0 / tanFov;
        value_type nf{ near / ( far - near ) };

        data()[0] = focal / aspect;
        data()[4] = value_type{ 0 };
        data()[8] = value_type{ 0 };
        data()[12] = value_type{ 0 };

        data()[1] = value_type{ 0 };
        data()[5] = -focal;
        data()[9] = value_type{ 0 };
        data()[13] = value_type{ 0 };

        data()[2] = value_type{ 0 };
        data()[6] = value_type{ 0 };
        data()[10] = nf;
        data()[14] = far * nf;

        data()[3] = value_type{ 0 };
        data()[7] = value_type{ 0 };
        data()[11] = value_type{ -1 };
        data()[15] = value_type{ 0 };

        // TODO: Check and write in transpose version.
        /*
            const matrix4<value_type> inverse;
            inverse[0] = aspect / focal;
            inverse[1] = value_type{ 0 };
            inverse[2] = value_type{ 0 };
            inverse[3] = value_type{ 0 };

            inverse[4] = value_type{ 0 };
            inverse[5] = 1 / -focal;
            inverse[6] = value_type{ 0 };
            inverse[7] = value_type{ 0 };

            inverse[8] = value_type{ 0 };
            inverse[9] = value_type{ 0 };
            inverse[10] = value_type{ 0 };
            inverse[11] = value_type{ -1 };

            inverse[12] = value_type{ 0 };
            inverse[13] = value_type{ 0 };
            inverse[14] = 1 / ( far * nf );
            inverse[15] = nf / ( far * nf );
            */
    }

    auto vperspective2( value_type fov, value_type aspect, value_type near, value_type far ) -> void {
        const auto radFov = degToRad( fov );

        // NOTE: GNU related function sincos()
        // Choose sincos() function according to value_type.

        value_type sinFov{}, cosFov{};
        if constexpr ( std::is_same_v<value_type, double> ) {
            sincos( static_cast<value_type>( 0.5 ) * radFov, &sinFov, &cosFov );
        } else if constexpr ( std::is_same_v<value_type, float> ) {
            sincosf( static_cast<value_type>( 0.5 ) * radFov, &sinFov, &cosFov );
        }

        // value_type tanFov = std::tan( value_type{ 0.5 } * radFov );
        value_type tanFov = sinFov / cosFov;
        value_type focal = static_cast<value_type>( 1.0 ) / tanFov;
        value_type nf{ far / ( far - near ) };

        data()[0] = focal / aspect;
        data()[1] = value_type{ 0 };
        data()[2] = value_type{ 0 };
        data()[3] = value_type{ 0 };

        data()[4] = value_type{ 0 };
        data()[5] = focal;
        data()[6] = value_type{ 0 };
        data()[7] = value_type{ 0 };

        data()[8] = value_type{ 0 };
        data()[9] = value_type{ 0 };
        data()[10] = nf;
        data()[11] = -near * nf;

        data()[12] = value_type{ 0 };
        data()[13] = value_type{ 0 };
        data()[14] = value_type{ 1 };
        data()[15] = value_type{ 0 };
    }

    auto lookAt( const vector3<value_type>& eye, const vector3<value_type>& target, const vector3<value_type>& up )
        -> void {
        vector3<value_type> eyeDir;

        constexpr value_type floatEps = std::numeric_limits<value_type>::epsilon();
        if ( std::fabs( eye[0] - target[0] ) < floatEps && std::fabs( eye[1] - target[1] ) < floatEps &&
             std::fabs( eye[2] - target[2] ) < floatEps ) {
            return;
        }

        value_type z0 = eye[0] - target[0];
        value_type z1 = eye[1] - target[1];
        value_type z2 = eye[2] - target[2];

        value_type len = 1.0 / std::hypot( z0, z1, z2 );
        z0 *= len;
        z1 *= len;
        z2 *= len;

        value_type x0 = up[1] * z2 - up[2] * z1;
        value_type x1 = up[2] * z0 - up[0] * z2;
        value_type x2 = up[0] * z1 - up[1] * z0;
        len = std::hypot( x0, x1, x2 );
        if ( len == 0.0 ) {
            x0 = 0;
            x1 = 0;
            x2 = 0;
        } else {
            len = 1.0 / len;
            x0 *= len;
            x1 *= len;
            x2 *= len;
        }

        value_type y0 = z1 * x2 - z2 * x1;
        value_type y1 = z2 * x0 - z0 * x2;
        value_type y2 = z0 * x1 - z1 * x0;

        len = std::hypot( y0, y1, y2 );
        if ( len == 0.0 ) {
            y0 = 0;
            y1 = 0;
            y2 = 0;
        } else {
            len = 1.0 / len;
            y0 *= len;
            y1 *= len;
            y2 *= len;
        }

        data()[0] = x0;
        data()[1] = y0;
        data()[2] = z0;
        data()[3] = 0.0;
        data()[4] = x1;
        data()[5] = y1;
        data()[6] = z1;
        data()[7] = 0.0;
        data()[8] = x2;
        data()[9] = y2;
        data()[10] = z2;
        data()[11] = 0.0;
        data()[12] = -( x0 * eye[0] + x1 * eye[1] + x2 * eye[2] );
        data()[13] = -( y0 * eye[0] + y1 * eye[1] + y2 * eye[2] );
        data()[14] = -( z0 * eye[0] + z1 * eye[1] + z2 * eye[2] );
        data()[15] = 1.0;
    }

    auto orthographic( value_type left, value_type right, value_type bottom, value_type top, value_type near,
                       value_type far ) -> void {
        data()[0] = 2.0 / ( right - left );
        data()[1] = 0.0;
        data()[2] = 0.0;
        data()[3] = 0.0;

        data()[4] = 0.0;
        data()[5] = 2.0 / ( top - bottom );
        data()[6] = 0.0;
        data()[7] = 0.0;

        data()[8] = 0.0;
        data()[9] = 0.0;
        data()[10] = -2.0 / ( far - near );
        data()[11] = 0.0;

        data()[12] = -( right + left ) / ( right - left );
        data()[13] = -( top + bottom ) / ( top - bottom );
        data()[14] = -( far + near ) / ( far - near );
        data()[15] = 1.0;
    }

    auto scale( const vector3<value_type>& offset ) -> void {
        idtt();

        data()[0] = offset.x();
        data()[5] = offset.y();
        data()[10] = offset.z();
    }

    auto translate( const vector3<value_type>& offset ) -> void {
        idtt();

        data()[12] = offset.x();
        data()[13] = offset.y();
        data()[14] = offset.z();
    }

    auto translate( value_type dx, value_type dy, value_type dz ) -> void {
        idtt();

        data()[12] = dx;
        data()[13] = dy;
        data()[14] = dz;
    }

    auto rotation_yaw( value_type angl ) -> void {
        idtt();

        value_type sa{}, ca{};

        auto a = degToRad( angl );

        if constexpr ( std::is_same_v<value_type, double> ) {
            sincos( a, &sa, &ca );
        } else if constexpr ( std::is_same_v<value_type, float> ) {
            sincosf( a, &sa, &ca );
        }

        data()[5] = ca;
        data()[9] = -sa;
        data()[6] = sa;
        data()[10] = ca;
    }

    auto rotation_pitch( value_type angl ) -> void {
        idtt();

        value_type sa{}, ca{};

        auto a = degToRad( angl );

        if constexpr ( std::is_same_v<value_type, double> ) {
            sincos( a, &sa, &ca );
        } else if constexpr ( std::is_same_v<value_type, float> ) {
            sincosf( a, &sa, &ca );
        }

        data()[0] = ca;
        data()[8] = sa;
        data()[2] = -sa;
        data()[10] = ca;
    }

    auto rotation_roll( value_type angl ) -> void {
        idtt();

        value_type sa{}, ca{};

        auto a = degToRad( angl );

        if constexpr ( std::is_same_v<value_type, double> ) {
            sincos( a, &sa, &ca );
        } else if constexpr ( std::is_same_v<value_type, float> ) {
            sincosf( a, &sa, &ca );
        }

        data()[0] = ca;
        data()[4] = -sa;
        data()[1] = sa;
        data()[5] = ca;
    }

    auto euler( value_type yaw, value_type pitch, value_type roll ) -> void {
        self y, p, r;

        y.rotation_yaw( yaw );
        p.rotation_pitch( pitch );
        r.rotation_roll( roll );

        *this = y * p * r;
    }

    auto axis_angle( const vector3<value_type>& ax, value_type phi ) -> void {
        value_type cosphi, sinphi, vxvy, vxvz, vyvz, vx, vy, vz;

        cosphi = std::cos( degToRad( phi ) );
        sinphi = std::sin( degToRad( phi ) );
        vxvy = ax.x() * ax.y();
        vxvz = ax.x() * ax.z();
        vyvz = ax.y() * ax.z();
        vx = ax.x();
        vy = ax.y();
        vz = ax.z();

        data()[0] = cosphi + ( 1.0 - cosphi ) * vx * vx;
        data()[1] = ( 1.0 - cosphi ) * vxvy + sinphi * vz;
        data()[2] = ( 1.0 - cosphi ) * vxvz - sinphi * vy;
        data()[3] = 0.0f;

        data()[4] = ( 1.0 - cosphi ) * vxvy - sinphi * vz;
        data()[5] = cosphi + ( 1.0 - cosphi ) * vy * vy;
        data()[6] = ( 1.0 - cosphi ) * vyvz + sinphi * vx;
        data()[7] = 0.0f;

        data()[8] = ( 1.0 - cosphi ) * vxvz + sinphi * vy;
        data()[9] = ( 1.0 - cosphi ) * vyvz - sinphi * vx;
        data()[10] = cosphi + ( 1.0 - cosphi ) * vz * vz;
        data()[11] = 0.0f;

        data()[12] = 0.0f;
        data()[13] = 0.0f;
        data()[14] = 0.0f;
        data()[15] = 1.0f;
    }

    auto inverse() const -> self {
        if ( algebra::determinant( _data ) == value_type{ 0 } ) {
            return {};
        }
        return self{ algebra::inverse( _data ) };
    }

private:
    explicit matrix4( const matrix<value_type, 4, 4>& data ) : _data{ data } {}

    matrix<value_type, 4, 4> _data{};
};

template <typename T>
auto transpose( const matrix4<T>& arg ) -> matrix4<T> {
    return arg.transpose();
}

template <typename T>
auto translate( T dx, T dy, T dz ) -> matrix4<T> {
    matrix4<T> rt{};
    rt.translate( dx, dy, dz );
    return rt;
}

template <typename T>
auto translate( vector3<T>& offset ) -> matrix4<T> {
    matrix4<T> rt{};
    rt.translate( offset );
    return rt;
}

template <typename T>
auto rotate( T dy, T dp, T dr ) -> matrix4<T> {
    matrix4<T> rt;
    rt.euler( dy, dp, dr );
    return rt;
}

template <typename T>
auto rotate( vector3<T> ax, T phi ) -> matrix4<T> {
    matrix4<T> rt;
    rt.axis_angle( ax, phi );
    return rt;
}

template <typename T>
auto scale( T dx, T dy, T dz ) -> matrix4<T> {
    matrix4<T> rt;
    rt.scale( { dx, dy, dz } );
    return rt;
}

template <typename T>
auto perspective( float fov, float aspect, float ncp, float fcp ) -> matrix4<T> {
    matrix4<T> rt;
    rt.perspective( fov, aspect, ncp, fcp );
    return rt;
}

template <typename T>
auto vperspective( float fov, float aspect, float ncp, float fcp ) -> matrix4<T> {
    matrix4<T> rt;
    rt.vperspective( fov, aspect, ncp, fcp );
    return rt;
}

template <typename T>
auto vperspective2( float fov, float aspect, float ncp, float fcp ) -> matrix4<T> {
    matrix4<T> rt;
    rt.vperspective2( fov, aspect, ncp, fcp );
    return rt;
}

template <typename T>
auto orthographic( T left, T right, T bottom, T top, T near, T far ) -> matrix4<T> {
    matrix4<T> rt;
    rt.orthographic( left, right, bottom, top, near, far );
    return rt;
}

using matrix4l = matrix4<long long>;
using matrix4f = matrix4<float>;
using matrix4d = matrix4<double>;
}  // namespace tire::algebra
