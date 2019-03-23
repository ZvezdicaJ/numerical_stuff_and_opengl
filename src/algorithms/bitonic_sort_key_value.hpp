#pragma once
#ifdef __AVX2__
#include <bitset>

#ifndef __BITONIC_SORT_KEY_VALUE__
#define __BITONIC_SORT_KEY_VALUE__

std::ostream &operator<<(std::ostream &out,
                         std::vector<std::pair<int, float>> &vec) {

    for (auto &a : vec)
        std::cout << a << std::endl;
    return out;
}

namespace BITONIC_SORT_KEY_VALUE {

static const int LOAD = 0xffffffff;
static const int STORE = 0xffffffff;

inline __attribute__((always_inline)) __m256i
_mm256_blendv_epi32(const __m256i &a, const __m256i &b, const __m256i &mask) {
    return _mm256_castps_si256(_mm256_blendv_ps(_mm256_castsi256_ps(a),
                                                _mm256_castsi256_ps(b),
                                                _mm256_castsi256_ps(mask)));
}

/**
 *@brief The function accepts a single __m256 vector and sorts it.
 * @param reg register to be sorted
 */
void bitonic_sort(__m256 &reg, __m256i &key) {

    __m256i mask1 =
        _mm256_set_epi32(0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
                         0xffffffff, 0x00000000, 0xffffffff, 0x00000000);
    __m256i mask2 =
        _mm256_set_epi32(0xffffffff, 0xffffffff, 0x00000000, 0x00000000,
                         0xffffffff, 0xffffffff, 0x00000000, 0x00000000);

    __m256i mask3 =
        _mm256_set_epi32(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                         0x00000000, 0x00000000, 0x00000000, 0x00000000);

    {
        // prvi korak: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b10110001);
        // print_avx(shuffled_reg, "shuffled: ");
        __m256 mask = _mm256_cmp_ps(reg, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask1));

        __m256i shuffled_key = _mm256_shuffle_epi32(key, 0b10110001);
        key = _mm256_blendv_epi32(shuffled_key, key, _mm256_castps_si256(mask));

        reg = _mm256_blendv_ps(shuffled_reg, reg, mask);
    }
    {
        // drugi korak *----* *--*     *----* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b00011011);

        //__m256 max = _mm256_max_ps(reg, shuffled_reg);
        //__m256 min = _mm256_min_ps(reg, shuffled_reg);
        __m256 mask = _mm256_cmp_ps(reg, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask2));

        __m256i shuffled_key = _mm256_shuffle_epi32(key, 0b00011011);
        key = _mm256_blendv_epi32(shuffled_key, key, _mm256_castps_si256(mask));

        reg = _mm256_blendv_ps(shuffled_reg, reg, mask);
        // max mora biti pri 256
        // min is located at the start of register (at 0 - lower half)
    }
    {
        // ponovimo prvi korak: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b10110001);
        __m256 mask = _mm256_cmp_ps(reg, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask1));

        __m256i shuffled_key = _mm256_shuffle_epi32(key, 0b10110001);
        key = _mm256_blendv_epi32(shuffled_key, key, _mm256_castps_si256(mask));

        reg = _mm256_blendv_ps(shuffled_reg, reg, mask);
    }
    {
        // now we have to reverse register:
        // *--------* *------* *----* *--*
        // the following line first reverses the two halves of register
        // in the next step it reverses both halves
        __m256 reversed_halves = _mm256_permute2f128_ps(reg, reg, 0b00000001);
        __m256 reversed =
            _mm256_shuffle_ps(reversed_halves, reversed_halves, 0b00011011);
        __m256 mask = _mm256_cmp_ps(reg, reversed, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask3));

        __m256i reversed_key_halves =
            _mm256_permute2f128_si256(key, key, 0b00000001);
        __m256i reversed_key =
            _mm256_shuffle_epi32(reversed_key_halves, 0b00011011);

        key = _mm256_blendv_epi32(reversed_key, key, _mm256_castps_si256(mask));

        reg = _mm256_blendv_ps(reversed, reg, mask);
    }
    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b01001110);
        __m256 mask = _mm256_cmp_ps(reg, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask2));
        __m256i shuffled_key = _mm256_shuffle_epi32(key, 0b01001110);
        key = _mm256_blendv_epi32(shuffled_key, key, _mm256_castps_si256(mask));
        reg = _mm256_blendv_ps(shuffled_reg, reg, mask);
    }
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b10110001);
        __m256 mask = _mm256_cmp_ps(reg, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask1));

        __m256i shuffled_key = _mm256_shuffle_epi32(key, 0b10110001);
        key = _mm256_blendv_epi32(shuffled_key, key, _mm256_castps_si256(mask));

        reg = _mm256_blendv_ps(shuffled_reg, reg, mask);
    }
}

} // namespace BITONIC_SORT_KEY_VALUE
#endif
#endif
