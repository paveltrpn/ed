
#pragma once

#include <cstddef>
#include <initializer_list>
#include <array>
#include <utility>


#include "concepts.h"

namespace tire::algebra {

 template <typename T>
    requires Algebraic<T>
struct matrix2 final {
    using value_type = T;
    using self = matrix2<value_type>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    matrix2() {
        data_[0] = value_type{ 1 };
        data_[1] = value_type{ 0 };
        data_[2] = value_type{ 0 };
        data_[3] = value_type{ 1 };
    }

    matrix2( std::initializer_list<value_type> list ) {
        for ( auto i = 0; const auto e : list ) {
            data_[i] = e;
            ++i;
        }
    }

    [[nodiscard]]
    auto operator[]( size_t index ) -> reference {
        return data_[index];
    }

    [[nodiscard]]
    auto operator[]( size_t index ) const -> const_reference {
        return data_[index];
    }

    [[nodiscard]]
    auto size() -> size_t {
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

    // Storage convention: row-major, data_[i*2 + j] is row i, column j.
    [[nodiscard]] auto transpose() -> self {
        auto rt = *this;
        std::swap( rt.data_[1], rt.data_[2] );
        return rt;
    }

    auto transposeSelf() -> void {
        std::swap( data_[1], data_[2] );
    }

    auto multiply( const self& rhs ) -> void {
        std::array<value_type, 4> tmp{};
        for ( size_t i = 0; i < 2; ++i ) {
            for ( size_t j = 0; j < 2; ++j ) {
                for ( size_t k = 0; k < 2; ++k ) {
                    tmp[i * 2 + j] += data_[i * 2 + k] * rhs[k * 2 + j];
                }
            }
        }
        data_ = tmp;
    }

    [[nodiscard]]
    auto operator*( const self& rhs ) const -> self {
        auto tmp{ *this };
        tmp.multiply( rhs );
        return tmp;
    }

    [[nodiscard]]
    auto determinant() -> value_type {
        return data_[0] * data_[3] - data_[2] * data_[1];
    }

private:
    std::array<value_type, 4> data_;
};

using matrix2l = matrix2<long long>;
using matrix2f = matrix2<float>;
using matrix2d = matrix2<double>;

}  // namespace tire::algebra
