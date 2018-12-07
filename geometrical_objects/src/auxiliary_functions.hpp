
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

inline __m128i factorial(int n) {
    __m128i factor = _mm_set_epi32(n, n, n, n);
    __m128i prod = _mm_set_epi32(1, 1, 1, 1);
    for (int i = 2; i <= n; i++) {
        prod = _mm_mul_epi32(prod, factor);
    }
    return prod;
}
inline __m128 chebyshev(int n, __m128 x_vec) {
    // T0
    __m128 T0 = _mm_set_ps(1.0, 1.0, 1.0, 1.0);
    if (n == 0)
        return T0;
    // T1
    __m128 T1 = x_vec;
    if (n == 1)
        return x_vec;
    // T2
    __m128 T2 = _mm_sub_ps(
        _mm_mul_ps(_mm_mul_ps(_mm_set_ps(2.0, 2.0, 2.0, 2.0), x_vec), x_vec),
        T0);
    if (n == 2)
        return T2;
    // T3
    __m128 T3 = _mm_sub_ps(
        _mm_mul_ps(_mm_mul_ps(_mm_set_ps(2.0, 2.0, 2.0, 2.0), T2), x_vec),
        x_vec);
    if (n == 3)
        return T3;
    __m128 Tn = T3;
    __m128 Tnm1 = T2;
    __m128 Tnp1;
    for (int i = 4; i <= n; i++) {
        Tnp1 = _mm_sub_ps(
            _mm_mul_ps(_mm_mul_ps(_mm_set_ps(2.0, 2.0, 2.0, 2.0), Tn), x_vec),
            Tnm1);
        Tnm1 = Tn;
        Tn = Tnp1;
    }
    return Tnp1;
}

inline __m128 chebyshev_next(__m128& p1, __m128& p2, __m128& x_vec) {
    return _mm_sub_ps(
        _mm_mul_ps(_mm_mul_ps(_mm_set_ps(2.0, 2.0, 2.0, 2.0), p1), x_vec), p2);
}

inline __m128 J2n(int n, __m128 x = _mm_set_ps(1.0, 1.0, 1.0, 1.0)) {
    __m128 ln = _mm_set_ps(4.0, 4.0, 4.0, 4.0);
    __m128 ln_power = _mm_set_ps(1.0, 1.0, 1.0, 1.0);
    for (int i = 0; i < n; i++) {
        ln_power = _mm_mul_ps(ln_power, ln);
    }
    __m128 sum = _mm_set_ps(0.0, 0.0, 0.0, 0.0);
    __m128 m1 = _mm_set_ps(-1.0, -1.0, -1.0, -1.0);
    __m128i fact = factorial(n);
    for (int l = 0; l < 1000; l++) {
        __m128 tmp = _mm_mul_ps(ln_power, _mm_cvtepi32_ps(fact));
        sum = _mm_add_ps(_mm_div_ps(m1, tmp), sum);
        int tmpf = 2 * n + 1;
        fact = _mm_mul_epi32(fact, _mm_set_epi32(tmpf, tmpf, tmpf, tmpf));
    }
    return sum;
}

inline __m128 sse_cos(__m128 x_vec) {
    __m128 sum = J2n(0);
    for (int n = 1; n <= 100; n++) {
        __m128 m1 = _mm_set_ps(-1.0, -1.0, -1.0, -1.0);
        sum = _mm_add_ps(sum, _mm_mul_ps(J2n(n), chebyshev(2 * n, x_vec)));
    }
    return sum;
}
