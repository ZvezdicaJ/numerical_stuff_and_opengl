
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
    __m128 result =
        _mm_fmsub_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1)),
                     _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2)),
                     _mm_mul_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 1, 0, 2)),
                                _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1))
                                ));
    return result;
}

inline float CalcDotProductSse(__m128 x, __m128 y) {
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
    // T1
    __m128 T1 = x_vec;
    if (n == 1)
        return x_vec;
    // T2
    __m128 T2 =
        _mm_sub_ps(_mm_mul_ps(_mm_mul_ps(_mm_set_ps1(2.0), x_vec), x_vec), T0);
    if (n == 2)
        return T2;
    // T3
    __m128 T3 =
        _mm_sub_ps(_mm_mul_ps(_mm_mul_ps(_mm_set_ps1(2.0), T2), x_vec), x_vec);
    if (n == 3)
        return T3;
    __m128 Tn = T3;
    __m128 Tnm1 = T2;
    __m128 Tnp1;
    for (int i = 4; i <= n; i++) {
        Tnp1 = _mm_sub_ps(_mm_mul_ps(_mm_mul_ps(_mm_set_ps1(2.0), Tn), x_vec),
                          Tnm1);
        Tnm1 = Tn;
        Tn = Tnp1;
    }
    return Tnp1;
}

inline __m128 chebyshev_next(__m128 &cn, __m128 &cn_1, __m128 &x_vec) {
    return _mm_fmsub_ps(_mm_mul_ps(_mm_set_ps1(2.0), cn), x_vec, cn_1);
}

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
        sum = _mm_fmadd_ps(coeff4, T2n, sum);
    }
    return sum;
}

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
        sum = _mm_fmadd_ps(coeff4, T2np1, sum);
        T2n = chebyshev_next(T2np1, T2n, x_vec);
        T2np1 = chebyshev_next(T2n, T2np1, x_vec);
    }
    return sum;
}
