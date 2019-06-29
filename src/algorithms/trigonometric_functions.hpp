#pragma once
#include <immintrin.h>
#include <string>
#include "print_simd_vectors.hpp"
#include "auxiliary_functions.hpp"
#include "../type_definitions.hpp"

/**
 * @brief Given previous two chebyshev polynomials and vector of x
 * values, the function calculates next chebyshev polynomial.
 * @param n -th Chebyshev polynomial evaluated in points of x vector
 * @param n-1 -th Chebyshev polynomial evaluated in points of x
 * vector
 * @param x_vec vector of x values in which the polynomial is
 * calculated
 */
inline __m128 cos(const __m128 &x_vec_) {
    float coeff[] = {
        -0.3042421776440938642020349128177049239697,
        -0.9708678652630182194109914323663784757039,
        0.3028491552626994215074191186309676140775,
        -0.02909193396501112114732073920800360778849,
        0.001392243991176231859984622208952274539411,
        -0.00004018994451075494298816526236368837878949,
        7.782767011815306088573057896947073998291e-7,
        -1.082653034185828481093421492678695775590e-8,
        1.135109177911507701030194019523024834037e-10,
        -9.295296632678756552885410084526215786661e-13,
        6.111364188334767723806229076684641965132e-15,
        -3.297657841343458986382435554107381460019e-17};

    __m128 sum =
        _mm_set_ps1(-0.3042421776440938642020349128177049239697);
    __m128 x_vec = _mm_div_ps(x_vec_, _mm_set_ps1(M_PI));
    __m128 T2n = _mm_set_ps1(1.0);
    __m128 T2np1 = x_vec;
    for (int n = 1; n < 9; n++) {
        __m128 coeff4 = _mm_set1_ps(coeff[n]);
        T2n = chebyshev_next(T2np1, T2n, x_vec);
        T2np1 = chebyshev_next(T2n, T2np1, x_vec);
#ifdef __FMA__
        sum = _mm_fmadd_ps(coeff4, T2n, sum);
#endif
#ifndef __FMA__
        sum = _mm_add_ps(_mm_mul_ps(coeff4, T2n), sum);
#endif
    }
    return sum;
}

#ifdef __AVX2__
/**
 * @brief calculates double precision cos.
 * @param x_vec vector of x values for which the cos is calculated
 */
inline __m256d cos(const __m256d &x_vec_) {
    float coeff[] = {
        -0.3042421776440938642020349128177049239697,
        -0.9708678652630182194109914323663784757039,
        0.3028491552626994215074191186309676140775,
        -0.02909193396501112114732073920800360778849,
        0.001392243991176231859984622208952274539411,
        -0.00004018994451075494298816526236368837878949,
        7.78276701181530608857305789694707399829 * 1e-7,
        -1.082653034185828481093421492678695775590 * 1e-8,
        1.135109177911507701030194019523024834037 * 1e-10,
        -9.29529663267875655288541008452621578666 * 1e-13,
        6.111364188334767723806229076684641965132 * 1e-15,
        -3.297657841343458986382435554107381460019 * 1e-17,
        1.486813423673207704795437109347570207022 * 1e-19,
        -5.685578136847164353761054026365713383437 * 1e-22};

    __m256d sum =
        _mm256_set1_pd(-0.3042421776440938642020349128177049239697);
    __m256d x_vec = _mm256_div_pd(x_vec_, _mm256_set1_pd(M_PI));
    __m256d T2n = _mm256_set1_pd(1.0);
    __m256d T2np1 = x_vec;
    for (int n = 1; n < 14; n++) {
        __m256d coeff4 = _mm256_set1_pd(coeff[n]);
        T2n = chebyshev_next(T2np1, T2n, x_vec);
        T2np1 = chebyshev_next(T2n, T2np1, x_vec);
        sum = _mm256_fmadd_pd(coeff4, T2n, sum);
    }
    return sum;
}
#endif

/**
 * @brief calculates single precision sin.
 * @param x_vec vector of x values for which the sin is calculated
 * calculated
 */
inline __m128 sin(const __m128 &x_vec_) {
    float coeff[] = {
        0.56923068635950551469,    -0.66691667240597907078,
        0.10428236873423694948,    -0.0068406335369915790099,
        0.00025000688495038622765, -5.8502483086391436917e-6,
        9.5347727502994011400e-8,  -1.1456384417094631513e-9,
        1.0574272617539128589e-11, -7.7352709954043070942e-14,
        4.5959561461829594592e-16, -2.2623059281974111043e-18};

    __m128 sum = _mm_set_ps1(0);
    __m128 x_vec = _mm_div_ps(x_vec_, _mm_set_ps1(M_PI));
    __m128 T2n = _mm_set_ps1(1.0);
    __m128 T2np1 = x_vec;
    for (int n = 0; n < 9; n++) {
        __m128 coeff4 = _mm_set1_ps(coeff[n]);
#ifdef __FMA__
        sum = _mm_fmadd_ps(coeff4, T2np1, sum);
#endif
#ifndef __FMA__
        sum = _mm_add_ps(_mm_mul_ps(coeff4, T2np1), sum);
#endif
        T2n = chebyshev_next(T2np1, T2n, x_vec);
        T2np1 = chebyshev_next(T2n, T2np1, x_vec);
    }
    return sum;
}

#ifdef __AVX2__
/**
 * @brief calculates double precision sin.
 * @param x_vec vector of x values for which the sin is calculated
 */
inline __m256d sin(const __m256d &x_vec_) {
    float coeff[] = {
        0.56923068635950551469,    -0.66691667240597907078,
        0.10428236873423694948,    -0.0068406335369915790099,
        0.00025000688495038622765, -5.8502483086391436917e-6,
        9.5347727502994011400e-8,  -1.1456384417094631513e-9,
        1.0574272617539128589e-11, -7.7352709954043070942e-14,
        4.5959561461829594592e-16, -2.2623059281974111043e-18};

    __m256d sum = _mm256_set1_pd(0);
    __m256d x_vec = _mm256_div_pd(x_vec_, _mm256_set1_pd(M_PI));
    __m256d T2n = _mm256_set1_pd(1.0);
    __m256d T2np1 = x_vec;
    for (int n = 0; n < 9; n++) {
        __m256d coeff4 = _mm256_set1_pd(coeff[n]);
        sum = _mm256_fmadd_pd(coeff4, T2np1, sum);
        T2n = chebyshev_next(T2np1, T2n, x_vec);
        T2np1 = chebyshev_next(T2n, T2np1, x_vec);
    }
    return sum;
}
#endif

/**
 *@brief This function calculates tangens of sse vector.
 *@details For detailed documentation on formula used check pdf
 *files discussing trigonometric functions. This is float version
 *and uses sse instructions.
 */
inline __m128 tan(const __m128 &x) {

    // this function should calculate tan to 1e-8 precision
    // stevec : 34459425 * a - 4729725 * a ^ 3 + 135135 * a ^ 5 -
    // 990 a ^ 7 + a ^ 9; this version seems to work best although
    // it should not x9 calculation should not work well for small x
    // since many multiplications loose precision
    __m128 x2 = _mm_mul_ps(x, x);
    __m128 x3 = _mm_mul_ps(x2, x);
    __m128 x4 = _mm_mul_ps(x2, x2);
    __m128 x5 = _mm_mul_ps(x3, x2);
    __m128 x6 = _mm_mul_ps(x3, x3);
    __m128 x7 = _mm_mul_ps(x3, x4);
    __m128 x8 = _mm_mul_ps(x4, x4);
    __m128 x9 = _mm_mul_ps(x4, x5);
    __m128 coeff1 = _mm_set_ps1(34459425);
#ifdef __FMA__
    __m128 numerator = _mm_fmsub_ps(
        x, coeff1,
        _mm_fmsub_ps(
            x3, _mm_set_ps1(4729725),
            _mm_fmsub_ps(x5, _mm_set_ps1(135135),
                         _mm_fmsub_ps(x7, _mm_set_ps1(990), x9))));
    // imenovalec: 34459425 - 16216200*a^2 + 945945 a^4 - 13860*a^6
    // + 45*a^8
    __m128 denominator = _mm_fmsub_ps(
        x8, _mm_set_ps1(45),
        _mm_fmsub_ps(
            x6, _mm_set_ps1(13860),
            _mm_fmsub_ps(
                x4, _mm_set_ps1(945945),
                _mm_fmsub_ps(x2, _mm_set_ps1(16216200), coeff1))));
#endif
#ifndef __FMA__
    __m128 numerator = _mm_sub_ps(
        _mm_mul_ps(x, coeff1),
        _mm_sub_ps(
            _mm_mul_ps(x3, _mm_set_ps1(4729725)),
            _mm_sub_ps(
                _mm_mul_ps(x5, _mm_set_ps1(135135)),
                _mm_sub_ps(_mm_mul_ps(x7, _mm_set_ps1(990)), x9))));
    // imenovalec: 34459425 - 16216200*a^2 + 945945 a^4 - 13860*a^6
    // + 45*a^8
    __m128 denominator = _mm_sub_ps(
        _mm_mul_ps(x8, _mm_set_ps1(45)),
        _mm_sub_ps(
            _mm_mul_ps(x6, _mm_set_ps1(13860)),
            _mm_sub_ps(
                _mm_mul_ps(x4, _mm_set_ps1(945945)),
                _mm_sub_ps(_mm_mul_ps(x2, _mm_set_ps1(16216200)),
                           coeff1))));
#endif
    return _mm_div_ps(numerator, denominator);
}

/**
 *@brief This function calculates tangens of sse vector. It's an
 *alternative to above function, but it's somehow (unexpectedly)
 *less accurate.
 *@details For detailed documentation on formula used check pdf
 *files discussing trigonometric functions. This is float version
 *and uses sse instructions.
 */
inline __m128 tan_ver2(const __m128 &x) {
    __m128 x2 = _mm_mul_ps(x, x);
    // stevec    -654729075 a + 91891800 a^3 - 2837835 a^5 + 25740
    // a^7 - 55 a^9
    __m128 numerator = _mm_sub_ps(x2, _mm_set_ps1(55));
    __m128 coeff1 = _mm_set_ps1(654729075);
#ifdef __FMA__
    numerator = _mm_fmadd_ps(x, _mm_mul_ps(x, numerator),
                             _mm_set_ps1(25740));
    numerator = _mm_fmsub_ps(x, _mm_mul_ps(x, numerator),
                             _mm_set_ps1(2837835));
    numerator = _mm_fmadd_ps(x, _mm_mul_ps(x, numerator),
                             _mm_set_ps1(91891800));
    numerator = _mm_fmsub_ps(x, _mm_mul_ps(x, numerator), coeff1);
    numerator = _mm_mul_ps(x, numerator);

    // imenovalec
    //   -654729075 + 310134825 a^2 - 18918900 a^4 + 315315 a^6 -
    //   1485 a^8 + a^10
    __m128 denominator = _mm_sub_ps(x2, _mm_set_ps1(-1485));
    denominator = _mm_fmadd_ps(x, _mm_mul_ps(x, denominator),
                               _mm_set_ps1(315315));
    denominator = _mm_fmsub_ps(x, _mm_mul_ps(x, denominator),
                               _mm_set_ps1(18918900));
    denominator = _mm_fmadd_ps(x, _mm_mul_ps(x, denominator),
                               _mm_set_ps1(310134825));

    denominator =
        _mm_fmsub_ps(x, _mm_mul_ps(x, denominator), coeff1);
#endif
#ifndef __FMA__
    numerator = _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(x, numerator)),
                           _mm_set_ps1(25740));
    numerator = _mm_sub_ps(_mm_mul_ps(x, _mm_mul_ps(x, numerator)),
                           _mm_set_ps1(2837835));
    numerator = _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(x, numerator)),
                           _mm_set_ps1(91891800));
    numerator =
        _mm_sub_ps(_mm_mul_ps(x, _mm_mul_ps(x, numerator)), coeff1);
    numerator = _mm_mul_ps(x, numerator);

    // imenovalec
    //   -654729075 + 310134825 a^2 - 18918900 a^4 + 315315 a^6 -
    //   1485 a^8 + a^10
    __m128 denominator = _mm_sub_ps(x2, _mm_set_ps1(-1485));
    denominator =
        _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(x, denominator)),
                   _mm_set_ps1(315315));
    denominator =
        _mm_sub_ps(_mm_mul_ps(x, _mm_mul_ps(x, denominator)),
                   _mm_set_ps1(18918900));
    denominator =
        _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(x, denominator)),
                   _mm_set_ps1(310134825));

    denominator = _mm_sub_ps(
        _mm_mul_ps(x, _mm_mul_ps(x, denominator)), coeff1);
#endif
    return _mm_div_ps(numerator, denominator);
}

/**
 *@brief This function calculates tangens of sse vector.  It's an
 *alternative to above function, but it's somehow (unexpectedly)
 *less accurate.
 *@details For detailed documentation on formula used check pdf
 *files discussing trigonometric functions. This is float version
 *and uses sse instructions.
 */
inline __m128 tan_ver3(const __m128 &x) {
    // version 4 S(4,a)
    // this function should calculate tan to 1e-8 precision
    // stevec : 34459425 * a - 4729725 * a ^ 3 + 135135 * a ^ 5 -
    // 990 a ^ 7 + a ^ 9;
    __m128 x2 = _mm_mul_ps(x, x);
    __m128 coeff1 = _mm_set_ps1(34459425);
    __m128 numerator = _mm_sub_ps(x2, _mm_set_ps1(-990));
#ifdef __FMA__
    numerator = _mm_fmadd_ps(x, _mm_mul_ps(x, numerator),
                             _mm_set_ps1(135135));
    numerator = _mm_fmsub_ps(x, _mm_mul_ps(x, numerator),
                             _mm_set_ps1(4729725));
    numerator = _mm_fmadd_ps(x, _mm_mul_ps(x, numerator), coeff1);
    numerator = _mm_mul_ps(x, numerator);

    // imenovalec: 34459425 - 16216200*a^2 + 945945 a^4 - 13860*a^6
    // + 45*a^8
    __m128 denominator = _mm_add_ps(x2, _mm_set_ps1(45));
    denominator = _mm_fmsub_ps(x, _mm_mul_ps(x, denominator),
                               _mm_set_ps1(13860));
    denominator = _mm_fmadd_ps(x, _mm_mul_ps(x, denominator),
                               _mm_set_ps1(945945));
    denominator = _mm_fmsub_ps(x, _mm_mul_ps(x, denominator),
                               _mm_set_ps1(16216200));
    denominator =
        _mm_fmadd_ps(x, _mm_mul_ps(x, denominator), coeff1);
#endif
#ifndef __FMA__
    numerator = _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(x, numerator)),
                           _mm_set_ps1(135135));
    numerator = _mm_sub_ps(_mm_mul_ps(x, _mm_mul_ps(x, numerator)),
                           _mm_set_ps1(4729725));
    numerator =
        _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(x, numerator)), coeff1);
    numerator = _mm_mul_ps(x, numerator);

    // imenovalec: 34459425 - 16216200*a^2 + 945945 a^4 - 13860*a^6
    // + 45*a^8
    __m128 denominator = _mm_add_ps(x2, _mm_set_ps1(45));
    denominator =
        _mm_sub_ps(_mm_mul_ps(x, _mm_mul_ps(x, denominator)),
                   _mm_set_ps1(13860));
    denominator =
        _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(x, denominator)),
                   _mm_set_ps1(945945));
    denominator =
        _mm_sub_ps(_mm_mul_ps(x, _mm_mul_ps(x, denominator)),
                   _mm_set_ps1(16216200));
    denominator = _mm_add_ps(
        _mm_mul_ps(x, _mm_mul_ps(x, denominator)), coeff1);
#endif
    return _mm_div_ps(numerator, denominator);
}


#ifdef __AVX2__
/**
 *@brief This function calculates tangens of sse vector.
 *@details For detailed documentation on formula used check pdf
 *files discussing trigonometric functions. This is double version
 *and uses avx2 instructions.
 *@param x vector of angles for which tangens is calculated
 */
inline __m256d tan(const __m256d &x) {
    // this function should calculate tan to 1e-8 precision
    // stevec : 34459425 * a - 4729725 * a ^ 3 + 135135 * a ^ 5 -
    // 990 a ^ 7
    // + a ^ 9;
    __m256d x2 = _mm256_mul_pd(x, x);
    __m256d x3 = _mm256_mul_pd(x2, x);
    __m256d x4 = _mm256_mul_pd(x2, x2);
    __m256d x5 = _mm256_mul_pd(x3, x2);
    __m256d x6 = _mm256_mul_pd(x3, x3);
    __m256d x7 = _mm256_mul_pd(x3, x4);
    __m256d x8 = _mm256_mul_pd(x4, x4);
    __m256d x9 = _mm256_mul_pd(x4, x5);
    __m256d coeff1 = _mm256_set1_pd(34459425);
    __m256d numerator = _mm256_fmsub_pd(
        x, coeff1,
        _mm256_fmsub_pd(
            x3, _mm256_set1_pd(4729725),
            _mm256_fmsub_pd(
                x5, _mm256_set1_pd(135135),
                _mm256_fmsub_pd(x7, _mm256_set1_pd(990), x9))));
    // imenovalec: 34459425 - 16216200*a^2 + 945945 a^4 - 13860*a^6
    // + 45*a^8
    __m256d denominator = _mm256_fmsub_pd(
        x8, _mm256_set1_pd(45),
        _mm256_fmsub_pd(
            x6, _mm256_set1_pd(13860),
            _mm256_fmsub_pd(
                x4, _mm256_set1_pd(945945),
                _mm256_fmsub_pd(x2, _mm256_set1_pd(16216200),
                                coeff1))));
    return _mm256_div_pd(numerator, denominator);
}
#endif

/**
 *@brief This function calculates arcus tangens of sse vector.
 *@details For detailed documentation on formula used check pdf
 *files discussing trigonometric functions. This is float version
 *and uses sse instructions.
 */
inline __m128 arctan(const __m128 &x0) {
    // this comparison sets 0xffff if true - which is not 1.0
    __m128 cmp1 = _mm_cmpgt_ps(_mm_setzero_ps(), x0);
    // now we compare to 1.0 to get desired 0.0 and 1.0 numbers
    cmp1 = _mm_and_ps(cmp1, _mm_set_ps1(-1.0));
    __m128 cmp1x2 = _mm_mul_ps(_mm_set_ps1(2.0), cmp1);
    // cmp1 =_mm_xor_ps(v, _mm_set1_ps(-0.0)); convert 1 to -1
    // now calculate absolute value
#ifdef __FMA__
    __m128 x = _mm_fmadd_ps(cmp1x2, x0, x0);
#endif
#ifndef __FMA__
    __m128 x = _mm_add_ps(_mm_mul_ps(cmp1x2, x0), x0);
#endif
    __m128 cmp2 = _mm_cmpgt_ps(x, _mm_set_ps1(1.0));
    // cmp2 = _mm_and_ps(cmp2, _mm_set_ps1(1.0));

    __m128 overx = _mm_div_ps(_mm_set_ps1(1.0), x);
    x = _mm_blendv_ps(x, overx, cmp2);

    // numerator coefficients a^0, a^1, a^2, a^3,....
    // {0, 2342475135, 0, 5941060125, 0, 5429886462, 0, 2146898754,
    // 0,
    //        341536195, 0, 14928225}
#ifdef __FMA__
    __m128 numerator =
        _mm_fmadd_ps(_mm_mul_ps(x, _mm_set_ps1(14928225)), x,
                     _mm_set_ps1(341536195));
    numerator = _mm_fmadd_ps(_mm_mul_ps(x, numerator), x,
                             _mm_set_ps1(2146898754));

    numerator = _mm_fmadd_ps(_mm_mul_ps(x, numerator), x,
                             _mm_set_ps1(5429886462));

    numerator = _mm_fmadd_ps(_mm_mul_ps(x, numerator), x,
                             _mm_set_ps1(5941060125));

    numerator = _mm_fmadd_ps(_mm_mul_ps(x, numerator), x,
                             _mm_set_ps1(2342475135));
    numerator = _mm_mul_ps(numerator, x);

    // {2342475135, 0, 6721885170, 0, 7202019825, 0, 3537834300, 0,
    // 780404625, 0, 62432370, 0, 800415}
    __m128 denominator =
        _mm_fmadd_ps(x, _mm_mul_ps(x, _mm_set_ps1(800415)),
                     _mm_set_ps1(62432370));
    denominator = _mm_fmadd_ps(x, _mm_mul_ps(denominator, x),
                               _mm_set_ps1(780404625));
    denominator = _mm_fmadd_ps(x, _mm_mul_ps(denominator, x),
                               _mm_set_ps1(3537834300));
    denominator = _mm_fmadd_ps(x, _mm_mul_ps(denominator, x),
                               _mm_set_ps1(7202019825));
    denominator = _mm_fmadd_ps(x, _mm_mul_ps(denominator, x),
                               _mm_set_ps1(6721885170));
    denominator = _mm_fmadd_ps(x, _mm_mul_ps(denominator, x),
                               _mm_set_ps1(2342475135));
#endif
#ifndef __FMA__
    __m128 numerator = _mm_add_ps(
        _mm_mul_ps(_mm_mul_ps(x, _mm_set_ps1(14928225)), x),
        _mm_set_ps1(341536195));
    numerator = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(x, numerator), x),
                           _mm_set_ps1(2146898754));

    numerator = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(x, numerator), x),
                           _mm_set_ps1(5429886462));

    numerator =
        _mm_fmadd_ps(_mm_mul_ps(_mm_mul_ps(x, numerator), x),
                     _mm_set_ps1(5941060125));

    numerator = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(x, numerator), x),
                           _mm_set_ps1(2342475135));
    numerator = _mm_mul_ps(numerator, x);

    // {2342475135, 0, 6721885170, 0, 7202019825, 0, 3537834300, 0,
    // 780404625, 0, 62432370, 0, 800415}
    __m128 denominator = _mm_add_ps(
        _mm_mul_ps(x, _mm_mul_ps(x, _mm_set_ps1(800415))),
        _mm_set_ps1(62432370));
    denominator =
        _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(denominator, x)),
                   _mm_set_ps1(780404625));
    denominator =
        _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(denominator, x)),
                   _mm_set_ps1(3537834300));
    denominator =
        _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(denominator, x)),
                   _mm_set_ps1(7202019825));
    denominator =
        _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(denominator, x)),
                   _mm_set_ps1(6721885170));
    denominator =
        _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(denominator, x)),
                   _mm_set_ps1(2342475135));
#endif
    __m128 result = _mm_div_ps(numerator, denominator);
    __m128 pi2 = _mm_set_ps1(M_PI / 2.0);
    __m128 x_pi = _mm_sub_ps(pi2, result);
    result = _mm_blendv_ps(result, x_pi, cmp2);
#ifdef __FMA__
    cmp1 = _mm_fmadd_ps(cmp1, _mm_set_ps1(2.0), _mm_set_ps1(1.0));
#endif
#ifndef __FMA__
    cmp1 = _mm_add_ps(_mm_mul_ps(cmp1, _mm_set_ps1(2.0)),
                      _mm_set_ps1(1.0));
#endif
    result = _mm_mul_ps(result, cmp1);
    return result;
}

#ifdef __AVX2__
/**
 *@brief This function calculates arcus tangens of avx vector.
 *@details For detailed documentation on formula used check pdf
 *files discussing trigonometric functions. This is double version
 *and uses avx2 instructions.
 */
inline __m256d arctan(const __m256d &x0) {
    // this comparison sets 0xffff if true - which is not 1.0
    __m256d cmp1 =
        _mm256_cmp_pd(_mm256_setzero_pd(), x0, _CMP_GT_OS);
    // now we compare to 1.0 to get desired 0.0 and 1.0 numbers
    cmp1 = _mm256_and_pd(cmp1, _mm256_set1_pd(-1.0));
    __m256d cmp1x2 = _mm256_mul_pd(_mm256_set1_pd(2.0), cmp1);
    // cmp1 =_mm_xor_ps(v, _mm_set1_ps(-0.0)); convert 1 to -1
    // now calculate absolute value
    __m256d x = _mm256_fmadd_pd(cmp1x2, x0, x0);

    __m256d cmp2 =
        _mm256_cmp_pd(x, _mm256_set1_pd(1.0), _CMP_GT_OS);
    // cmp2 = _mm_and_ps(cmp2, _mm_set_ps1(1.0));

    __m256d overx = _mm256_div_pd(_mm256_set1_pd(1.0), x);
    x = _mm256_blendv_pd(x, overx, cmp2);

    // numerator coefficients a^0, a^1, a^2, a^3,....
    // double precision
    //    {0, 579118415250375, 0, 2338535314915800, 0,
    //    3874828669311600, 0,
    //        3393172947724560, 0, 1684523959239550, 0,
    //        471862454520600, 0, 69652545704280, 0, 4581496496640,
    //        0, 87091688691}
    __m256d numerator = _mm256_fmadd_pd(
        _mm256_mul_pd(x, _mm256_set1_pd(87091688691)), x,
        _mm256_set1_pd(4581496496640));

    numerator = _mm256_fmadd_pd(_mm256_mul_pd(x, numerator), x,
                                _mm256_set1_pd(69652545704280));

    numerator = _mm256_fmadd_pd(_mm256_mul_pd(x, numerator), x,
                                _mm256_set1_pd(471862454520600));

    numerator = _mm256_fmadd_pd(_mm256_mul_pd(x, numerator), x,
                                _mm256_set1_pd(1684523959239550));

    numerator = _mm256_fmadd_pd(_mm256_mul_pd(x, numerator), x,
                                _mm256_set1_pd(3393172947724560));

    numerator = _mm256_fmadd_pd(_mm256_mul_pd(x, numerator), x,
                                _mm256_set1_pd(3874828669311600));

    numerator = _mm256_fmadd_pd(_mm256_mul_pd(x, numerator), x,
                                _mm256_set1_pd(2338535314915800));

    numerator = _mm256_fmadd_pd(_mm256_mul_pd(x, numerator), x,
                                _mm256_set1_pd(579118415250375));

    numerator = _mm256_mul_pd(numerator, x);

    // denominator
    // {579118415250375, 0, 2531574786665925, 0, 4602863248483500,
    // 0, 4503876942064500, 0, 2562550673933250, 0, 854183557977750,
    // 0, 159447597489180, 0, 14855366225700, 0, 530548793775, 0,
    // 3102624525}
    __m256d denominator = _mm256_fmadd_pd(
        x, _mm256_mul_pd(x, _mm256_set1_pd(3102624525)),
        _mm256_set1_pd(530548793775));

    denominator = _mm256_fmadd_pd(x, _mm256_mul_pd(denominator, x),
                                  _mm256_set1_pd(14855366225700));

    denominator = _mm256_fmadd_pd(x, _mm256_mul_pd(denominator, x),
                                  _mm256_set1_pd(159447597489180));

    denominator = _mm256_fmadd_pd(x, _mm256_mul_pd(denominator, x),
                                  _mm256_set1_pd(854183557977750));

    denominator = _mm256_fmadd_pd(x, _mm256_mul_pd(denominator, x),
                                  _mm256_set1_pd(2562550673933250));

    denominator = _mm256_fmadd_pd(x, _mm256_mul_pd(denominator, x),
                                  _mm256_set1_pd(4503876942064500));

    denominator = _mm256_fmadd_pd(x, _mm256_mul_pd(denominator, x),
                                  _mm256_set1_pd(4602863248483500));

    denominator = _mm256_fmadd_pd(x, _mm256_mul_pd(denominator, x),
                                  _mm256_set1_pd(2531574786665925));

    denominator = _mm256_fmadd_pd(x, _mm256_mul_pd(denominator, x),
                                  _mm256_set1_pd(579118415250375));

    __m256d result = _mm256_div_pd(numerator, denominator);
    __m256d pi2 = _mm256_set1_pd(M_PI / 2.0);
    __m256d x_pi = _mm256_sub_pd(pi2, result);
    result = _mm256_blendv_pd(result, x_pi, cmp2);
    cmp1 = _mm256_fmadd_pd(cmp1, _mm256_set1_pd(2.0),
                           _mm256_set1_pd(1.0));
    result = _mm256_mul_pd(result, cmp1);
    return result;
}
#endif

/**
 *
 *
 */
inline __attribute__((always_inline)) __m128
arccos_ver1(const __m128 &x) {

    /*Sqrt[2] Sqrt[t] + t^(3/2)/(6 Sqrt[2]) + (3 t^(5/2))/(80
       Sqrt[2]) + ( 5 t^(7/2))/(448 Sqrt[2]) + (35 t^(9/2))/(9216
       Sqrt[2])
    */

    __m128 mask = _mm_cmpgt_ps(x, _mm_setzero_ps());
    static const __m128 SIGN_BIT_MASK =
        _mm_castsi128_ps(_mm_set1_epi32(0x80000000));

    // Get the absolute value of the vector;
    __m128 x_abs = _mm_andnot_ps(SIGN_BIT_MASK, x);

    __m128 sqrt2 = _mm_set1_ps(1.4142135623730950488);
    __m128 coeff32 = _mm_set1_ps(0.11785113019775792073);
    __m128 coeff52 = _mm_set1_ps(0.026516504294495532165);
    __m128 coeff72 = _mm_set1_ps(0.0078918167543141464777);
    __m128 coeff92 = _mm_set1_ps(0.0026854098677874526209);
    __m128 coeff112 = _mm_set1_ps(0.00098871908768538028314);
    __m128 coeff132 = _mm_set1_ps(0.00038344554362157376365);
    __m128 coeff152 = _mm_set1_ps(0.00015429118302868087157);
    __m128 t = _mm_sub_ps(_mm_set1_ps(1.0), x_abs);
    __m128 sqrtt = _mm_sqrt_ps(t);
    __m128 t32 = _mm_mul_ps(t, sqrtt);
    __m128 t52 = _mm_mul_ps(t, t32);
    __m128 t72 = _mm_mul_ps(t52, t);
    __m128 t92 = _mm_mul_ps(t72, t);
    __m128 t112 = _mm_mul_ps(t92, t);
    __m128 t132 = _mm_mul_ps(t112, t);
    __m128 t152 = _mm_mul_ps(t132, t);
    __m128 result = _mm_fmadd_ps(
        sqrt2, sqrtt,
        _mm_fmadd_ps(
            t32, coeff32,
            _mm_fmadd_ps(
                t52, coeff52,
                _mm_fmadd_ps(
                    t72, coeff72,
                    _mm_fmadd_ps(
                        t92, coeff92,
                        _mm_fmadd_ps(
                            t112, coeff112,
                            _mm_fmadd_ps(
                                t132, coeff132,
                                _mm_mul_ps(coeff152, t152))))))));

    __m128 neg_result = _mm_add_ps(
        _mm_set1_ps(M_PI), _mm_xor_ps(result, _mm_set1_ps(-0.0)));

    return _mm_blendv_ps(neg_result, result, mask);
};

inline __attribute__((always_inline)) __m128
arccos_ver2(const __m128 &x) {

    /*
      1.4142135623730950488 Sqrt[t] + 0.11785113019775792073 t^(3/2)
      + 0.026516504294495532165 t^(5/2) + 0.0078918167543141464777
      t^(7/2) + 0.0026854098677874526209 t^(9/2) +
      0.00098871908768538028314 t^(11/2) +
      0.00038344554362157376365 t^(13/2)
     */

    __m128 mask = _mm_cmpgt_ps(x, _mm_setzero_ps());
    static const __m128 SIGN_BIT_MASK =
        _mm_castsi128_ps(_mm_set1_epi32(0x80000000));

    // Get the absolute value of the vector;
    __m128 x_abs = _mm_andnot_ps(SIGN_BIT_MASK, x);

    __m128 coeff1 = _mm_set1_ps(1.4142135623730950488);
    __m128 coeff3 = _mm_set1_ps(0.11785113019775792073);
    __m128 coeff5 = _mm_set1_ps(0.026516504294495532165);
    __m128 coeff7 = _mm_set1_ps(0.0078918167543141464777);
    __m128 coeff9 = _mm_set1_ps(0.0026854098677874526209);
    __m128 coeff11 = _mm_set1_ps(0.00098871908768538028314);
    __m128 coeff13 = _mm_set1_ps(0.00038344554362157376365);
    __m128 coeff15 = _mm_set1_ps(0.00015429118302868087157);
    __m128 t = _mm_sub_ps(_mm_set1_ps(1.0), x_abs);
    __m128 sqrtt = _mm_sqrt_ps(t);
    /*    __m128 t32 = _mm_mul_ps(t, sqrtt);
    __m128 t52 = _mm_mul_ps(t, t32);
    __m128 t72 = _mm_mul_ps(t52, t);
    __m128 t92 = _mm_mul_ps(t72, t);
    __m128 t112 = _mm_mul_ps(t92, t);
    __m128 t132 = _mm_mul_ps(t112, t);
    */
    __m128 result = _mm_mul_ps(
        _mm_fmadd_ps(
            _mm_fmadd_ps(
                _mm_fmadd_ps(
                    _mm_fmadd_ps(
                        _mm_fmadd_ps(
                            _mm_fmadd_ps(
                                _mm_fmadd_ps(coeff15, t, coeff13),
                                t, coeff11),
                            t, coeff9),
                        t, coeff7),
                    t, coeff5),
                t, coeff3),
            t, coeff1),
        sqrtt);

    __m128 neg_result = _mm_add_ps(
        _mm_set1_ps(M_PI), _mm_xor_ps(result, _mm_set1_ps(-0.0)));

    return _mm_blendv_ps(neg_result, result, mask);
};

inline __attribute__((always_inline)) __m128
arccos_ver3(const __m128 &t) {

    /*1.5707963267948966192-t-0.16666666666666666667
     * t^3-0.075000000000000000000 t^5-0.044642857142857142857
     * t^7-0.030381944444444444444 t^9+O[t]^11
     */

    __m128 coeff0 = _mm_set1_ps(1.5707963267948966192);

    __m128 coeff3 = _mm_set1_ps(-0.16666666666666666667);

    __m128 coeff5 = _mm_set1_ps(-0.0750);
    __m128 coeff7 = _mm_set1_ps(-0.044642857142857142857);
    __m128 coeff9 = _mm_set1_ps(-0.030381944444444444444);
    __m128 coeff11 = _mm_set1_ps(-0.022372159090909090909);

    __m128 t2 = _mm_mul_ps(t, t);
    /*    __m128 t3 = _mm_mul_ps(t2, t);
    __m128 t5 = _mm_mul_ps(t3, t2);
    __m128 t7 = _mm_mul_ps(t5, t2);
    __m128 t9 = _mm_mul_ps(t7, t2);
    */
    __m128 result = _mm_fmadd_ps(
        _mm_fmadd_ps(

            _mm_fmadd_ps(
                _mm_fmadd_ps(
                    _mm_fmadd_ps(_mm_fmadd_ps(coeff11, t2, coeff9),
                                 t2, coeff7),
                    t2, coeff5),
                t2, coeff3),
            t2, _mm_set1_ps(-1.0)),
        t, coeff0);

    return result;
};

inline __attribute__((always_inline)) __m128
arccos_ver4(const __m128 &t) {

    __m128 ones = _mm_set1_ps(1.0);
    __m128 oversqrt2 = _mm_set1_ps(0.70710678118654752440);
    __m128 cmp1 = _mm_cmplt_ps(t, oversqrt2);
    __m128 sqrt = _mm_sub_ps(ones, _mm_mul_ps(t, t));

    __m128 coeff0 = _mm_set1_ps(1.5707963267948966192);

    __m128 coeff3 = _mm_set1_ps(-0.16666666666666666667);

    __m128 coeff5 = _mm_set1_ps(-0.0750);
    __m128 coeff7 = _mm_set1_ps(-0.044642857142857142857);
    __m128 coeff9 = _mm_set1_ps(-0.030381944444444444444);
    __m128 coeff11 = _mm_set1_ps(-0.022372159090909090909);

    __m128 t2 = _mm_mul_ps(t, t);
    /*
    __m128 t3 = _mm_mul_ps(t2, t);
    __m128 t5 = _mm_mul_ps(t3, t2);
    __m128 t7 = _mm_mul_ps(t5, t2);
    __m128 t9 = _mm_mul_ps(t7, t2);
    */
    __m128 result = _mm_fmadd_ps(
        _mm_fmadd_ps(

            _mm_fmadd_ps(
                _mm_fmadd_ps(
                    _mm_fmadd_ps(_mm_fmadd_ps(coeff11, t2, coeff9),
                                 t2, coeff7),
                    t2, coeff5),
                t2, coeff3),
            t2, _mm_set1_ps(-1.0)),
        t, coeff0);

    return result;
};

/** @brief The function calculates arcsin of input vector. Input
 * vector should contain elements in range [-1.0, 1.0].
 * @details This is sse float implementation.
 * @param t input sse vector of values for which arcsin is
 * calculated
 */
inline __attribute__((always_inline)) __m128
arcsin(const __m128 &t) {

    __m128 twos = _mm_set1_ps(2.0);
    __m128 ones = _mm_set1_ps(1.0);
    __m128 t2 = _mm_mul_ps(t, t);

    __m128 sqrt = _mm_set1_ps(0.70710678118654752440);
    __m128 mask1 = _mm_cmpgt_ps(t, sqrt);
    __m128 mask2 = _mm_cmplt_ps(t, _mm_xor_ps(sqrt, SIGNMASK));
    __m128 combined_mask = _mm_or_ps(mask1, mask2);
    __m128 sqrtt = _mm_sqrt_ps(_mm_sub_ps(ones, t2));

    __m128 t_mod = _mm_blendv_ps(t, sqrtt, combined_mask);
    t2 = _mm_mul_ps(t_mod, t_mod);

    __m128 dn = ones;
    __m128 cn = twos;
    __m128 sum = t_mod;
    __m128 next_t = _mm_mul_ps(t2, t_mod);
    __m128 counter2 = twos;
    __m128 i_vec = _mm_set1_ps(3.0);

    // determine the number of iterations needed
    float max_element =
        _mm_horizontal_max_ps(_mm_andnot_ps(SIGNMASK, t_mod));

    int max_iter =
        std::ceil(-5 / std::log10(std::fabs(max_element)));

    for (int i = 3; i <= max_iter; i += 2) {

        __m128 factor = _mm_div_ps(dn, _mm_mul_ps(cn, i_vec));
#ifdef __FMA__
        sum = _mm_fmadd_ps(factor, next_t, sum);
#endif
#ifndef __FMA__
        sum = _mm_add_ps(_mm_mul_ps(factor, next_t, sum));
#endif
        next_t = _mm_mul_ps(next_t, t2);
        dn = _mm_mul_ps(dn, i_vec);

        counter2 = _mm_add_ps(counter2, twos);
        cn = _mm_mul_ps(cn, counter2);

        i_vec = _mm_add_ps(i_vec, twos);
    }
    static const __m128 pi2 =
        _mm_set1_ps(1.5707963267948966192); // pi/2
    __m128 sol1 = _mm_sub_ps(pi2, sum);
    __m128 sol2 = _mm_add_ps(_mm_xor_ps(pi2, SIGNMASK), sum);

    return _mm_blendv_ps(_mm_blendv_ps(sum, sol1, mask1), sol2,
                         mask2);
}

/** @brief The function calculates arcsin of input vector. Input
 * vector should contain elements in range [-1.0, 1.0].
 * @details This is sse float implementation.
 * @param t input sse vector of values for which arcsin is
 * calculated
 */
inline __attribute__((always_inline)) __m128
arccos(const __m128 &t) {
    return _mm_sub_ps(_mm_set1_ps(1.5707963267948966192),
                      arcsin(t));
}

#ifdef __AVX2__
/** @brief The function calculates arcsin of input vector. Input
 * vector should contain elements in range [-1.0, 1.0].
 * @details This is avx double implementation.
 * @param t input avx vector of values for which double precision
 * arcsin is calculated.
 */
inline __attribute__((always_inline)) __m256d
arcsin(const __m256d &t) {

    __m256d twos = _mm256_set1_pd(2.0);
    __m256d ones = _mm256_set1_pd(1.0);
    __m256d t2 = _mm256_mul_pd(t, t);

    __m256d sqrt = _mm256_set1_pd(0.70710678118654752440);
    __m256d mask1 = _mm256_cmp_pd(t, sqrt, _CMP_GT_OS);
    __m256d mask2 = _mm256_cmp_pd(
        t, _mm256_xor_pd(sqrt, SIGNMASK256d), _CMP_LT_OS);

    __m256d combined_mask = _mm256_or_pd(mask1, mask2);
    __m256d sqrtt = _mm256_sqrt_pd(_mm256_sub_pd(ones, t2));

    __m256d t_mod = _mm256_blendv_pd(t, sqrtt, combined_mask);

    t2 = _mm256_mul_pd(t_mod, t_mod);

    __m256d dn = ones;
    __m256d cn = twos;
    __m256d sum = t_mod;
    __m256d next_t = _mm256_mul_pd(t2, t_mod);
    __m256d counter2 = twos;
    __m256d i_vec = _mm256_set1_pd(3.0);

    // determine the number of iterations needed
    double max_element = _mm256_horizontal_max_pd(
        _mm256_andnot_pd(SIGNMASK256d, t_mod));

    int max_iter =
        std::ceil(-12 / std::log10(std::fabs(max_element)));

    for (int i = 3; i <= max_iter; i += 2) {

        __m256d factor =
            _mm256_div_pd(dn, _mm256_mul_pd(cn, i_vec));

        sum = _mm256_fmadd_pd(factor, next_t, sum);
        next_t = _mm256_mul_pd(next_t, t2);
        dn = _mm256_mul_pd(dn, i_vec);

        counter2 = _mm256_add_pd(counter2, twos);
        cn = _mm256_mul_pd(cn, counter2);

        i_vec = _mm256_add_pd(i_vec, twos);
    }
    static const __m256d pi2 =
        _mm256_set1_pd(1.5707963267948966192); // pi/2
    __m256d sol1 = _mm256_sub_pd(pi2, sum);
    __m256d sol2 =
        _mm256_add_pd(_mm256_xor_pd(pi2, SIGNMASK256d), sum);

    return _mm256_blendv_pd(_mm256_blendv_pd(sum, sol1, mask1),
                            sol2, mask2);
}
#endif

#ifdef __AVX2__
/** @brief The function calculates arcsin of input vector. Input
 * vector should contain elements in range [-1.0, 1.0].
 * @details This is avx double implementation.
 * @param t input avx vector of values for which double precision
 * arcsin is calculated.
 */
inline __attribute__((always_inline)) __m256d
arccos(const __m256d &t) {
    return _mm256_sub_pd(_mm256_set1_pd(1.5707963267948966192),
                         arcsin(t));
};
#endif
