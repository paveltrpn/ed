
#pragma once

#include <concepts>
#include <type_traits>

namespace tire::algebra {

template <typename T>
concept Algebraic = std::integral<T> || std::floating_point<T>;

template <typename T>
concept SignedIntegral = std::integral<T> && std::signed_integral<T>;

template <typename T>
concept UnsignedIntegral = std::integral<T> && std::unsigned_integral<T>;

template <auto N>
concept Positive = ( N > 0 );

template <typename T>
requires Algebraic<T> struct matrix2;

template <typename T>
requires Algebraic<T> struct matrix3;

template <Algebraic T>
struct matrix4;

template <typename T>
requires Algebraic<T> struct vector2;

template <typename T>
requires Algebraic<T> struct vector3;

template <typename T>
requires Algebraic<T> struct vector4;

template <typename T>
concept MatrixFloat =
    std::is_same_v<T, matrix2<float>> || std::is_same_v<T, matrix3<float>> || std::is_same_v<T, matrix4<float>>;

template <typename T>
concept MatrixDouble =
    std::is_same_v<T, matrix2<double>> || std::is_same_v<T, matrix3<double>> || std::is_same_v<T, matrix4<double>>;

template <typename T>
concept VectorFloat =
    std::is_same_v<T, vector2<float>> || std::is_same_v<T, vector3<float>> || std::is_same_v<T, vector4<float>>;

template <typename T>
concept VectorDouble =
    std::is_same_v<T, vector2<double>> || std::is_same_v<T, vector3<double>> || std::is_same_v<T, vector4<double>>;

template <typename T>
concept VectorInt =
    std::is_same_v<T, vector2<int>> || std::is_same_v<T, vector3<int>> || std::is_same_v<T, vector4<int>>;

template <typename T>
concept VectorUInt = std::is_same_v<T, vector2<unsigned int>> || std::is_same_v<T, vector3<unsigned int>> ||
                     std::is_same_v<T, vector4<unsigned int>>;

template <typename T>
concept VectorLong = std::is_same_v<T, vector2<long long>> || std::is_same_v<T, vector3<long long>> ||
                     std::is_same_v<T, vector4<long long>>;

template <typename T>
concept ConceptMatrix = std::is_same_v<T, matrix4<long long>> || std::is_same_v<T, algebra::matrix4<float>> ||
                        std::is_same_v<T, matrix4<double>> || std::is_same_v<T, algebra::matrix4<int>> ||
                        std::is_same_v<T, matrix4<unsigned int>> || std::is_same_v<T, algebra::matrix3<long long>> ||
                        std::is_same_v<T, matrix3<float>> || std::is_same_v<T, algebra::matrix3<double>> ||
                        std::is_same_v<T, matrix3<int>> || std::is_same_v<T, algebra::matrix3<unsigned int>> ||
                        std::is_same_v<T, matrix2<long long>> || std::is_same_v<T, algebra::matrix2<float>> ||
                        std::is_same_v<T, matrix2<double>> || std::is_same_v<T, algebra::matrix2<int>> ||
                        std::is_same_v<T, matrix2<unsigned int>>;

}  // namespace tire::algebra
