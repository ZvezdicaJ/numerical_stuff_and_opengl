template <class T> inline void hash_combine(std::size_t &seed, const T &v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std {
template <typename S, typename T> struct hash<pair<S, T>> {
    inline size_t operator()(const pair<S, T> &v) const {
        size_t seed = 0;
        ::hash_combine(seed, v.first);
        ::hash_combine(seed, v.second);
        return seed;
    }
};
} // namespace std

/*avx2 compare codes*/
/* Ordered vs Unordered has to do with whether the comparison is true if one of
 * the operands contains a NaN. Signaling (S) vs non-signaling (Q for quiet?)
 * will determine whether an exception is raised if an operand contains a NaN.
 */
#ifndef __INTEL_COMPILER
#define _CMP_EQ_OQ 0x00    /* Equal (ordered, non-signaling)  */
#define _CMP_LT_OS 0x01    /* Less-than (ordered, signaling)  */
#define _CMP_LE_OS 0x02    /* Less-than-or-equal (ordered, signaling)  */
#define _CMP_UNORD_Q 0x03  /* Unordered (non-signaling)  */
#define _CMP_NEQ_UQ 0x04   /* Not-equal (unordered, non-signaling)  */
#define _CMP_NLT_US 0x05   /* Not-less-than (unordered, signaling)  */
#define _CMP_NLE_US 0x06   /* Not-less-than-or-equal (unordered, signaling)  */
#define _CMP_ORD_Q 0x07    /* Ordered (nonsignaling)   */
#define _CMP_EQ_UQ 0x08    /* Equal (unordered, non-signaling)  */
#define _CMP_NGE_US 0x09   /* Not-greater-than-or-equal (unord, signaling)  */
#define _CMP_NGT_US 0x0a   /* Not-greater-than (unordered, signaling)  */
#define _CMP_FALSE_OQ 0x0b /* False (ordered, non-signaling)  */
#define _CMP_NEQ_OQ 0x0c   /* Not-equal (ordered, non-signaling)  */
#define _CMP_GE_OS 0x0d    /* Greater-than-or-equal (ordered, signaling)  */
#define _CMP_GT_OS 0x0e    /* Greater-than (ordered, signaling)  */
#define _CMP_TRUE_UQ 0x0f  /* True (unordered, non-signaling)  */
#define _CMP_EQ_OS 0x10    /* Equal (ordered, signaling)  */
#define _CMP_LT_OQ 0x11    /* Less-than (ordered, non-signaling)  */
#define _CMP_LE_OQ 0x12    /* Less-than-or-equal (ordered, non-signaling)  */
#define _CMP_UNORD_S 0x13  /* Unordered (signaling)  */
#define _CMP_NEQ_US 0x14   /* Not-equal (unordered, signaling)  */
#define _CMP_NLT_UQ 0x15   /* Not-less-than (unordered, non-signaling)  */
#define _CMP_NLE_UQ 0x16   /* Not-less-than-or-equal (unord, non-signaling)  */
#define _CMP_ORD_S 0x17    /* Ordered (signaling)  */
#define _CMP_EQ_US 0x18    /* Equal (unordered, signaling)  */
#define _CMP_NGE_UQ 0x19   /* Not-greater-than-or-equal (unord, non-sign)  */
#define _CMP_NGT_UQ 0x1a   /* Not-greater-than (unordered, non-signaling)  */
#define _CMP_FALSE_OS 0x1b /* False (ordered, signaling)  */
#define _CMP_NEQ_OS 0x1c   /* Not-equal (ordered, signaling)  */
#define _CMP_GE_OQ 0x1d    /* Greater-than-or-equal (ordered, non-signaling)  */
#define _CMP_GT_OQ 0x1e    /* Greater-than (ordered, non-signaling)  */
#define _CMP_TRUE_US 0x1f  /* True (unordered, signaling)  */
#endif

#ifdef __SSE__
/**
 *Function loads 3 floats to a sse vector, the last value is set to 0.
 * @param value float pointer pointing to first element
 * @return returns __m128 vector, last element is 0.
 */
inline __m128 load_vertex(float *value) {
    // load the x and y element of the float3 vector using a 64 bit load
    // and set the upper 64 bits to zero (00YX)
    __m128 xy = _mm_loadl_pi(_mm_setzero_ps(), (__m64 *)value);
    // now load the z element using a 32 bit load (000Z)
    __m128 z = _mm_load_ss(value + 2);
    // finally, combine both by moving the z component to the high part
    // of the register, while keeping x and y components in the low part
    return _mm_movelh_ps(xy, z); // (0ZYX)
}

/**
 *Function loads 3 floats to a sse vector, the last value is set to 0.
 * @param value float pointer pointing to first element
 * @return returns __m128 vector, last element is 0.
 */
inline __m128 load_vertex2(float *value) {
    // load x, y with a 64 bit integer load (00YX)
    __m128i xy = _mm_loadl_epi64((__m128i *)value);
    // now load the z element using a 32 bit float load (000Z)
    __m128 z = _mm_load_ss(value + 2);
    // we now need to cast the __m128i register into a __m18 one (0ZYX)
    return _mm_movelh_ps(_mm_castsi128_ps(xy), z);
}
#endif

/**
 * @brief Old version: Calculates cross product of two __m128 vectors (floats):
 * a x b.
 * @param a __m128 vector
 * @param b __m128 vector
 */
inline __m128 cross_product_old(__m128 a, __m128 b) {
#ifdef __FMA__
    __m128 result =
        _mm_fmsub_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1)),
                     _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2)),
                     _mm_mul_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 1, 0, 2)),
                                _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1))));
#endif

#ifndef __FMA__
    __m128 result =
        _mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1)),
                              _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2))),
                   _mm_mul_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 1, 0, 2)),
                              _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1))));
#endif
    return result;
}

/**
 * @brief Calculates cross product of two __m128 vectors (floats): a x b.
 * @param a __m128 vector
 * @param b __m128 vector
 */
inline __m128 cross_product(__m128 a, __m128 b) {
#ifndef __FMA__
    __m128 result = _mm_sub_ps(
        _mm_mul_ps(b, _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1))),
        _mm_mul_ps(a, _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1))));
#endif

#ifdef __FMA__
    __m128 result = _mm_fmsub_ps(
        b, _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1)),
        _mm_mul_ps(a, _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1))));
#endif

    return _mm_shuffle_ps(result, result, _MM_SHUFFLE(3, 0, 2, 1));
}

#ifdef __AVX2__
/**
 * @brief Calculates cross product of two __m256d vectors (doubles): a x b.
 * @param a __m256d vector
 * @param b __m256d vector
 */
inline __m256d cross_product(__m256d a, __m256d b) {
    __m256d c = _mm256_permute4x64_pd(
        _mm256_fmsub_pd(a, _mm256_permute4x64_pd(b, _MM_SHUFFLE(3, 0, 2, 1)),
                        _mm256_mul_pd(b, _mm256_permute4x64_pd(
                                             a, _MM_SHUFFLE(3, 0, 2, 1)))),
        _MM_SHUFFLE(3, 0, 2, 1));
    return c;
}
#endif

#ifdef __SSE__
/**
 * @brief Calculates scalar product of two __m128 vectors (floats): a x b.
 * @param a __m128 vector
 * @param b __m128 vector
 */
inline float CalcDotProduct(__m128 x, __m128 y) {
    __m128 mulRes, shufReg, sumsReg;
    mulRes = _mm_mul_ps(x, y);

    // Calculates the sum of SSE Register -
    // https://stackoverflow.com/a/35270026/195787
    shufReg = _mm_movehdup_ps(mulRes); // Broadcast elements 3,1 to 2,0
    sumsReg = _mm_add_ps(mulRes, shufReg);
    shufReg = _mm_movehl_ps(shufReg, sumsReg); // High Half -> Low Half
    sumsReg = _mm_add_ss(sumsReg, shufReg);
    return _mm_cvtss_f32(
        sumsReg); // Result in the lower part of the SSE Register
}
#endif

/**
 * @brief Calculates cross product of two float vectors: a x b.
 * @param a  vector
 * @param b  vector
 */
inline float CalcDotProduct(float vec1[3], float vec2[3]) {
    return vec1[0] * vec2[0] + vec1[1] * vec2[1] + vec1[2] * vec2[2];
}

#ifdef __AVX2__
/**
 * @brief Calculates cross product of two __m128 vectors (floats): a x b.
 * @param a __m256d vector
 * @param b __m256d vector
 */
inline double CalcDotProduct(__m256d x, __m256d y) {
    // double *tmp_d = (double *)(&x);
    // std::cout << "CalcDotProduct function: " << tmp_d[0] << " " << tmp_d[1]
    //          << " " << tmp_d[2] << std::endl;
    __m256d tmp = _mm256_mul_pd(x, y);
    double *tmp_d = (double *)(&tmp);
    // std::cout << "CalcDotProduct function: " << tmp_d[0] << " " << tmp_d[1]
    //          << " " << tmp_d[2] << std::endl;
    return tmp_d[0] + tmp_d[1] + tmp_d[2];
}
#endif

// this is slow; do not use
/**
 * @brief Calculates factorial of integer and returns vector containing the
 * result - this is slow and useless - do not use.
 * @param n calculates n!
 */
inline __m128i sse_factorial(int n) {
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
inline int scalar_factorial(int n) {
    int prod = 1;
    for (int i = 2; i <= n; i++) {
        prod *= i;
    }
    return prod;
}

#ifdef __SSE__
/**
 * @brief Calculates Chebyshev polynomials.
 * @param n number of Chebyshev polynomial to calculate
 * @param x_vec vector of x values for which the value of polynomial is
 * calculated
 */
inline __m128 chebyshev(int n, __m128 x_vec) {
    // T0
    __m128 T0 = _mm_set_ps1(1.0);
    if (n == 0)
        return T0;
    if (n == 1)
        return x_vec;

        // T2
#ifdef __FMA__
    __m128 T2 = _mm_fmsub_ps(_mm_mul_ps(_mm_set_ps1(2.0), x_vec), x_vec, T0);
#endif
#ifndef __FMA__
    __m128 T2 =
        _mm_sub_ps(_mm_mul_ps(_mm_mul_ps(_mm_set_ps1(2.0), x_vec), x_vec), T0);
#endif
    if (n == 2)
        return T2;

        // T3
#ifdef __FMA__
    __m128 T3 = _mm_fmsub_ps(_mm_mul_ps(_mm_set_ps1(2.0), T2), x_vec, x_vec);
#endif
#ifndef __FMA__
    __m128 T3 =
        _mm_sub_ps(_mm_mul_ps(_mm_mul_ps(_mm_set_ps1(2.0), T2), x_vec), x_vec);
#endif

    if (n == 3)
        return T3;
    __m128 Tn = T3;
    __m128 Tnm1 = T2;
    __m128 Tnp1;
    for (int i = 4; i <= n; i++) {
#ifdef __FMA__
        Tnp1 = _mm_fmsub_ps(_mm_mul_ps(_mm_set_ps1(2.0), Tn), x_vec, Tnm1);
#endif
#ifndef __FMA__
        Tnp1 = _mm_sub_ps(_mm_mul_ps(_mm_mul_ps(_mm_set_ps1(2.0), Tn), x_vec),
                          Tnm1);
#endif
        Tnm1 = Tn;
        Tn = Tnp1;
    }
    return Tnp1;
}
#endif

#ifdef __AVX2__
/**
 * @brief Calculates Chebyshev polynomials.
 * @param n number of Chebyshev polynomial to calculate
 * @param x_vec vector of x values for which the value of polynomial is
 * calculated
 */
inline __m256d chebyshev(int n, __m256d x_vec) {
    __m256d T0 = _mm256_set1_pd(1.0);
    if (n == 0)
        return _mm256_set1_pd(1.0);
    if (n == 1)
        return x_vec;

    // T2
    __m256d T2 =
        _mm256_fmsub_pd(_mm256_mul_pd(_mm256_set1_pd(2.0), x_vec), x_vec, T0);
    if (n == 2)
        return T2;

    // T3
    __m256d T3 =
        _mm256_fmsub_pd(_mm256_mul_pd(_mm256_set1_pd(2.0), T2), x_vec, x_vec);

    if (n == 3)
        return T3;
    __m256d Tn = T3;
    __m256d Tnm1 = T2;
    __m256d Tnp1;
    for (int i = 4; i <= n; i++) {
        Tnp1 = _mm256_fmsub_pd(_mm256_mul_pd(_mm256_set1_pd(2.0), Tn), x_vec,
                               Tnm1);
        Tnm1 = Tn;
        Tn = Tnp1;
    }
    return Tnp1;
}
#endif

#ifdef __SSE__
/**
 * @brief Given previous two chebyshev polynomials and vector of x values, the
 * function calculates next chebyshev polynomial.
 * @param n -th Chebyshev polynomial evaluated in points of x vector
 * @param n-1 -th Chebyshev polynomial evaluated in points of x vector
 * @param x_vec vector of x values in which the polynomial is calculated
 */
inline __m128 chebyshev_next(__m128 &cn, __m128 &cn_1, __m128 &x_vec) {
#ifdef __FMA__
    return _mm_fmsub_ps(_mm_mul_ps(_mm_set_ps1(2.0), cn), x_vec, cn_1);
#endif
#ifndef __FMA__
    return _mm_sub_ps(_mm_mul_ps(_mm_mul_ps(_mm_set_ps1(2.0), cn), x_vec),
                      cn_1);
#endif
}
#endif

#ifdef __AVX2__
/**
 * @brief Given previous two chebyshev polynomials and vector of x values, the
 * function calculates next chebyshev polynomial.
 * @param n -th Chebyshev polynomial evaluated in points of x vector
 * @param n-1 -th Chebyshev polynomial evaluated in points of x vector
 * @param x_vec vector of x values in which the polynomial is calculated
 */
inline __m256d chebyshev_next(__m256d &cn, __m256d &cn_1, __m256d &x_vec) {
    return _mm256_fmsub_pd(_mm256_mul_pd(_mm256_set1_pd(2.0), cn), x_vec, cn_1);
}
#endif

#ifdef __SSE__
/**
 * @brief Given previous two chebyshev polynomials and vector of x values, the
 * function calculates next chebyshev polynomial.
 * @param n -th Chebyshev polynomial evaluated in points of x vector
 * @param n-1 -th Chebyshev polynomial evaluated in points of x vector
 * @param x_vec vector of x values in which the polynomial is calculated
 */
inline __m128 cos(__m128 x_vec_) {
    float coeff[] = {-0.3042421776440938642020349128177049239697,
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

    __m128 sum = _mm_set_ps1(-0.3042421776440938642020349128177049239697);
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
#endif

#ifdef __AVX2__
/**
 * @brief calculates double precision cos.
 * @param x_vec vector of x values for which the cos is calculated
 */
inline __m256d cos(__m256d x_vec_) {
    float coeff[] = {-0.3042421776440938642020349128177049239697,
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

    __m256d sum = _mm256_set1_pd(-0.3042421776440938642020349128177049239697);
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

#ifdef __SSE__
/**
 * @brief calculates single precision sin.
 * @param x_vec vector of x values for which the sin is calculated
 * calculated
 */
inline __m128 sin(__m128 x_vec_) {
    float coeff[] = {0.56923068635950551469,    -0.66691667240597907078,
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
#endif

#ifdef __AVX2__
/**
 * @brief calculates double precision sin.
 * @param x_vec vector of x values for which the sin is calculated
 */
inline __m256d sin(__m256d x_vec_) {
    float coeff[] = {0.56923068635950551469,    -0.66691667240597907078,
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

#ifdef __SSE__
/**
 * @brief given two consequtive Legendre polynomials, this code calculates next
 * Legendre polynomial
 * @param x_vec vector of x values for which the next polynomial is calculated
 * calculated
 */
inline __m128 legendre_next(__m128 Pn, __m128 Pnm1, __m128 x_vec, int n) {
    __m128 n_vec = _mm_set_ps1(n);
    __m128 np1_vec = _mm_set_ps1(n + 1);
#ifdef __FMA__
    __m128 coeff1 = _mm_div_ps(
        _mm_mul_ps(_mm_fmadd_ps(_mm_set_ps1(2.0), n_vec, _mm_set_ps1(1.0)),
                   x_vec),
        np1_vec);
    __m128 result =
        _mm_fmsub_ps(coeff1, Pn, _mm_mul_ps(_mm_div_ps(n_vec, np1_vec), Pnm1));
#endif

#ifndef __FMA__
    __m128 coeff1 =
        _mm_div_ps(_mm_mul_ps(_mm_add_ps(_mm_mul_ps(_mm_set_ps1(2.0), n_vec),
                                         _mm_set_ps1(1.0)),
                              x_vec),
                   np1_vec);
    __m128 result = _mm_sub_ps(_mm_mul_ps(coeff1, Pn),
                               _mm_mul_ps(_mm_div_ps(n_vec, np1_vec), Pnm1));
#endif
    return result;
}
#endif

#ifdef __SSE__
/**
 *@brief This function calculates tangens of sse vector.
 *@details For detailed documentation on formula used check pdf files discussing
 *trigonometric functions. This is float version and uses sse instructions.
 */
inline __m128 tan(__m128 x) {

    // this function should calculate tan to 1e-8 precision
    // stevec : 34459425 * a - 4729725 * a ^ 3 + 135135 * a ^ 5 - 990 a ^ 7 + a
    // ^ 9;
    // this version seems to work best although it should not
    // x9 calculation should not work well for small x
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
        _mm_fmsub_ps(x3, _mm_set_ps1(4729725),
                     _mm_fmsub_ps(x5, _mm_set_ps1(135135),
                                  _mm_fmsub_ps(x7, _mm_set_ps1(990), x9))));
    // imenovalec: 34459425 - 16216200*a^2 + 945945 a^4 - 13860*a^6 + 45*a^8
    __m128 denominator = _mm_fmsub_ps(
        x8, _mm_set_ps1(45),
        _mm_fmsub_ps(
            x6, _mm_set_ps1(13860),
            _mm_fmsub_ps(x4, _mm_set_ps1(945945),
                         _mm_fmsub_ps(x2, _mm_set_ps1(16216200), coeff1))));
#endif
#ifndef __FMA__
    __m128 numerator = _mm_sub_ps(
        _mm_mul_ps(x, coeff1),
        _mm_sub_ps(
            _mm_mul_ps(x3, _mm_set_ps1(4729725)),
            _mm_sub_ps(_mm_mul_ps(x5, _mm_set_ps1(135135)),
                       _mm_sub_ps(_mm_mul_ps(x7, _mm_set_ps1(990)), x9))));
    // imenovalec: 34459425 - 16216200*a^2 + 945945 a^4 - 13860*a^6 + 45*a^8
    __m128 denominator = _mm_sub_ps(
        _mm_mul_ps(x8, _mm_set_ps1(45)),
        _mm_sub_ps(_mm_mul_ps(x6, _mm_set_ps1(13860)),
                   _mm_sub_ps(_mm_mul_ps(x4, _mm_set_ps1(945945)),
                              _mm_sub_ps(_mm_mul_ps(x2, _mm_set_ps1(16216200)),
                                         coeff1))));
#endif
    return _mm_div_ps(numerator, denominator);
}

/**
 *@brief This function calculates tangens of sse vector.
 *@details For detailed documentation on formula used check pdf files discussing
 *trigonometric functions. This is float version and uses sse instructions.
 */
inline __m128 tan_ver2(__m128 x) {
    __m128 x2 = _mm_mul_ps(x, x);
    // stevec    -654729075 a + 91891800 a^3 - 2837835 a^5 + 25740 a^7 - 55 a^9
    __m128 numerator = _mm_sub_ps(x2, _mm_set_ps1(55));
    __m128 coeff1 = _mm_set_ps1(654729075);
#ifdef __FMA__
    numerator = _mm_fmadd_ps(x, _mm_mul_ps(x, numerator), _mm_set_ps1(25740));
    numerator = _mm_fmsub_ps(x, _mm_mul_ps(x, numerator), _mm_set_ps1(2837835));
    numerator =
        _mm_fmadd_ps(x, _mm_mul_ps(x, numerator), _mm_set_ps1(91891800));
    numerator = _mm_fmsub_ps(x, _mm_mul_ps(x, numerator), coeff1);
    numerator = _mm_mul_ps(x, numerator);

    // imenovalec
    //   -654729075 + 310134825 a^2 - 18918900 a^4 + 315315 a^6 -
    //   1485 a^8 + a^10
    __m128 denominator = _mm_sub_ps(x2, _mm_set_ps1(-1485));
    denominator =
        _mm_fmadd_ps(x, _mm_mul_ps(x, denominator), _mm_set_ps1(315315));
    denominator =
        _mm_fmsub_ps(x, _mm_mul_ps(x, denominator), _mm_set_ps1(18918900));
    denominator =
        _mm_fmadd_ps(x, _mm_mul_ps(x, denominator), _mm_set_ps1(310134825));

    denominator = _mm_fmsub_ps(x, _mm_mul_ps(x, denominator), coeff1);
#endif
#ifndef __FMA__
    numerator =
        _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(x, numerator)), _mm_set_ps1(25740));
    numerator = _mm_sub_ps(_mm_mul_ps(x, _mm_mul_ps(x, numerator)),
                           _mm_set_ps1(2837835));
    numerator = _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(x, numerator)),
                           _mm_set_ps1(91891800));
    numerator = _mm_sub_ps(_mm_mul_ps(x, _mm_mul_ps(x, numerator)), coeff1);
    numerator = _mm_mul_ps(x, numerator);

    // imenovalec
    //   -654729075 + 310134825 a^2 - 18918900 a^4 + 315315 a^6 -
    //   1485 a^8 + a^10
    __m128 denominator = _mm_sub_ps(x2, _mm_set_ps1(-1485));
    denominator = _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(x, denominator)),
                             _mm_set_ps1(315315));
    denominator = _mm_sub_ps(_mm_mul_ps(x, _mm_mul_ps(x, denominator)),
                             _mm_set_ps1(18918900));
    denominator = _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(x, denominator)),
                             _mm_set_ps1(310134825));

    denominator = _mm_sub_ps(_mm_mul_ps(x, _mm_mul_ps(x, denominator)), coeff1);
#endif
    return _mm_div_ps(numerator, denominator);
}

/**
 *@brief This function calculates tangens of sse vector.
 *@details For detailed documentation on formula used check pdf files discussing
 *trigonometric functions. This is float version and uses sse instructions.
 */
inline __m128 tan_ver3(__m128 x) {
    // version 4 S(4,a)
    // this function should calculate tan to 1e-8 precision
    // stevec : 34459425 * a - 4729725 * a ^ 3 + 135135 * a ^ 5 - 990 a ^ 7 + a
    // ^ 9;
    __m128 x2 = _mm_mul_ps(x, x);
    __m128 coeff1 = _mm_set_ps1(34459425);
    __m128 numerator = _mm_sub_ps(x2, _mm_set_ps1(-990));
#ifdef __FMA__
    numerator = _mm_fmadd_ps(x, _mm_mul_ps(x, numerator), _mm_set_ps1(135135));
    numerator = _mm_fmsub_ps(x, _mm_mul_ps(x, numerator), _mm_set_ps1(4729725));
    numerator = _mm_fmadd_ps(x, _mm_mul_ps(x, numerator), coeff1);
    numerator = _mm_mul_ps(x, numerator);

    // imenovalec: 34459425 - 16216200*a^2 + 945945 a^4 - 13860*a^6 + 45*a^8
    __m128 denominator = _mm_add_ps(x2, _mm_set_ps1(45));
    denominator =
        _mm_fmsub_ps(x, _mm_mul_ps(x, denominator), _mm_set_ps1(13860));
    denominator =
        _mm_fmadd_ps(x, _mm_mul_ps(x, denominator), _mm_set_ps1(945945));
    denominator =
        _mm_fmsub_ps(x, _mm_mul_ps(x, denominator), _mm_set_ps1(16216200));
    denominator = _mm_fmadd_ps(x, _mm_mul_ps(x, denominator), coeff1);
#endif
#ifndef __FMA__
    numerator = _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(x, numerator)),
                           _mm_set_ps1(135135));
    numerator = _mm_sub_ps(_mm_mul_ps(x, _mm_mul_ps(x, numerator)),
                           _mm_set_ps1(4729725));
    numerator = _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(x, numerator)), coeff1);
    numerator = _mm_mul_ps(x, numerator);

    // imenovalec: 34459425 - 16216200*a^2 + 945945 a^4 - 13860*a^6 + 45*a^8
    __m128 denominator = _mm_add_ps(x2, _mm_set_ps1(45));
    denominator = _mm_sub_ps(_mm_mul_ps(x, _mm_mul_ps(x, denominator)),
                             _mm_set_ps1(13860));
    denominator = _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(x, denominator)),
                             _mm_set_ps1(945945));
    denominator = _mm_sub_ps(_mm_mul_ps(x, _mm_mul_ps(x, denominator)),
                             _mm_set_ps1(16216200));
    denominator = _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(x, denominator)), coeff1);
#endif
    return _mm_div_ps(numerator, denominator);
}
#endif

#ifdef __AVX2__
/**
 *@brief This function calculates tangens of sse vector.
 *@details For detailed documentation on formula used check pdf files discussing
 *trigonometric functions. This is double version and uses avx2 instructions.
 */
inline __m256d tan(__m256d x) {
    // this function should calculate tan to 1e-8 precision
    // stevec : 34459425 * a - 4729725 * a ^ 3 + 135135 * a ^ 5 - 990 a ^ 7
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
            _mm256_fmsub_pd(x5, _mm256_set1_pd(135135),
                            _mm256_fmsub_pd(x7, _mm256_set1_pd(990), x9))));
    // imenovalec: 34459425 - 16216200*a^2 + 945945 a^4 - 13860*a^6 + 45*a^8
    __m256d denominator = _mm256_fmsub_pd(
        x8, _mm256_set1_pd(45),
        _mm256_fmsub_pd(
            x6, _mm256_set1_pd(13860),
            _mm256_fmsub_pd(
                x4, _mm256_set1_pd(945945),
                _mm256_fmsub_pd(x2, _mm256_set1_pd(16216200), coeff1))));
    return _mm256_div_pd(numerator, denominator);
}
#endif

#ifdef __SSE__
/**
 *@brief This function calculates arcus tangens of sse vector.
 *@details For detailed documentation on formula used check pdf files discussing
 *trigonometric functions. This is float version and uses sse instructions.
 */
inline __m128 arctan(__m128 x) {
    // this comparison sets 0xffff if true - which is not 1.0
    __m128 cmp1 = _mm_cmpgt_ps(_mm_setzero_ps(), x);
    // now we compare to 1.0 to get desired 0.0 and 1.0 numbers
    cmp1 = _mm_and_ps(cmp1, _mm_set_ps1(-1.0));
    __m128 cmp1x2 = _mm_mul_ps(_mm_set_ps1(2.0), cmp1);
    // cmp1 =_mm_xor_ps(v, _mm_set1_ps(-0.0)); convert 1 to -1
    // now calculate absolute value
#ifdef __FMA__
    x = _mm_fmadd_ps(cmp1x2, x, x);
#endif
#ifndef __FMA__
    x = _mm_add_ps(_mm_mul_ps(cmp1x2, x), x);
#endif
    __m128 cmp2 = _mm_cmpgt_ps(x, _mm_set_ps1(1.0));
    // cmp2 = _mm_and_ps(cmp2, _mm_set_ps1(1.0));

    __m128 overx = _mm_div_ps(_mm_set_ps1(1.0), x);
    x = _mm_blendv_ps(x, overx, cmp2);

    // numerator coefficients a^0, a^1, a^2, a^3,....
    // {0, 2342475135, 0, 5941060125, 0, 5429886462, 0, 2146898754, 0,
    //        341536195, 0, 14928225}
#ifdef __FMA__
    __m128 numerator = _mm_fmadd_ps(_mm_mul_ps(x, _mm_set_ps1(14928225)), x,
                                    _mm_set_ps1(341536195));
    numerator =
        _mm_fmadd_ps(_mm_mul_ps(x, numerator), x, _mm_set_ps1(2146898754));

    numerator =
        _mm_fmadd_ps(_mm_mul_ps(x, numerator), x, _mm_set_ps1(5429886462));

    numerator =
        _mm_fmadd_ps(_mm_mul_ps(x, numerator), x, _mm_set_ps1(5941060125));

    numerator =
        _mm_fmadd_ps(_mm_mul_ps(x, numerator), x, _mm_set_ps1(2342475135));
    numerator = _mm_mul_ps(numerator, x);

    // {2342475135, 0, 6721885170, 0, 7202019825, 0, 3537834300, 0,
    // 780404625, 0, 62432370, 0, 800415}
    __m128 denominator = _mm_fmadd_ps(x, _mm_mul_ps(x, _mm_set_ps1(800415)),
                                      _mm_set_ps1(62432370));
    denominator =
        _mm_fmadd_ps(x, _mm_mul_ps(denominator, x), _mm_set_ps1(780404625));
    denominator =
        _mm_fmadd_ps(x, _mm_mul_ps(denominator, x), _mm_set_ps1(3537834300));
    denominator =
        _mm_fmadd_ps(x, _mm_mul_ps(denominator, x), _mm_set_ps1(7202019825));
    denominator =
        _mm_fmadd_ps(x, _mm_mul_ps(denominator, x), _mm_set_ps1(6721885170));
    denominator =
        _mm_fmadd_ps(x, _mm_mul_ps(denominator, x), _mm_set_ps1(2342475135));
#endif
#ifndef __FMA__
    __m128 numerator =
        _mm_add_ps(_mm_mul_ps(_mm_mul_ps(x, _mm_set_ps1(14928225)), x),
                   _mm_set_ps1(341536195));
    numerator = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(x, numerator), x),
                           _mm_set_ps1(2146898754));

    numerator = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(x, numerator), x),
                           _mm_set_ps1(5429886462));

    numerator = _mm_fmadd_ps(_mm_mul_ps(_mm_mul_ps(x, numerator), x),
                             _mm_set_ps1(5941060125));

    numerator = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(x, numerator), x),
                           _mm_set_ps1(2342475135));
    numerator = _mm_mul_ps(numerator, x);

    // {2342475135, 0, 6721885170, 0, 7202019825, 0, 3537834300, 0,
    // 780404625, 0, 62432370, 0, 800415}
    __m128 denominator =
        _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(x, _mm_set_ps1(800415))),
                   _mm_set_ps1(62432370));
    denominator = _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(denominator, x)),
                             _mm_set_ps1(780404625));
    denominator = _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(denominator, x)),
                             _mm_set_ps1(3537834300));
    denominator = _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(denominator, x)),
                             _mm_set_ps1(7202019825));
    denominator = _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(denominator, x)),
                             _mm_set_ps1(6721885170));
    denominator = _mm_add_ps(_mm_mul_ps(x, _mm_mul_ps(denominator, x)),
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
    cmp1 = _mm_add_ps(_mm_mul_ps(cmp1, _mm_set_ps1(2.0)), _mm_set_ps1(1.0));
#endif
    result = _mm_mul_ps(result, cmp1);
    return result;
}
#endif

#ifdef __AVX2__
/**
 *@brief This function calculates arcus tangens of sse vector.
 *@details For detailed documentation on formula used check pdf files discussing
 *trigonometric functions. This is double version and uses avx2 instructions.
 */
inline __m256d arctan(__m256d x) {
    // this comparison sets 0xffff if true - which is not 1.0
    __m256d cmp1 = _mm256_cmp_pd(_mm256_setzero_pd(), x, _CMP_GT_OS);
    // now we compare to 1.0 to get desired 0.0 and 1.0 numbers
    cmp1 = _mm256_and_pd(cmp1, _mm256_set1_pd(-1.0));
    __m256d cmp1x2 = _mm256_mul_pd(_mm256_set1_pd(2.0), cmp1);
    // cmp1 =_mm_xor_ps(v, _mm_set1_ps(-0.0)); convert 1 to -1
    // now calculate absolute value
    x = _mm256_fmadd_pd(cmp1x2, x, x);

    __m256d cmp2 = _mm256_cmp_pd(x, _mm256_set1_pd(1.0), _CMP_GT_OS);
    // cmp2 = _mm_and_ps(cmp2, _mm_set_ps1(1.0));

    __m256d overx = _mm256_div_pd(_mm256_set1_pd(1.0), x);
    x = _mm256_blendv_pd(x, overx, cmp2);

    // numerator coefficients a^0, a^1, a^2, a^3,....
    // double precision
    //    {0, 579118415250375, 0, 2338535314915800, 0, 3874828669311600, 0,
    //        3393172947724560, 0, 1684523959239550, 0, 471862454520600, 0,
    //        69652545704280, 0, 4581496496640, 0, 87091688691}
    __m256d numerator =
        _mm256_fmadd_pd(_mm256_mul_pd(x, _mm256_set1_pd(87091688691)), x,
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
    // {579118415250375, 0, 2531574786665925, 0, 4602863248483500, 0,
    // 4503876942064500, 0, 2562550673933250, 0, 854183557977750, 0,
    // 159447597489180, 0, 14855366225700, 0, 530548793775, 0, 3102624525}
    __m256d denominator =
        _mm256_fmadd_pd(x, _mm256_mul_pd(x, _mm256_set1_pd(3102624525)),
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
    cmp1 = _mm256_fmadd_pd(cmp1, _mm256_set1_pd(2.0), _mm256_set1_pd(1.0));
    result = _mm256_mul_pd(result, cmp1);
    return result;
}
#endif

/**
 * @brief The function finds all integer pairs whose multiplication yield the
 * supplied.
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
 * @brief The function return closest pair of numbers
 * @param Vector of pairs among which to find closest pair
 */
inline std::pair<int, int>
closest_pair(std::vector<std::pair<int, int>> pairs) {
    std::pair<int, int> min_el = *std::min_element(
        std::begin(pairs), std::end(pairs),
        [](std::pair<int, int> a, std::pair<int, int> b) {
            return std::abs(a.first - a.second) < std::abs(b.first - b.second);
        });
    return min_el;
}
