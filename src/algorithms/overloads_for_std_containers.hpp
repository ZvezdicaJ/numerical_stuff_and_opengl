#pragma once

#include <array>

template <typename T, size_t N>
std::array<T, N> operator*(const std::array<T, N> &arr1,
                           const std::array<T, N> &arr2) {
    static_assert(
        std::is_arithmetic<T>::value &&
        "* operator overload for array is only defined for arithmetic types!");

    std::array<T, N> result;

    for (unsigned i = 0; i < arr1.size(); i++) {
        result[i] = arr1[i] * arr2[i];
    }

    return result;
}

template <>
std::array<float, 4> operator*(const std::array<float, 4> &arr1,
                               const std::array<float, 4> &arr2) {
    __m128 array1 = _mm_loadu_ps(arr1.data());
    __m128 array2 = _mm_loadu_ps(arr2.data());
    std::array<float, 4> result;
    _mm_storeu_ps(result.data(), _mm_mul_ps(array1, array2));
    return result;
}

template <>
std::array<double, 4> operator*(const std::array<double, 4> &arr1,
                                const std::array<double, 4> &arr2) {
    __m256d array1 = _mm256_loadu_pd(arr1.data());
    __m256d array2 = _mm256_loadu_pd(arr2.data());
    std::array<double, 4> result;
    _mm256_storeu_pd(result.data(), _mm256_mul_pd(array1, array2));
    return result;
}

template <>
std::array<int, 4> operator*(const std::array<int, 4> &arr1,
                             const std::array<int, 4> &arr2) {
    __m128i array1 = _mm_loadu_si128((__m128i *)arr1.data());
    __m128i array2 = _mm_loadu_si128((__m128i *)arr2.data());
    std::array<int, 4> result;
    _mm_storeu_si128((__m128i *)result.data(), _mm_mul_epi32(array1, array2));
    return result;
}

template <typename T>
std::vector<T> operator*(const std::vector<T> &vec1,
                         const std::vector<T> &vec2) {
    static_assert(
        std::is_arithmetic<T>::value &&
        "* operator overload for vector is only defined for arithmetic types!");

    if (vec1.size() != vec2.size())
        std::cerr << "* operator overload: vector not of the same size";

    std::vector<T> result;

    for (unsigned i = 0; i < vec1.size(); i++) {
        result[i] = vec1[i] * vec2[i];
    }

    return result;
}
