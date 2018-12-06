
inline __m128 load_vertex(float* value) {
    // load the x and y element of the float3 vector using a 64 bit load
    // and set the upper 64 bits to zero (00YX)
    __m128 xy = _mm_loadl_pi(_mm_setzero_ps(), ( __m64 *)value);
    // now load the z element using a 32 bit load (000Z)
    __m128 z = _mm_load_ss(value + 2);
    // finally, combine both by moving the z component to the high part
    // of the register, while keeping x and y components in the low part
    return _mm_movelh_ps(xy, z);  // (0ZYX)
}

inline __m128 load_vertex2(float* value) {
    // load x, y with a 64 bit integer load (00YX)
  __m128i xy = _mm_loadl_epi64((__m128i*)value);
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
    shufReg = _mm_movehdup_ps(mulRes);  // Broadcast elements 3,1 to 2,0
    sumsReg = _mm_add_ps(mulRes, shufReg);
    shufReg = _mm_movehl_ps(shufReg, sumsReg);  // High Half -> Low Half
    sumsReg = _mm_add_ss(sumsReg, shufReg);
    return _mm_cvtss_f32(
        sumsReg);  // Result in the lower part of the SSE Register
}

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
}  // namespace std
