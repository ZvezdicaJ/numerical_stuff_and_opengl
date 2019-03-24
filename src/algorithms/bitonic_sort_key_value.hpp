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

/**
 * @brief The function blends two __m256i vectors.
 * @param a first vector to be blended
 * @param second vector to be blended
 * @param mask mask according to which to blend two vector. For more into check
 * Intel intrinsics guide - _mm256_blendv_ps function
 */
inline __attribute__((always_inline)) __m256i
_mm256_blendv_epi32(const __m256i &a, const __m256i &b, const __m256i &mask) {
    return _mm256_castps_si256(_mm256_blendv_ps(_mm256_castsi256_ps(a),
                                                _mm256_castsi256_ps(b),
                                                _mm256_castsi256_ps(mask)));
}

/**
 * @brief The overload function with mask neing of __m256 type.
 * @param a first vector to be blended
 * @param second vector to be blended
 * @param mask mask according to which to blend two vector. For more into check
 * Intel intrinsics guide - _mm256_blendv_ps function
 */
inline __attribute__((always_inline)) __m256i
_mm256_blendv_epi32(const __m256i &a, const __m256i &b, const __m256 &mask) {
    return _mm256_castps_si256(
        _mm256_blendv_ps(_mm256_castsi256_ps(a), _mm256_castsi256_ps(b), mask));
}

inline __attribute__((always_inline)) void
shuffle_and_compare(__m256 &reg, __m256i &key, const __m256i &mask_epi32,
                    const uint8_t mask8) {
    //  *----* *----*     *----* *----*
    __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, mask8);
    __m256 mask = _mm256_cmp_ps(reg, shuffled_reg, _CMP_LE_OQ);
    mask = _mm256_castsi256_ps(
        _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));

    __m256i shuffled_key = _mm256_shuffle_epi32(key, mask8);
    key = _mm256_blendv_epi32(shuffled_key, key, mask);

    reg = _mm256_blendv_ps(shuffled_reg, reg, mask);
}

/**
 * @brief The function accepts a single __m256 vector and sorts it.
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

    shuffle_and_compare(reg, key, mask1, 0b10110001);
    shuffle_and_compare(reg, key, mask2, 0b00011011);
    shuffle_and_compare(reg, key, mask1, 0b10110001);

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
 * @brief The function accepts two unsorted  __m256 vectors two __m256i vector
 * containing indices and sorts them according to values in __m256 vector.
 * @param reg1 upper vector of numbers - at the end it contains larger values,
 * the largest value is in the upper half of register [255:192]
 *
 * @param reg0 lower vector of numbers - at the end it contains smaller values.
 * The smallest value is in the lowest half of register - at [63:0]
 * @param
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
    shuffle_and_compare(reg0, key0, mask2, 0b01001110);
    shuffle_and_compare(reg0, key0, mask1, 0b10110001);

    shuffle_and_compare(reg1, key1, mask2, 0b01001110);
    shuffle_and_compare(reg1, key1, mask1, 0b10110001);

    return;
}

/**
 * @brief The function accepts four unsorted  __m256 vectors and sorts them.
 * @param reg3 upper vector of numbers - at the end it contains larger values,
 * the largest value is in the upper half of register [255:192]
 * @param reg2
 * @param reg1
 * @param reg0 lower vector of numbers - at the end it contains smaller values.
 * The smallest value is in the lowest half of register - at [63:0]
 *
 */
inline void bitonic_sort(__m256 &reg0, __m256 &reg1, __m256 &reg2, __m256 &reg3,
                         __m256i &key0, __m256i &key1, __m256i &key2,
                         __m256i &key3) {

    // sort each quarter
    bitonic_sort(reg0, key0);
    bitonic_sort(reg1, key1);
    bitonic_sort(reg2, key2);
    bitonic_sort(reg3, key3);
    // sort each half
    bitonic_sort(reg0, reg1, key0, key1);
    bitonic_sort(reg2, reg3, key2, key3);

    // sort full width
    { // reverse lover half and compare to upper half
        __m256 reversed_halves = _mm256_permute2f128_ps(reg0, reg0, 0b00000001);
        __m256 reversed =
            _mm256_shuffle_ps(reversed_halves, reversed_halves, 0b00011011);

        __m256i reversed_key_halves =
            _mm256_permute2f128_si256(key0, key0, 0b00000001);
        __m256i reversed_key =
            _mm256_shuffle_epi32(reversed_key_halves, 0b00011011);

        __m256 mask = _mm256_cmp_ps(reversed, reg3, _CMP_LE_OQ);

        reg0 = _mm256_blendv_ps(reg3, reversed, mask);
        reg3 = _mm256_blendv_ps(reversed, reg3, mask);

        key0 =
            _mm256_blendv_epi32(key3, reversed_key, _mm256_castps_si256(mask));
        key3 =
            _mm256_blendv_epi32(reversed_key, key3, _mm256_castps_si256(mask));
    }

    { // reverse lover half and compare to upper half
        __m256 reversed_halves = _mm256_permute2f128_ps(reg1, reg1, 0b00000001);
        __m256 reversed =
            _mm256_shuffle_ps(reversed_halves, reversed_halves, 0b00011011);

        __m256i reversed_key_halves =
            _mm256_permute2f128_si256(key1, key1, 0b00000001);
        __m256i reversed_key =
            _mm256_shuffle_epi32(reversed_key_halves, 0b00011011);

        __m256 mask = _mm256_cmp_ps(reversed, reg2, _CMP_LE_OQ);

        reg1 = _mm256_blendv_ps(reg2, reversed, mask);
        reg2 = _mm256_blendv_ps(reversed, reg2, mask);

        key1 =
            _mm256_blendv_epi32(key2, reversed_key, _mm256_castps_si256(mask));
        key2 =
            _mm256_blendv_epi32(reversed_key, key2, _mm256_castps_si256(mask));
    }
    {
        __m256 mask = _mm256_cmp_ps(reg1, reg3, _CMP_LE_OQ);
        __m256 tmp = _mm256_blendv_ps(reg3, reg1, mask);
        reg3 = _mm256_blendv_ps(reg1, reg3, mask);
        reg1 = tmp;
        __m256i tmp_key = _mm256_blendv_epi32(key3, key1, mask);
        key3 = _mm256_blendv_epi32(key1, key3, mask);
        key1 = tmp_key;
        /////////////// repeat the same for registers 0 and 2

        mask = _mm256_cmp_ps(reg0, reg2, _CMP_LE_OQ);
        tmp = _mm256_blendv_ps(reg2, reg0, mask);
        reg2 = _mm256_blendv_ps(reg0, reg2, mask);
        reg0 = tmp;
        tmp_key = _mm256_blendv_epi32(key2, key0, mask);
        key2 = _mm256_blendv_epi32(key0, key2, mask);
        key0 = tmp_key;
    }

    {
        __m256 mask = _mm256_cmp_ps(reg0, reg1, _CMP_LE_OQ);
        __m256 tmp = _mm256_blendv_ps(reg1, reg0, mask);
        reg1 = _mm256_blendv_ps(reg0, reg1, mask);
        reg0 = tmp;
        __m256i tmp_key = _mm256_blendv_epi32(key1, key0, mask);
        key1 = _mm256_blendv_epi32(key0, key1, mask);
        key0 = tmp_key;
        /////////////// repeat the same for registers 2 and 3

        mask = _mm256_cmp_ps(reg2, reg3, _CMP_LE_OQ);
        tmp = _mm256_blendv_ps(reg3, reg2, mask);
        reg3 = _mm256_blendv_ps(reg2, reg3, mask);
        reg2 = tmp;
        tmp_key = _mm256_blendv_epi32(key3, key2, mask);
        key3 = _mm256_blendv_epi32(key2, key3, mask);
        key2 = tmp_key;
    }

    __m256i mask_epi32 =
        _mm256_set_epi32(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                         0x00000000, 0x00000000, 0x00000000, 0x00000000);
    { // mix across avx lanes
        __m256 shuffled_reg = _mm256_permute2f128_ps(reg0, reg0, 0b00000001);

        __m256 mask = _mm256_cmp_ps(reg0, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));

        __m256i shuffled_key =
            _mm256_permute2f128_si256(key0, key0, 0b00000001);
        key0 =
            _mm256_blendv_epi32(shuffled_key, key0, _mm256_castps_si256(mask));

        reg0 = _mm256_blendv_ps(shuffled_reg, reg0, mask);
    }

    { // mix across avx lanes
        __m256 shuffled_reg = _mm256_permute2f128_ps(reg1, reg1, 0b00000001);
        __m256 mask = _mm256_cmp_ps(reg1, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));

        __m256i shuffled_key =
            _mm256_permute2f128_si256(key1, key1, 0b00000001);
        key1 = _mm256_blendv_epi32(shuffled_key, key1, mask);

        reg1 = _mm256_blendv_ps(shuffled_reg, reg1, mask);
    }
    { // mix across avx lanes
        __m256 shuffled_reg = _mm256_permute2f128_ps(reg2, reg2, 0b00000001);
        __m256 mask = _mm256_cmp_ps(reg2, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));

        __m256i shuffled_key =
            _mm256_permute2f128_si256(key2, key2, 0b00000001);
        key2 = _mm256_blendv_epi32(shuffled_key, key2, mask);

        reg2 = _mm256_blendv_ps(shuffled_reg, reg2, mask);
    }
    { // mix across avx lanes
        __m256 shuffled_reg = _mm256_permute2f128_ps(reg3, reg3, 0b00000001);
        __m256 mask = _mm256_cmp_ps(reg3, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));

        __m256i shuffled_key =
            _mm256_permute2f128_si256(key3, key3, 0b00000001);
        key3 = _mm256_blendv_epi32(shuffled_key, key3, mask);

        reg3 = _mm256_blendv_ps(shuffled_reg, reg3, mask);
    }

    ///////////////////////////////////////////////////////////
    // mix inside avx lanes each register separately, but jump single value
    mask_epi32 =
        _mm256_set_epi32(0xffffffff, 0xffffffff, 0x00000000, 0x00000000,
                         0xffffffff, 0xffffffff, 0x00000000, 0x00000000);

    shuffle_and_compare(reg0, key0, mask_epi32, 0b01001110);
    shuffle_and_compare(reg1, key1, mask_epi32, 0b01001110);
    shuffle_and_compare(reg2, key2, mask_epi32, 0b01001110);
    shuffle_and_compare(reg3, key3, mask_epi32, 0b01001110);

    ////////////////////////////////////////////////
    mask_epi32 =
        _mm256_set_epi32(0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
                         0xffffffff, 0x00000000, 0xffffffff, 0x00000000);

    shuffle_and_compare(reg0, key0, mask_epi32, 0b10110001);
    shuffle_and_compare(reg1, key1, mask_epi32, 0b10110001);
    shuffle_and_compare(reg2, key2, mask_epi32, 0b10110001);
    shuffle_and_compare(reg3, key3, mask_epi32, 0b10110001);
}

} // namespace BITONIC_SORT_KEY_VALUE
#endif
#endif
