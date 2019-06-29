#pragma once
#include <immintrin.h>
#include <string>
#include "print_simd_vectors.hpp"
#include "../type_definitions.hpp"

static const __m128 SIGNMASK =
    _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
static const __m256d SIGNMASK256d = _mm256_set1_pd(-0.0);

template <class T>
inline void hash_combine(std::size_t &seed, const T &v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std {
template <typename S, typename T>
struct hash<pair<S, T>> {
    inline size_t operator()(const pair<S, T> &v) const {
        size_t seed = 0;
        ::hash_combine(seed, v.first);
        ::hash_combine(seed, v.second);
        return seed;
    }
};
} // namespace std

// force loop unrolling
#pragma GCC push_options
#pragma GCC optimize("unroll-loops")

inline __attribute__((always_inline)) float
_mm_horizontal_max_ps(const __m128 &x) {
    __m128 max1 = _mm_shuffle_ps(x, x, _MM_SHUFFLE(0, 0, 3, 2));
    __m128 max2 = _mm_max_ps(x, max1);
    __m128 max3 =
        _mm_shuffle_ps(max2, max2, _MM_SHUFFLE(0, 0, 0, 1));
    __m128 max4 = _mm_max_ps(max2, max3);
    float result = _mm_cvtss_f32(max4);
    return result;
}

inline __attribute__((always_inline)) double
_mm256_horizontal_max_pd(const __m256d &x) {
    __m256d y =
        _mm256_permute2f128_pd(x, x, 1); // permute 128-bit values
    __m256d m1 =
        _mm256_max_pd(x, y); // m1[0] = max(x[0], x[2]), m1[1] =
                             // max(x[1], x[3]), etc.
    __m256d m2 = _mm256_permute_pd(
        m1, 5); // set m2[0] = m1[1], m2[1] = m1[0], etc.
    __m256d m = _mm256_max_pd(
        m1, m2); // all m[0] ... m[3] contain the
                 // horizontal max(x[0], x[1], x[2], x[3])
    return *((double *)&m);
}

/**
 *@brief Function loads 3 floats to an sse vector -> x:[32:0],
 *y:[32:64], z:[64:96]
 * @param value float pointer pointing to first element
 * @return returns __m128 vector, last element is 0.
 */
inline __attribute__((always_inline)) __m128
load_vertex(float *value) {
    // load the x and y element of the float3 vector using a 64 bit
    // load and set the upper 64 bits to zero (00YX)
    __m128 xy = _mm_loadl_pi(_mm_setzero_ps(), (__m64 *)value);
    // now load the z element using a 32 bit load (000Z)
    __m128 z = _mm_load_ss(value + 2);
    // finally, combine both by moving the z component to the high
    // part of the register, while keeping x and y components in the
    // low part
    return _mm_movelh_ps(xy, z); // (0ZYX)
}

/**
 *Function loads 3 floats to an sse vector -> x:[32:0], y:[32:64],
 *z:[64:96]
 * @param value float pointer pointing to first element
 * @return returns __m128 vector, last element is 0.
 */
inline __attribute__((always_inline)) __m128
load_vertex2(float *value) {
    // load x, y with a 64 bit integer load (00YX)
    __m128i xy = _mm_loadl_epi64((__m128i *)value);
    // now load the z element using a 32 bit float load (000Z)
    __m128 z = _mm_load_ss(value + 2);
    // we now need to cast the __m128i register into a __m18 one
    // (0ZYX)
    return _mm_movelh_ps(_mm_castsi128_ps(xy), z);
}

/**
 * @brief Old version: Calculates cross product of two __m128
 * vectors (floats): a x b.
 * @param a __m128 vector
 * @param b __m128 vector
 */
inline __attribute__((always_inline)) __m128
cross_product_old(const __m128 &a, const __m128 &b) {
#ifdef __FMA__
    __m128 result = _mm_fmsub_ps(
        _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1)),
        _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2)),
        _mm_mul_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 1, 0, 2)),
                   _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1))));
#endif

#ifndef __FMA__
    __m128 result = _mm_sub_ps(
        _mm_mul_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1)),
                   _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2))),
        _mm_mul_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 1, 0, 2)),
                   _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1))));
#endif
    return result;
}

/**
 * @brief Calculates cross product of two __m128 vectors (floats): a
 * x b.
 * @param a __m128 vector
 * @param b __m128 vector
 */
inline __attribute__((always_inline)) __m128
cross_product(const __m128 &a, const __m128 &b) {
#ifndef __FMA__
    __m128 result = _mm_sub_ps(
        _mm_mul_ps(b,
                   _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1))),
        _mm_mul_ps(a,
                   _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1))));
#endif

#ifdef __FMA__
    __m128 result = _mm_fmsub_ps(
        b, _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1)),
        _mm_mul_ps(a,
                   _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1))));
#endif

    return _mm_shuffle_ps(result, result, _MM_SHUFFLE(3, 0, 2, 1));
}

#ifdef __AVX2__
/**
 * @brief Calculates cross product of two __m256d vectors (doubles):
 * a x b.
 * @param a __m256d vector
 * @param b __m256d vector
 */
inline __attribute__((always_inline)) __m256d
cross_product(const __m256d &a, const __m256d &b) {
    __m256d c = _mm256_permute4x64_pd(
        _mm256_fmsub_pd(
            a, _mm256_permute4x64_pd(b, _MM_SHUFFLE(3, 0, 2, 1)),
            _mm256_mul_pd(b, _mm256_permute4x64_pd(
                                 a, _MM_SHUFFLE(3, 0, 2, 1)))),
        _MM_SHUFFLE(3, 0, 2, 1));
    return c;
}
#endif

/**
 * @brief Calculates scalar product of two __m128 vectors (floats):
 * a x b.
 * @param a __m128 vector
 * @param b __m128 vector
 */
inline __attribute__((always_inline)) float
CalcDotProduct(const __m128 &x, const __m128 &y) {
    __m128 mulRes, shufReg, sumsReg;
    mulRes = _mm_mul_ps(x, y);

    // Calculates the sum of SSE Register -
    // https://stackoverflow.com/a/35270026/195787
    shufReg =
        _mm_movehdup_ps(mulRes); // Broadcast elements 3,1 to 2,0
    sumsReg = _mm_add_ps(mulRes, shufReg);
    shufReg =
        _mm_movehl_ps(shufReg, sumsReg); // High Half -> Low Half
    sumsReg = _mm_add_ss(sumsReg, shufReg);
    return _mm_cvtss_f32(
        sumsReg); // Result in the lower part of the SSE Register
}

/**
 * @brief Calculates cross product of two float vectors: a x b.
 * @param a  vector
 * @param b  vector
 */
inline __attribute__((always_inline)) float
CalcDotProduct(float vec1[3], float vec2[3]) {
    return vec1[0] * vec2[0] + vec1[1] * vec2[1] +
           vec1[2] * vec2[2];
}

#ifdef __AVX2__
/**
 * @brief Calculates cross product of two __m128 vectors (floats): a
 * x b.
 * @param a __m256d vector
 * @param b __m256d vector
 */
inline __attribute__((always_inline)) double
CalcDotProduct(const __m256d &x, const __m256d &y) {
    // double *tmp_d = (double *)(&x);
    // std::cout << "CalcDotProduct function: " << tmp_d[0] << " "
    // << tmp_d[1]
    //          << " " << tmp_d[2] << std::endl;
    __m256d tmp = _mm256_mul_pd(x, y);
    double *tmp_d = (double *)(&tmp);
    // std::cout << "CalcDotProduct function: " << tmp_d[0] << " "
    // << tmp_d[1]
    //          << " " << tmp_d[2] << std::endl;
    return tmp_d[0] + tmp_d[1] + tmp_d[2];
}
#endif

/**
 * @brief Calculates factorial of integer and returns vector
 * containing the result - this is slow and useless - do not use.
 * @param n calculates n!
 */
inline __attribute__((always_inline)) __m128i sse_factorial(int n) {
    __m128i prod = _mm_set_epi32(1, 1, 1, 1);
    __m128i onse = prod;
    __m128i factor = _mm_set_epi32(2, 2, 2, 2);
    //__m128i prod;
    // prod = _mm_cmpeq_epi32(prod, prod);
    for (int i = 2; i <= n; i++) {
        factor = _mm_add_epi32(factor, onse);
        prod = _mm_mul_epi32(prod, factor);
    }
    return prod;
}

/**
 * @brief Calculates factorial of the parameter.
 * @param n calculates n!
 */
inline __attribute__((always_inline)) int
scalar_factorial(const int &n) {
    int prod = 1;
    for (int i = 2; i <= n; i++) {
        prod *= i;
    }
    return prod;
}

/**
 * @brief Calculates single precision Chebyshev polynomials.
 * @param n number of Chebyshev polynomial to calculate
 * @param x_vec vector of x values for which the value of polynomial
 * is calculated
 */
inline __attribute__((always_inline)) __m128
chebyshev(const int &n, const __m128 &x_vec) {
    // T0
    __m128 T0 = _mm_set_ps1(1.0);
    if (n == 0)
        return T0;
    if (n == 1)
        return x_vec;

        // T2
#ifdef __FMA__
    __m128 T2 = _mm_fmsub_ps(_mm_mul_ps(_mm_set_ps1(2.0), x_vec),
                             x_vec, T0);
#endif
#ifndef __FMA__
    __m128 T2 = _mm_sub_ps(
        _mm_mul_ps(_mm_mul_ps(_mm_set_ps1(2.0), x_vec), x_vec), T0);
#endif
    if (n == 2)
        return T2;

        // T3
#ifdef __FMA__
    __m128 T3 = _mm_fmsub_ps(_mm_mul_ps(_mm_set_ps1(2.0), T2),
                             x_vec, x_vec);
#endif
#ifndef __FMA__
    __m128 T3 = _mm_sub_ps(
        _mm_mul_ps(_mm_mul_ps(_mm_set_ps1(2.0), T2), x_vec), x_vec);
#endif

    if (n == 3)
        return T3;
    __m128 Tn = T3;
    __m128 Tnm1 = T2;
    __m128 Tnp1;
    for (int i = 4; i <= n; i++) {
#ifdef __FMA__
        Tnp1 = _mm_fmsub_ps(_mm_mul_ps(_mm_set_ps1(2.0), Tn), x_vec,
                            Tnm1);
#endif
#ifndef __FMA__
        Tnp1 = _mm_sub_ps(
            _mm_mul_ps(_mm_mul_ps(_mm_set_ps1(2.0), Tn), x_vec),
            Tnm1);
#endif
        Tnm1 = Tn;
        Tn = Tnp1;
    }
    return Tnp1;
}

#ifdef __AVX2__
/**
 * @brief Calculates double precision Chebyshev polynomials.
 * @param n number of Chebyshev polynomial to calculate
 * @param x_vec vector of x values for which the value of polynomial
 * is calculated
 */
inline __m256d chebyshev(const int &n, const __m256d &x_vec) {
    __m256d T0 = _mm256_set1_pd(1.0);
    if (n == 0)
        return _mm256_set1_pd(1.0);
    if (n == 1)
        return x_vec;

    // T2
    __m256d T2 = _mm256_fmsub_pd(
        _mm256_mul_pd(_mm256_set1_pd(2.0), x_vec), x_vec, T0);
    if (n == 2)
        return T2;

    // T3
    __m256d T3 = _mm256_fmsub_pd(
        _mm256_mul_pd(_mm256_set1_pd(2.0), T2), x_vec, x_vec);

    if (n == 3)
        return T3;
    __m256d Tn = T3;
    __m256d Tnm1 = T2;
    __m256d Tnp1;
    for (int i = 4; i <= n; i++) {
        Tnp1 = _mm256_fmsub_pd(
            _mm256_mul_pd(_mm256_set1_pd(2.0), Tn), x_vec, Tnm1);
        Tnm1 = Tn;
        Tn = Tnp1;
    }
    return Tnp1;
}
#endif

/**
 * @brief Given previous two chebyshev polynomials and vector of x
 * values, the function calculates next chebyshev polynomial.
 * @param n -th Chebyshev polynomial evaluated in points of x vector
 * @param n-1 -th Chebyshev polynomial evaluated in points of x
 * vector
 * @param x_vec vector of x values in which the polynomial is
 * calculated
 */
inline __m128 chebyshev_next(const __m128 &cn, const __m128 &cn_1,
                             const __m128 &x_vec) {
#ifdef __FMA__
    return _mm_fmsub_ps(_mm_mul_ps(_mm_set_ps1(2.0), cn), x_vec,
                        cn_1);
#endif
#ifndef __FMA__
    return _mm_sub_ps(
        _mm_mul_ps(_mm_mul_ps(_mm_set_ps1(2.0), cn), x_vec), cn_1);
#endif
}

#ifdef __AVX2__
/**
 * @brief Given previous two chebyshev polynomials and vector of x
 * values, the function calculates next chebyshev polynomial.
 * @param n -th Chebyshev polynomial evaluated in points of x vector
 * @param n-1 -th Chebyshev polynomial evaluated in points of x
 * vector
 * @param x_vec vector of x values in which the polynomial is
 * calculated
 */
inline __m256d chebyshev_next(const __m256d &cn,
                              const __m256d &cn_1,
                              const __m256d &x_vec) {
    return _mm256_fmsub_pd(_mm256_mul_pd(_mm256_set1_pd(2.0), cn),
                           x_vec, cn_1);
}
#endif

/**
 * @brief given two consequtive Legendre polynomials, this code
 * calculates next Legendre polynomial
 * @param x_vec vector of x values for which the next polynomial is
 * calculated calculated
 */
inline __m128 legendre_next(const __m128 &Pn, const __m128 &Pnm1,
                            const __m128 &x_vec, const int &n) {
    __m128 n_vec = _mm_set_ps1(n);
    __m128 np1_vec = _mm_set_ps1(n + 1);
#ifdef __FMA__
    __m128 coeff1 =
        _mm_div_ps(_mm_mul_ps(_mm_fmadd_ps(_mm_set_ps1(2.0), n_vec,
                                           _mm_set_ps1(1.0)),
                              x_vec),
                   np1_vec);
    __m128 result = _mm_fmsub_ps(
        coeff1, Pn, _mm_mul_ps(_mm_div_ps(n_vec, np1_vec), Pnm1));
#endif

#ifndef __FMA__
    __m128 coeff1 = _mm_div_ps(
        _mm_mul_ps(_mm_add_ps(_mm_mul_ps(_mm_set_ps1(2.0), n_vec),
                              _mm_set_ps1(1.0)),
                   x_vec),
        np1_vec);
    __m128 result =
        _mm_sub_ps(_mm_mul_ps(coeff1, Pn),
                   _mm_mul_ps(_mm_div_ps(n_vec, np1_vec), Pnm1));
#endif
    return result;
}

/**
 * @brief The function finds all integer pairs whose
 * multiplication yield the supplied integer.
 * @param number to be factorized
 */
inline std::vector<std::pair<int, int>> find_products(int num) {
    int upper_bound = std::floor(std::sqrt((float)num));
    std::vector<std::pair<int, int>> pairs;
    pairs.reserve(upper_bound / 2);

    for (int i = 1; i <= upper_bound; i++) {
        if (num % i == 0) {
            std::pair<int, int> p({i, num / i});
            pairs.push_back(std::make_pair(i, num / i));
        }
    }
    return pairs;
}

/**
 * @brief The function return closest pair of numbers in a vector of
 * pairs
 * @param Vector of pairs among which to find closest pair
 */
inline std::pair<int, int>
closest_pair(std::vector<std::pair<int, int>> pairs) {
    std::pair<int, int> min_el = *std::min_element(
        std::begin(pairs), std::end(pairs),
        [](std::pair<int, int> a, std::pair<int, int> b) {
            return std::abs(a.first - a.second) <
                   std::abs(b.first - b.second);
        });
    return min_el;
}

/** @brief Integer division
 *
 */
inline __attribute__((always_inline)) __m128i
_mm_div_epi32(const __m128i &a, const __m128i &b) {
    __m128 x = _mm_cvtepi32_ps(a);
    __m128 y = _mm_cvtepi32_ps(b);
    return _mm_cvttps_epi32(
        _mm_div_ps(_mm_mul_ps(x, _mm_set1_ps(1.0000001)), y));
}

/** @brief This function calculates element wise modulo. This modulo
 * works only for positive integers.
 */
inline __attribute__((always_inline)) __m128i
_mm_positive_mod_epi32(const __m128i &numerator,
                       const __m128i &denominator) {
    __m128i quotient = _mm_div_epi32(numerator, denominator);

    __m128i modulo = _mm_sub_epi32(
        numerator, _mm_mullo_epi32(quotient, denominator));
    return modulo;
};

#pragma GCC pop_options
