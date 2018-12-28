
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

inline __m128 load_vertex2(float *value) {
    // load x, y with a 64 bit integer load (00YX)
    __m128i xy = _mm_loadl_epi64((__m128i *)value);
    // now load the z element using a 32 bit float load (000Z)
    __m128 z = _mm_load_ss(value + 2);
    // we now need to cast the __m128i register into a __m18 one (0ZYX)
    return _mm_movelh_ps(_mm_castsi128_ps(xy), z);
}

inline __m128 cross_product(__m128 a, __m128 b) {
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

#ifdef __AVX2__
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

inline float CalcDotProduct(float vec1[3], float vec2[3]) {
    return vec1[0] * vec2[0] + vec1[1] * vec2[1] + vec1[2] * vec2[2];
}

#ifdef __AVX2__
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

// this is slow; do not use
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

inline int scalar_factorial(int n) {
    int prod = 1;
    for (int i = 2; i <= n; i++) {
        prod *= i;
    }
    return prod;
}

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

#ifdef __AVX2__
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

inline __m128 chebyshev_next(__m128 &cn, __m128 &cn_1, __m128 &x_vec) {
#ifdef __FMA__
    return _mm_fmsub_ps(_mm_mul_ps(_mm_set_ps1(2.0), cn), x_vec, cn_1);
#endif
#ifndef __FMA__
    return _mm_sub_ps(_mm_mul_ps(_mm_mul_ps(_mm_set_ps1(2.0), cn), x_vec),
                      cn_1);
#endif
}

#ifdef __AVX2__
inline __m256d chebyshev_next(__m256d &cn, __m256d &cn_1, __m256d &x_vec) {
    return _mm256_fmsub_pd(_mm256_mul_pd(_mm256_set1_pd(2.0), cn), x_vec, cn_1);
}
#endif

inline __m128 sse_cos(__m128 x_vec_) {
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

#ifdef __AVX2__
inline __m256d avx_cos(__m256d x_vec_) {
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

inline __m128 sse_sin(__m128 x_vec_) {
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

inline __m128 sse_tan(__m128 x) {
    // this function should calculate tan to 1e-8 precision
    // stevec : 34459425 * a - 4729725 * a ^ 3 + 135135 * a ^ 5 - 990 a ^ 7 + a
    // ^ 9;
    __m128 x2 = _mm_mul_ps(x, x);
    __m128 x3 = _mm_mul_ps(x2, x);
    __m128 x4 = _mm_mul_ps(x2, x2);
    __m128 x5 = _mm_mul_ps(x3, x2);
    __m128 x6 = _mm_mul_ps(x3, x3);
    __m128 x7 = _mm_mul_ps(x3, x4);
    __m128 x8 = _mm_mul_ps(x4, x4);
    __m128 x9 = _mm_mul_ps(x4, x5);
    __m128 coeff1 = _mm_set_ps1(34459425);
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
    return _mm_div_ps(numerator, denominator);
}

#ifdef __AVX2__
inline __m256d avx_tan(__m256d x) {
    // this function should calculate tan to 1e-8 precision
    // stevec : 34459425 * a - 4729725 * a ^ 3 + 135135 * a ^ 5 - 990 a ^ 7 + a
    // ^ 9;
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
