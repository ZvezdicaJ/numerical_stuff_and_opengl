#pragma once

#include <array>
#include <immintrin.h>
//////////////////////////////////////////////////////////////
// OVERLOADS FOR ARRAY
// MULTIPLICATION FOR ARRAY
template <typename T, size_t N>
std::array<T, N> operator*(const std::array<T, N> &arr1,
                           const std::array<T, N> &arr2) {
    static_assert(std::is_arithmetic<T>::value &&
                  "* operator overload for array is only defined "
                  "for arithmetic types!");

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
    __m128i res = _mm_mullo_epi32(array1, array2);

    _mm_storeu_si128((__m128i *)result.data(), res);
    return result;
}

////////////////////////////////////////
// ADDITION FOR ARRAYS
template <typename T, size_t N>
std::array<T, N> operator+(const std::array<T, N> &arr1,
                           const std::array<T, N> &arr2) {
    static_assert(std::is_arithmetic<T>::value &&
                  "* operator overload for array is only defined "
                  "for arithmetic types!");

    std::array<T, N> result;

    for (unsigned i = 0; i < arr1.size(); i++) {
        result[i] = arr1[i] + arr2[i];
    }

    return result;
}

template <>
std::array<float, 4> operator+(const std::array<float, 4> &arr1,
                               const std::array<float, 4> &arr2) {
    __m128 array1 = _mm_loadu_ps(arr1.data());
    __m128 array2 = _mm_loadu_ps(arr2.data());
    std::array<float, 4> result;
    _mm_storeu_ps(result.data(), _mm_add_ps(array1, array2));
    return result;
}

template <>
std::array<double, 4> operator+(const std::array<double, 4> &arr1,
                                const std::array<double, 4> &arr2) {
    __m256d array1 = _mm256_loadu_pd(arr1.data());
    __m256d array2 = _mm256_loadu_pd(arr2.data());
    std::array<double, 4> result;
    _mm256_storeu_pd(result.data(), _mm256_add_pd(array1, array2));
    return result;
}

template <>
std::array<int, 4> operator+(const std::array<int, 4> &arr1,
                             const std::array<int, 4> &arr2) {
    __m128i array1 = _mm_loadu_si128((__m128i *)arr1.data());
    __m128i array2 = _mm_loadu_si128((__m128i *)arr2.data());

    std::array<int, 4> result;
    __m128i res = _mm_add_epi32(array1, array2);

    _mm_storeu_si128((__m128i *)result.data(), res);
    return result;
}

////////////////////////////////////////
// DIVISION FOR ARRAYS
template <typename T, size_t N>
std::array<T, N> operator/(const std::array<T, N> &arr1,
                           const std::array<T, N> &arr2) {
    static_assert(std::is_arithmetic<T>::value &&
                  "* operator overload for array is only defined "
                  "for arithmetic types!");

    std::array<T, N> result;

    for (unsigned i = 0; i < arr1.size(); i++) {
        result[i] = arr1[i] / arr2[i];
    }

    return result;
}

template <>
std::array<float, 4> operator/(const std::array<float, 4> &arr1,
                               const std::array<float, 4> &arr2) {
    __m128 array1 = _mm_loadu_ps(arr1.data());
    __m128 array2 = _mm_loadu_ps(arr2.data());
    std::array<float, 4> result;
    _mm_storeu_ps(result.data(), _mm_div_ps(array1, array2));
    return result;
}

template <>
std::array<double, 4> operator/(const std::array<double, 4> &arr1,
                                const std::array<double, 4> &arr2) {
    __m256d array1 = _mm256_loadu_pd(arr1.data());
    __m256d array2 = _mm256_loadu_pd(arr2.data());
    std::array<double, 4> result;
    _mm256_storeu_pd(result.data(), _mm256_div_pd(array1, array2));
    return result;
}
/*
//  not working currently
//  fails for divisions of type 5 / (-3) where one number is
//  positive and the other is negative

template <>
std::array<int, 4> operator/(const std::array<int, 4> &arr1,
                             const std::array<int, 4> &arr2) {
    __m128i array1 = _mm_loadu_si128((__m128i *)arr1.data());
    __m128i array2 = _mm_loadu_si128((__m128i *)arr2.data());
    print_sse(array1, "array1: ");
    print_sse(array2, "array2: ");

    std::array<int, 4> result;
    __m128 resf = _mm_floor_ps(_mm_div_ps(_mm_cvtepi32_ps(array1),
                                          _mm_cvtepi32_ps(array2)));

    __m128i res = _mm_cvtps_epi32(_mm_floor_ps(_mm_div_ps(
        _mm_cvtepi32_ps(array1), _mm_cvtepi32_ps(array2))));

    print_sse(res, "res");
    print_sse(resf, "resf");

    _mm_storeu_si128((__m128i *)result.data(), res);

    return result;
    }*/

//////////////////////////////////////////////////////////////
// OVERLOADS FOR VECTOR
template <typename T>
std::vector<T> operator*(const std::vector<T> &vec1,
                         const std::vector<T> &vec2) {
    static_assert(std::is_arithmetic<T>::value &&
                  "* operator overload for vector is only defined "
                  "for arithmetic types!");

    if (vec1.size() != vec2.size())
        std::cerr
            << "* operator overload: vector not of the same size";

    std::vector<T> result;
    for (unsigned i = 0; i < vec1.size(); i++) {
        result[i] = vec1[i] * vec2[i];
    }

    return result;
}

template <>
std::vector<float> operator*(const std::vector<float> &vec1,
                             const std::vector<float> &vec2) {
    static_assert(std::is_arithmetic<float>::value &&
                  "* operator overload for vector is only defined "
                  "for arithmetic types!");

    if (vec1.size() != vec2.size())
        std::cerr
            << "* operator overload: vector not of the same size";

    std::vector<float> result;
    result.reserve(vec1.size());
    result.resize(vec1.size());
    int i = 0;
    while (i < vec1.size()) {
        __m256 v1 = _mm256_loadu_ps(vec1.data() + i);
        __m256 v2 = _mm256_loadu_ps(vec2.data() + i);
        _mm256_storeu_ps(result.data() + i, _mm256_mul_ps(v1, v2));
        i += 8;
    }

    while (i < vec1.size()) {
        result[i] = vec1[i] * vec2[i];
        i++;
    }

    return result;
}
