
#pragma once

#include <cstddef>
#include <initializer_list>
#include <cmath>
#include <array>
#include <utility>

#include "concepts.h"
#include "vector3.h"

namespace tire::algebra {

 template <typename T>
    requires Algebraic<T>
struct matrix3 final {
    using value_type = T;
    using self = matrix3<value_type>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    matrix3() {
        //
        idtt();
    }

    [[nodiscard]]
    auto operator[]( size_t index ) -> reference {
        //
        return data_[index];
    }

    [[nodiscard]]
    auto operator[]( size_t index ) const -> const_reference {
        //
        return data_[index];
    }

    [[nodiscard]]
    auto size() const -> size_t {
        //
        return data_.size();
    }

    [[nodiscard]]
    auto data() -> pointer {
        //
        return data_.data();
    }

    [[nodiscard]]
    auto data() const -> const_pointer {
        //
        return data_.data();
    }

    void zero() {
        data_[0] = T{ 0 };
        data_[1] = T{ 0 };
        data_[2] = T{ 0 };
        data_[3] = T{ 0 };
        data_[4] = T{ 0 };
        data_[5] = T{ 0 };
        data_[6] = T{ 0 };
        data_[7] = T{ 0 };
        data_[8] = T{ 0 };
    }

    void idtt() {
        data_[0] = T{ 1 };
        data_[2] = T{ 0 };
        data_[3] = T{ 0 };
        data_[1] = T{ 0 };
        data_[4] = T{ 1 };
        data_[5] = T{ 0 };
        data_[6] = T{ 0 };
        data_[7] = T{ 0 };
        data_[8] = T{ 1 };
    }

    matrix3( std::initializer_list<value_type> list ) {
        for ( auto i = 0; const auto e : list ) {
            data_[i] = e;
            ++i;
        }
    }

    // Storage convention: row-major, data_[i*3 + j] is row i, column j.
    [[nodiscard]]
    auto transpose() -> self {
        auto rt = *this;
        rt.transposeSelf();
        return rt;
    }

    auto transposeSelf() -> void {
        for ( size_t i = 0; i < 3; ++i ) {
            for ( size_t j = i + 1; j < 3; ++j ) {
                std::swap( data_[i * 3 + j], data_[j * 3 + i] );
            }
        }
    }

    auto multiply( const self& rhs ) -> void {
        std::array<value_type, 9> tmp{};
        for ( size_t i = 0; i < 3; ++i ) {
            for ( size_t j = 0; j < 3; ++j ) {
                for ( size_t k = 0; k < 3; ++k ) {
                    tmp[i * 3 + j] += data_[i * 3 + k] * rhs[k * 3 + j];
                }
            }
        }
        data_ = tmp;
    }

    auto operator*( const self& rhs ) const -> self {
        auto tmp{ *this };
        tmp.multiply( rhs );
        return tmp;
    }

    auto mult_vector3( const vector3<value_type>& v ) -> vector3<value_type> {
        vector3<value_type> rt;

        rt[0] = v[0] * ( *this )[0] + v[1] * ( *this )[1] + v[2] * ( *this )[2];
        rt[1] = v[0] * ( *this )[3] + v[1] * ( *this )[4] + v[2] * ( *this )[5];
        rt[2] = v[0] * ( *this )[6] + v[1] * ( *this )[7] + v[2] * ( *this )[8];

        return rt;
    }

    // NOTE: LLM generated.
    [[nodiscard]]
    auto determinant() -> value_type {
        return data_[0] * ( data_[4] * data_[8] - data_[5] * data_[7] ) -
               data_[1] * ( data_[3] * data_[8] - data_[5] * data_[6] ) +
               data_[2] * ( data_[3] * data_[7] - data_[4] * data_[6] );
    }

    auto rotation( value_type yaw, value_type pitch, value_type roll ) -> void {
        auto siny = std::sin( yaw );
        auto cosy = std::cos( yaw );
        auto sinp = std::sin( pitch );
        auto cosp = std::cos( pitch );
        auto sinr = std::sin( roll );
        auto cosr = std::cos( roll );

        data_[0] = cosy * cosr - siny * cosp * sinr;
        data_[1] = -cosy * sinr - siny * cosp * cosr;
        data_[2] = siny * sinp;

        data_[3] = siny * cosr + cosy * cosp * sinr;
        data_[4] = -siny * sinr + cosy * cosp * cosr;
        data_[5] = -cosy * sinp;

        data_[6] = sinp * sinr;
        data_[7] = sinp * cosr;
        data_[8] = cosp;
    }

private:
    std::array<value_type, 9> data_;
};

using matrix3l = matrix3<long long>;
using matrix3f = matrix3<float>;
using matrix3d = matrix3<double>;

}  // namespace tire::algebra
