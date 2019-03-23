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

/**
 * @brief The function accepts two already sorted  __m256 vectors and sorts
 * them.
 * @param reg1 upper vector of numbers - at the end it contains larger values,
 * the largest value is in the upper half of register [255:192]
 *
 * @param reg0 lower vector of numbers - at the end it contains smaller values.
 * The smallest value is in the lowest half of register - at [63:0]
 */
inline void bitonic_sort(__m256 &reg0, __m256 &reg1, __m256i &key0,
                         __m256i &key1) {
    bitonic_sort(reg1, key1); // sort first register
    bitonic_sort(reg0, key0); // sort second register

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
        // reverse one of registers register reg0
        __m256 reversed_halves = _mm256_permute2f128_ps(reg0, reg0, 0b00000001);
        __m256 reversed =
            _mm256_shuffle_ps(reversed_halves, reversed_halves, 0b00011011);

        __m256i reversed_key_halves =
            _mm256_permute2f128_si256(key0, key0, 0b00000001);
        __m256i reversed_key =
            _mm256_shuffle_epi32(reversed_key_halves, 0b00011011);

        __m256 mask = _mm256_cmp_ps(reg1, reversed, _CMP_LE_OQ);

        reg0 = _mm256_blendv_ps(reversed, reg1, mask);
        reg1 = _mm256_blendv_ps(reg1, reversed, mask);

        key0 =
            _mm256_blendv_epi32(reversed_key, key1, _mm256_castps_si256(mask));
        key1 =
            _mm256_blendv_epi32(key1, reversed_key, _mm256_castps_si256(mask));
    }
    {
        //
        __m256 reversed_halves = _mm256_permute2f128_ps(reg0, reg0, 0b00000001);
        __m256 mask = _mm256_cmp_ps(reg0, reversed_halves, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask3));
        reg0 = _mm256_blendv_ps(reversed_halves, reg0, mask);

        __m256i reversed_key_halves =
            _mm256_permute2f128_si256(key0, key0, 0b00000001);

        key0 = _mm256_blendv_epi32(reversed_key_halves, key0,
                                   _mm256_castps_si256(mask));
    }
    {
        //
        __m256 reversed_halves = _mm256_permute2f128_ps(reg1, reg1, 0b00000001);
        __m256 mask = _mm256_cmp_ps(reg1, reversed_halves, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask3));
        reg1 = _mm256_blendv_ps(reversed_halves, reg1, mask);

        __m256i reversed_key_halves =
            _mm256_permute2f128_si256(key1, key1, 0b00000001);
        key1 = _mm256_blendv_epi32(reversed_key_halves, key1,
                                   _mm256_castps_si256(mask));
    }

    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg0, reg0, 0b01001110);
        __m256 mask = _mm256_cmp_ps(reg0, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask2));
        reg0 = _mm256_blendv_ps(shuffled_reg, reg0, mask);

        __m256i shuffled_key = _mm256_shuffle_epi32(key0, 0b01001110);
        key0 =
            _mm256_blendv_epi32(shuffled_key, key0, _mm256_castps_si256(mask));
    }

    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg0, reg0, 0b10110001);

        __m256 mask = _mm256_cmp_ps(reg0, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask1));
        reg0 = _mm256_blendv_ps(shuffled_reg, reg0, mask);
        __m256i shuffled_key = _mm256_shuffle_epi32(key0, 0b10110001);
        key0 =
            _mm256_blendv_epi32(shuffled_key, key0, _mm256_castps_si256(mask));
    }

    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg1, reg1, 0b01001110);
        __m256 mask = _mm256_cmp_ps(reg1, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask2));
        reg1 = _mm256_blendv_ps(shuffled_reg, reg1, mask);

        __m256i shuffled_key = _mm256_shuffle_epi32(key1, 0b01001110);
        key1 =
            _mm256_blendv_epi32(shuffled_key, key1, _mm256_castps_si256(mask));
    }

    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg1, reg1, 0b10110001);

        __m256 mask = _mm256_cmp_ps(reg1, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask1));
        reg1 = _mm256_blendv_ps(shuffled_reg, reg1, mask);
        __m256i shuffled_key = _mm256_shuffle_epi32(key1, 0b10110001);
        key1 =
            _mm256_blendv_epi32(shuffled_key, key1, _mm256_castps_si256(mask));
    }
    return;
}

} // namespace BITONIC_SORT_KEY_VALUE
#endif
#endif
