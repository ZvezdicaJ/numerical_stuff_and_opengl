#pragma once
#ifdef __AVX2__
#include <bitset>

#ifndef __IMPROVED_BITONIC_SORT_FLOAT__
#define __IMPROVED_BITONIC_SORT_FLOAT__

namespace IMPROVED_BITONIC_SORT {

static const int LOAD = 0xffffffff;
static const int STORE = 0xffffffff;

/**
 *@brief The function accepts a single __m256 vector and sorts it.
 * @param reg register to be sorted
 */
void bitonic_sort(__m256 &reg) {

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
        /* print_avx(mask, "mask after xor: ");
        std::bitset<32> bitset3(*reinterpret_cast<unsigned long *>(p));
        std::bitset<32> bitset4(*reinterpret_cast<unsigned long *>(p + 1));
        std::cout << *p << ": " << bitset3 << '\n';
        std::cout << *(p + 1) << ": " << bitset4 << "\n\n\n";
        */

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
        reg = _mm256_blendv_ps(reversed, reg, mask);
    }
    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b01001110);
        __m256 mask = _mm256_cmp_ps(reg, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask2));
        reg = _mm256_blendv_ps(shuffled_reg, reg, mask);
    }
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b10110001);
        __m256 mask = _mm256_cmp_ps(reg, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask1));
        reg = _mm256_blendv_ps(shuffled_reg, reg, mask);
    }
}

/**
 * @brief The function accepts two unsorted  __m256 vectors and sorts them.
 * @param reg1 upper vector of numbers - at the end it contains larger values,
 * the largest value is in the upper half of register [255:192]
 *
 * @param reg0 lower vector of numbers - at the end it contains smaller values.
 * The smallest value is in the lowest half of register - at [63:0]
 *
 */

inline void bitonic_sort(__m256 &reg0, __m256 &reg1) {
    bitonic_sort(reg1); // sort first register
    bitonic_sort(reg0); // sort second register

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

        // register 2 vsebuje min vrednosti
        reg0 = _mm256_min_ps(reg1, reversed);
        // register 1 vsebuje max vrednosti
        reg1 = _mm256_max_ps(reg1, reversed);
    }
    {
        // tu narediš *----* *----* *----* *----*
        __m256 reversed_halves = _mm256_permute2f128_ps(reg0, reg0, 0b00000001);
        __m256 mask = _mm256_cmp_ps(reg0, reversed_halves, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask3));
        reg0 = _mm256_blendv_ps(reversed_halves, reg0, mask);
    }
    {
        // tu narediš *----* *----* *----* *----*
        __m256 reversed_halves = _mm256_permute2f128_ps(reg1, reg1, 0b00000001);
        __m256 mask = _mm256_cmp_ps(reg1, reversed_halves, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask3));
        reg1 = _mm256_blendv_ps(reversed_halves, reg1, mask);
    }

    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg0, reg0, 0b01001110);
        __m256 mask = _mm256_cmp_ps(reg0, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask2));
        reg0 = _mm256_blendv_ps(shuffled_reg, reg0, mask);
    }
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg0, reg0, 0b10110001);

        __m256 mask = _mm256_cmp_ps(reg0, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask1));
        reg0 = _mm256_blendv_ps(shuffled_reg, reg0, mask);
    }

    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg1, reg1, 0b01001110);
        __m256 mask = _mm256_cmp_ps(reg1, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask2));
        reg1 = _mm256_blendv_ps(shuffled_reg, reg1, mask);
    }
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg1, reg1, 0b10110001);
        __m256 mask = _mm256_cmp_ps(reg1, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask1));
        reg1 = _mm256_blendv_ps(shuffled_reg, reg1, mask);
    }
    return;
}

/**
 * @brief The function accepts two already sorted  __m256 vectors and sorts
 * them.
 * @param reg1 upper vector of numbers - at the end it contains larger values,
 * the largest value is in the upper half of register [255:192]
 *
 * @param reg0 lower vector of numbers - at the end it contains smaller values.
 * The smallest value is in the lowest half of register - at [63:0]
 *
 */
inline void bitonic_merge(__m256 &reg0, __m256 &reg1) {

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

        // register 2 vsebuje min vrednosti
        reg0 = _mm256_min_ps(reg1, reversed);
        // register 1 vsebuje max vrednosti
        reg1 = _mm256_max_ps(reg1, reversed);
    }
    {
        // tu narediš *----* *----* *----* *----*
        __m256 reversed_halves = _mm256_permute2f128_ps(reg0, reg0, 0b00000001);
        __m256 mask = _mm256_cmp_ps(reg0, reversed_halves, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask3));
        reg0 = _mm256_blendv_ps(reversed_halves, reg0, mask);
    }
    {
        // tu narediš *----* *----* *----* *----*
        __m256 reversed_halves = _mm256_permute2f128_ps(reg1, reg1, 0b00000001);
        __m256 mask = _mm256_cmp_ps(reg1, reversed_halves, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask3));
        reg1 = _mm256_blendv_ps(reversed_halves, reg1, mask);
    }

    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg0, reg0, 0b01001110);
        __m256 mask = _mm256_cmp_ps(reg0, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask2));
        reg0 = _mm256_blendv_ps(shuffled_reg, reg0, mask);
    }
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg0, reg0, 0b10110001);

        __m256 mask = _mm256_cmp_ps(reg0, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask1));
        reg0 = _mm256_blendv_ps(shuffled_reg, reg0, mask);
    }

    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg1, reg1, 0b01001110);
        __m256 mask = _mm256_cmp_ps(reg1, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask2));
        reg1 = _mm256_blendv_ps(shuffled_reg, reg1, mask);
    }
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg1, reg1, 0b10110001);
        __m256 mask = _mm256_cmp_ps(reg1, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask1));
        reg1 = _mm256_blendv_ps(shuffled_reg, reg1, mask);
    }
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
inline void bitonic_sort(__m256 &reg0, __m256 &reg1, __m256 &reg2,
                         __m256 &reg3) {

    // sort each quarter
    bitonic_sort(reg0);
    bitonic_sort(reg1);
    bitonic_sort(reg2);
    bitonic_sort(reg3);
    // sort each half
    bitonic_sort(reg0, reg1);
    bitonic_sort(reg2, reg3);

    // sort full width
    { // reverse lover half and compare to upper half
        __m256 reversed_halves0 =
            _mm256_permute2f128_ps(reg0, reg0, 0b00000001);
        __m256 reversed0 =
            _mm256_shuffle_ps(reversed_halves0, reversed_halves0, 0b00011011);
        reg0 = _mm256_min_ps(reversed0, reg3);
        reg3 = _mm256_max_ps(reversed0, reg3);

        __m256 reversed_halves1 =
            _mm256_permute2f128_ps(reg1, reg1, 0b00000001);
        __m256 reversed1 =
            _mm256_shuffle_ps(reversed_halves1, reversed_halves1, 0b00011011);
        reg1 = _mm256_min_ps(reversed1, reg2);
        reg2 = _mm256_max_ps(reversed1, reg2);
    }
    {
        __m256 max = _mm256_max_ps(reg1, reg3);
        __m256 min = _mm256_min_ps(reg1, reg3);
        reg1 = min;
        reg3 = max;
        max = _mm256_max_ps(reg2, reg0);
        min = _mm256_min_ps(reg2, reg0);
        reg2 = max;
        reg0 = min;
    }
    {
        __m256 max = _mm256_max_ps(reg0, reg1);
        __m256 min = _mm256_min_ps(reg0, reg1);
        reg0 = min;
        reg1 = max;
        max = _mm256_max_ps(reg2, reg3);
        min = _mm256_min_ps(reg2, reg3);
        reg2 = min;
        reg3 = max;
    }
    __m256i mask_epi32 =
        _mm256_set_epi32(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                         0x00000000, 0x00000000, 0x00000000, 0x00000000);
    { // mix across avx lanes
        __m256 shuffled_reg = _mm256_permute2f128_ps(reg0, reg0, 0b00000001);
        __m256 mask = _mm256_cmp_ps(reg0, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg0 = _mm256_blendv_ps(shuffled_reg, reg0, mask);
    }
    { // mix across avx lanes
        __m256 shuffled_reg = _mm256_permute2f128_ps(reg1, reg1, 0b00000001);
        __m256 mask = _mm256_cmp_ps(reg1, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg1 = _mm256_blendv_ps(shuffled_reg, reg1, mask);
    }
    { // mix across avx lanes
        __m256 shuffled_reg = _mm256_permute2f128_ps(reg2, reg2, 0b00000001);
        __m256 mask = _mm256_cmp_ps(reg2, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg2 = _mm256_blendv_ps(shuffled_reg, reg2, mask);
    }
    { // mix across avx lanes
        __m256 shuffled_reg = _mm256_permute2f128_ps(reg3, reg3, 0b00000001);
        __m256 mask = _mm256_cmp_ps(reg3, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg3 = _mm256_blendv_ps(shuffled_reg, reg3, mask);
    }

    // mix inside avx lanes each register separately, but jump single value
    mask_epi32 =
        _mm256_set_epi32(0xffffffff, 0xffffffff, 0x00000000, 0x00000000,
                         0xffffffff, 0xffffffff, 0x00000000, 0x00000000);
    {
        // register 0:  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg0, reg0, 0b01001110);
        __m256 mask = _mm256_cmp_ps(reg0, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg0 = _mm256_blendv_ps(shuffled_reg, reg0, mask);
    }

    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg1, reg1, 0b01001110);
        __m256 mask = _mm256_cmp_ps(reg1, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg1 = _mm256_blendv_ps(shuffled_reg, reg1, mask);
    }
    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg2, reg2, 0b01001110);
        __m256 mask = _mm256_cmp_ps(reg2, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg2 = _mm256_blendv_ps(shuffled_reg, reg2, mask);
    }
    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg3, reg3, 0b01001110);
        __m256 mask = _mm256_cmp_ps(reg3, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg3 = _mm256_blendv_ps(shuffled_reg, reg3, mask);
    }

    mask_epi32 =
        _mm256_set_epi32(0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
                         0xffffffff, 0x00000000, 0xffffffff, 0x00000000);
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg0, reg0, 0b10110001);
        __m256 mask = _mm256_cmp_ps(reg0, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg0 = _mm256_blendv_ps(shuffled_reg, reg0, mask);
    }
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg1, reg1, 0b10110001);
        __m256 mask = _mm256_cmp_ps(reg1, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg1 = _mm256_blendv_ps(shuffled_reg, reg1, mask);
    }
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg2, reg2, 0b10110001);
        __m256 mask = _mm256_cmp_ps(reg2, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg2 = _mm256_blendv_ps(shuffled_reg, reg2, mask);
    }
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg3, reg3, 0b10110001);
        __m256 mask = _mm256_cmp_ps(reg3, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg3 = _mm256_blendv_ps(shuffled_reg, reg3, mask);
    }
}

/**
 * @brief The function accepts four sorted  __m256 vectors and merges them.
 * @param reg3 upper vector of numbers - at the end it contains larger values,
 * the largest value is in the upper half of register [255:192]
 * @param reg2
 * @param reg1
 * @param reg0 lower vector of numbers - at the end it contains smaller values.
 * The smallest value is in the lowest half of register - at [63:0]
 *
 */
inline void bitonic_merge(__m256 &reg0, __m256 &reg1, __m256 &reg2,
                          __m256 &reg3) {
    { // reverse lover half and compare to upper half
        __m256 reversed_halves0 =
            _mm256_permute2f128_ps(reg0, reg0, 0b00000001);
        __m256 reversed0 =
            _mm256_shuffle_ps(reversed_halves0, reversed_halves0, 0b00011011);
        reg0 = _mm256_min_ps(reversed0, reg3);
        reg3 = _mm256_max_ps(reversed0, reg3);

        __m256 reversed_halves1 =
            _mm256_permute2f128_ps(reg1, reg1, 0b00000001);
        __m256 reversed1 =
            _mm256_shuffle_ps(reversed_halves1, reversed_halves1, 0b00011011);
        reg1 = _mm256_min_ps(reversed1, reg2);
        reg2 = _mm256_max_ps(reversed1, reg2);
    }
    {
        __m256 max = _mm256_max_ps(reg1, reg3);
        __m256 min = _mm256_min_ps(reg1, reg3);
        reg1 = min;
        reg3 = max;
        max = _mm256_max_ps(reg2, reg0);
        min = _mm256_min_ps(reg2, reg0);
        reg2 = max;
        reg0 = min;
    }
    {
        __m256 max = _mm256_max_ps(reg0, reg1);
        __m256 min = _mm256_min_ps(reg0, reg1);
        reg0 = min;
        reg1 = max;
        max = _mm256_max_ps(reg2, reg3);
        min = _mm256_min_ps(reg2, reg3);
        reg2 = min;
        reg3 = max;
    }
    __m256i mask_epi32 =
        _mm256_set_epi32(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                         0x00000000, 0x00000000, 0x00000000, 0x00000000);
    { // mix across avx lanes
        __m256 shuffled_reg = _mm256_permute2f128_ps(reg0, reg0, 0b00000001);
        __m256 mask = _mm256_cmp_ps(reg0, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg0 = _mm256_blendv_ps(shuffled_reg, reg0, mask);
    }
    { // mix across avx lanes
        __m256 shuffled_reg = _mm256_permute2f128_ps(reg1, reg1, 0b00000001);
        __m256 mask = _mm256_cmp_ps(reg1, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg1 = _mm256_blendv_ps(shuffled_reg, reg1, mask);
    }
    { // mix across avx lanes
        __m256 shuffled_reg = _mm256_permute2f128_ps(reg2, reg2, 0b00000001);
        __m256 mask = _mm256_cmp_ps(reg2, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg2 = _mm256_blendv_ps(shuffled_reg, reg2, mask);
    }
    { // mix across avx lanes
        __m256 shuffled_reg = _mm256_permute2f128_ps(reg3, reg3, 0b00000001);
        __m256 mask = _mm256_cmp_ps(reg3, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg3 = _mm256_blendv_ps(shuffled_reg, reg3, mask);
    }

    // mix inside avx lanes each register separately, but jump single value
    mask_epi32 =
        _mm256_set_epi32(0xffffffff, 0xffffffff, 0x00000000, 0x00000000,
                         0xffffffff, 0xffffffff, 0x00000000, 0x00000000);
    {
        // register 0:  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg0, reg0, 0b01001110);
        __m256 mask = _mm256_cmp_ps(reg0, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg0 = _mm256_blendv_ps(shuffled_reg, reg0, mask);
    }

    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg1, reg1, 0b01001110);
        __m256 mask = _mm256_cmp_ps(reg1, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg1 = _mm256_blendv_ps(shuffled_reg, reg1, mask);
    }
    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg2, reg2, 0b01001110);
        __m256 mask = _mm256_cmp_ps(reg2, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg2 = _mm256_blendv_ps(shuffled_reg, reg2, mask);
    }
    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg3, reg3, 0b01001110);
        __m256 mask = _mm256_cmp_ps(reg3, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg3 = _mm256_blendv_ps(shuffled_reg, reg3, mask);
    }

    mask_epi32 =
        _mm256_set_epi32(0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
                         0xffffffff, 0x00000000, 0xffffffff, 0x00000000);
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg0, reg0, 0b10110001);
        __m256 mask = _mm256_cmp_ps(reg0, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg0 = _mm256_blendv_ps(shuffled_reg, reg0, mask);
    }
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg1, reg1, 0b10110001);
        __m256 mask = _mm256_cmp_ps(reg1, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg1 = _mm256_blendv_ps(shuffled_reg, reg1, mask);
    }
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg2, reg2, 0b10110001);
        __m256 mask = _mm256_cmp_ps(reg2, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg2 = _mm256_blendv_ps(shuffled_reg, reg2, mask);
    }
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg3, reg3, 0b10110001);
        __m256 mask = _mm256_cmp_ps(reg3, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_ps(
            _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
        reg3 = _mm256_blendv_ps(shuffled_reg, reg3, mask);
    }
}

//////////////////////////////////////////////////////////////////
// 2^N FLOAT ARRAY SORTING ALGORITHM

// float implementation
/** @brief compared vectors from top and bottom of array and then gradually
 * compare inner vectors.
 * @param arr pointer to the float array to be sorted
 * @param start index of the first element to be sorted
 * @param end index of the last element to be sorted
 */
inline void compare_full_length(float *arr, unsigned start, unsigned end) {
    unsigned length = end - start + 1;
    unsigned half = length / 2;
    for (int i = 0; i < half; i += 8) {
        float *p1 = arr + start + i;
        float *p2 = arr + end - 7 - i;
        { // reverse lover half and compare to upper half
            __m256 vec1 = _mm256_load_ps(p1);
            __m256 vec2 = _mm256_load_ps(p2);

            __m256 reversed_halves =
                _mm256_permute2f128_ps(vec1, vec1, 0b00000001);
            __m256 reversed =
                _mm256_shuffle_ps(reversed_halves, reversed_halves, 0b00011011);
            vec1 = _mm256_min_ps(reversed, vec2);
            vec2 = _mm256_max_ps(reversed, vec2);
            _mm256_store_ps(p1, vec1);
            _mm256_store_ps(p2, vec2);
        }
    }
}

/** @brief compared vectors from top and bottom of array and then gradually
 * compare inner vectors.
 * @param arr pointer to the array to be sorted
 * @param start index of the first element to be sorted
 * @param end index of the last element to be sorted
 * @param depth a parameter to follow the depth of recursion
 */
inline void lane_crossing_compare(float *arr, unsigned start, unsigned end,
                                  unsigned depth) {
    unsigned length = end - start + 1;
    if (length == 8) {
        __m256 reg = _mm256_load_ps(arr + start);
        // this is the ending case do single vector permutations
        {
            // here one compares:
            // 0-4, 1-5, 2-6, 3-7 -> reverse halves
            __m256 reversed_halves =
                _mm256_permute2f128_ps(reg, reg, 0b00000001);
            __m256 mask = _mm256_cmp_ps(reg, reversed_halves, _CMP_LE_OQ);

            __m256i mask_epi32 = _mm256_set_epi32(
                0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000,
                0x00000000, 0x00000000, 0x00000000);

            mask = _mm256_castsi256_ps(
                _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));

            reg = _mm256_blendv_ps(reversed_halves, reg, mask);
        }
        {
            //  here one compares:
            // 0-2, 1-3, 4-6, 5-7 -> you have to mix elements inside halves
            __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b01001110);

            __m256 mask = _mm256_cmp_ps(reg, shuffled_reg, _CMP_LE_OQ);

            __m256i mask_epi32 = _mm256_set_epi32(
                0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0xffffffff,
                0xffffffff, 0x00000000, 0x00000000);

            mask = _mm256_castsi256_ps(
                _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));

            reg = _mm256_blendv_ps(shuffled_reg, reg, mask);
        }
        {
            // and finally repeat the first step: *--*  *--*  *--* *--*
            __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b10110001);
            __m256 mask = _mm256_cmp_ps(reg, shuffled_reg, _CMP_LE_OQ);

            __m256i mask_epi32 = _mm256_set_epi32(
                0xffffffff, 0x00000000, 0xffffffff, 0x00000000, 0xffffffff,
                0x00000000, 0xffffffff, 0x00000000);

            mask = _mm256_castsi256_ps(
                _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));

            reg = _mm256_blendv_ps(shuffled_reg, reg, mask);

            _mm256_store_ps(arr + start, reg);
        }
        return;
    }
    float *p = arr + start;
    for (unsigned i = 0; i < length / 2; i += 8) {
        {
            float *p1 = p + i;
            float *p2 = p + length / 2 + i;
            __m256 reg0 = _mm256_load_ps(p1); // i-ti od začetka
            __m256 reg1 = _mm256_load_ps(p2); // ta je prvi čez polovico
            // register 2 vsebuje min vrednosti
            __m256 min = _mm256_min_ps(reg1, reg0);
            // register 1 vsebuje max vrednosti
            reg1 = _mm256_max_ps(reg1, reg0);
            reg0 = min;
            _mm256_store_ps(p1, reg0);
            _mm256_store_ps(p2, reg1);
        }
    }
    lane_crossing_compare(arr, start, (start + end) / 2, depth + 1);
    lane_crossing_compare(arr, (start + end) / 2 + 1, end, depth + 1);
};

/**
 * @brief The function sorts an array with 8*n elements
 * @param array pointer to the start of the array
 * @param start index of the first number to be sorted
 * @param end index of the last number to be sorted
 * @details end-start+1 should be 8*N. If it's not
 * then add additional elements - like infinity, so that it will be large enough
 */
inline void sort_2n_vector(float *array, unsigned start, unsigned end) {

    unsigned full_length = end - start + 1; // number of double numbers to sort
    unsigned vec_count = full_length / 8;
    assert((full_length >= 0 && !(full_length & (full_length - 1))) &&
           "The array to be sorted is not the power of 2!");

    //__m256d *arr = avx_vec.data();
    if (full_length == 8) {

        __m256 vec = _mm256_load_ps(array);
        bitonic_sort(vec);
        _mm256_store_ps(array, vec);

    } else if (full_length == 16) {

        __m256 vec1 = _mm256_load_ps(array);
        __m256 vec2 = _mm256_load_ps(array + 8);
        bitonic_sort(vec1, vec2);
        _mm256_store_ps(array, vec1);
        _mm256_store_ps(array + 8, vec2);

    } else if (full_length == 32) {

        __m256 vec1 = _mm256_load_ps(array);
        __m256 vec2 = _mm256_load_ps(array + 8);
        __m256 vec3 = _mm256_load_ps(array + 16);
        __m256 vec4 = _mm256_load_ps(array + 24);
        bitonic_sort(vec1, vec2, vec3, vec4);
        _mm256_store_ps(array, vec1);
        _mm256_store_ps(array + 8, vec2);
        _mm256_store_ps(array + 16, vec3);
        _mm256_store_ps(array + 24, vec4);

    } else if (full_length >= 64) {
        for (unsigned i = start; i < end; i += 8) {
            __m256 vec1 = _mm256_load_ps(array + i);
            bitonic_sort(vec1);
            _mm256_store_ps(array + i, vec1);
        }

        // outer loop
        // len is number of floats in length to be compared
        // each step increases this length by factor of 2.
        for (unsigned len = 16; len <= full_length; len *= 2) {
            // std::cout << "len: " << len << std::endl;
            // inner loop goes over all subdivisions
            for (unsigned n = 0; n < full_length; n += len) {
                compare_full_length(array, n, n + len - 1);
                lane_crossing_compare(array, n, n + len - 1, 0);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////
// FLOAT ARRAY SORTING ALGORITHM, the array must contain 8n elements

// float implementation
/** @brief compared vectors from top and bottom of array and then gradually
 * compare inner vectors.
 * @param arr pointer to the float array to be sorted
 * @param start index of the first element to be sorted
 * @param end index of the last element to be sorted
 * @param last_index last index of array to be sorted, maximum is array size - 1
 */
inline void compare_full_length(float *arr, unsigned start, unsigned end,
                                unsigned last_index) {
    unsigned length = end - start + 1;
    unsigned half = length / 2; // half je index prvega cez polovico
    for (int i = half - 8; i >= 0; i -= 8) {
        if (end - 7 - i > last_index)
            break;
        // std::cout << "start: " << start << std::endl;
        // std::cout << "compare_full_length: " << start + i << "  " << end - 7
        // - i
        //          << std::endl;
        float *p1 = arr + start + i;
        float *p2 = arr + end - 7 - i;

        { // reverse lover half and compare to upper half
            __m256 vec1 = _mm256_load_ps(p1);
            __m256 vec2 = _mm256_load_ps(p2);
            __m256 reversed_halves =
                _mm256_permute2f128_ps(vec1, vec1, 0b00000001);
            __m256 reversed =
                _mm256_shuffle_ps(reversed_halves, reversed_halves, 0b00011011);
            vec1 = _mm256_min_ps(reversed, vec2);
            vec2 = _mm256_max_ps(reversed, vec2);
            reversed_halves = _mm256_permute2f128_ps(vec1, vec1, 0b00000001);
            vec1 =
                _mm256_shuffle_ps(reversed_halves, reversed_halves, 0b00011011);
            _mm256_store_ps(p1, vec1);
            _mm256_store_ps(p2, vec2);
            // print_avx(vec1, "vec1: ");
            // print_avx(vec2, "vec2: ");
        }
    }
}

/** @brief compared vectors from top and bottom of array and then gradually
 * compare inner vectors.
 * @param arr pointer to the array to be sorted
 * @param start index of the first element to be sorted
 * @param end index of the last element to be sorted
 * @param last_index last index of array to be sorted, maximum is array size - 1
 * @param depth a parameter to follow the depth of recursion
 */
inline void lane_crossing_compare(float *arr, unsigned start, unsigned end,
                                  unsigned last_index, unsigned depth) {

    if (start > last_index) {
        // std::cout << "aborted start index: " << start << std::endl;
        return;
    }
    unsigned length = end - start + 1;
    if (length == 8) {
        __m256 reg = _mm256_load_ps(arr + start);
        // this is the ending case do single vector permutations
        {
            // here one compares:
            // 0-4, 1-5, 2-6, 3-7 -> reverse halves
            __m256 reversed_halves =
                _mm256_permute2f128_ps(reg, reg, 0b00000001);
            __m256 mask = _mm256_cmp_ps(reg, reversed_halves, _CMP_LE_OQ);

            __m256i mask_epi32 = _mm256_set_epi32(
                0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000,
                0x00000000, 0x00000000, 0x00000000);

            mask = _mm256_castsi256_ps(
                _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));

            reg = _mm256_blendv_ps(reversed_halves, reg, mask);
        }
        {
            //  here one compares:
            // 0-2, 1-3, 4-6, 5-7 -> you have to mix elements inside halves
            __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b01001110);

            __m256 mask = _mm256_cmp_ps(reg, shuffled_reg, _CMP_LE_OQ);

            __m256i mask_epi32 = _mm256_set_epi32(
                0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0xffffffff,
                0xffffffff, 0x00000000, 0x00000000);

            mask = _mm256_castsi256_ps(
                _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));

            reg = _mm256_blendv_ps(shuffled_reg, reg, mask);
        }
        {
            // and finally repeat the first step: *--*  *--*  *--* *--*
            __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b10110001);
            __m256 mask = _mm256_cmp_ps(reg, shuffled_reg, _CMP_LE_OQ);

            __m256i mask_epi32 = _mm256_set_epi32(
                0xffffffff, 0x00000000, 0xffffffff, 0x00000000, 0xffffffff,
                0x00000000, 0xffffffff, 0x00000000);

            mask = _mm256_castsi256_ps(
                _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));

            reg = _mm256_blendv_ps(shuffled_reg, reg, mask);

            _mm256_store_ps(arr + start, reg);
        }
        return;
    }
    float *p = arr + start;
    // for (unsigned i = 0; i < length / 2; i += 8) {
    for (unsigned i = 0; i < length / 2; i += 8) {
        if (start + length / 2 + i > last_index)
            break;
        {
            // std::cout << "shuffling vecs: " << start + i << " "
            //         << start + length / 2 + i << std::endl;
            float *p1 = p + i;
            float *p2 = p + length / 2 + i;
            __m256 reg0 = _mm256_load_ps(p1); // i-ti od začetka
            __m256 reg1 = _mm256_load_ps(p2); // ta je prvi čez polovico
            // register 2 vsebuje min vrednosti
            __m256 min = _mm256_min_ps(reg1, reg0);
            // register 1 vsebuje max vrednosti
            reg1 = _mm256_max_ps(reg1, reg0);
            reg0 = min;
            // print_avx(reg0, "reg0: ");
            // print_avx(reg1, "reg1: ");
            // std::cout << "\n";
            _mm256_store_ps(p1, reg0);
            _mm256_store_ps(p2, reg1);
        }
    }
    lane_crossing_compare(arr, start, (start + end) / 2, last_index, depth + 1);
    lane_crossing_compare(arr, (start + end) / 2 + 1, end, last_index,
                          depth + 1);
};

/**
 * @brief The function sorts an array with 2^n elements
 * @param array pointer to the start of the array
 * @param start index of the first number to be sorted
 * @param end index of the last number to be sorted
 * @details end-start+1 should be 8*n
 */
inline void sort_8n_vector(float *array, unsigned start, unsigned end) {

    //    unsigned full_length = end - start + 1; // number of double
    //    numbers to sort
    int pow2 = (int)std::ceil(std::log2f(end + 1));
    int imaginary_length = (int)std::pow(2, pow2);
    unsigned full_length = end - start + 1;
    unsigned last_index = end - 7; // last index to be loaded
    // std::cout << "imaginary_length: " << imaginary_length << std::endl;
    unsigned vec_count = full_length / 8;
    assert((full_length >= 0 && (full_length % 8) == 0) &&
           "The array to be sorted is not a multiples of 8!");

    //__m256d *arr = avx_vec.data();
    // if (full_length < 8) {
    //    std::sort(array, array + full_length);
    // } else
    if (full_length == 8) {
        __m256 vec = _mm256_load_ps(array);
        bitonic_sort(vec);
        _mm256_store_ps(array, vec);
    } else if (full_length == 16) {
        __m256 vec1 = _mm256_load_ps(array);
        __m256 vec2 = _mm256_load_ps(array + 8);
        bitonic_sort(vec1, vec2);
        _mm256_store_ps(array, vec1);
        _mm256_store_ps(array + 8, vec2);

    } else if (full_length == 32) {
        __m256 vec1 = _mm256_load_ps(array);
        __m256 vec2 = _mm256_load_ps(array + 8);
        __m256 vec3 = _mm256_load_ps(array + 16);
        __m256 vec4 = _mm256_load_ps(array + 24);
        bitonic_sort(vec1, vec2, vec3, vec4);
        _mm256_store_ps(array, vec1);
        _mm256_store_ps(array + 8, vec2);
        _mm256_store_ps(array + 16, vec3);
        _mm256_store_ps(array + 24, vec4);

    } else {
        for (unsigned i = start; i < end; i += 8) {
            __m256 vec1 = _mm256_load_ps(array + i);
            bitonic_sort(vec1);
            _mm256_store_ps(array + i, vec1);
        }

        // outer loop
        // len is number of floats in length to be compared
        // each step increases this length by factor of 2.
        for (unsigned len = 16; len <= imaginary_length; len *= 2) {
            // std::cout << "len: " << len << std::endl;
            // inner loop goes over all subdivisions
            for (unsigned n = 0; n < imaginary_length; n += len) {
                compare_full_length(array, n, n + len - 1, last_index);
                lane_crossing_compare(array, n, n + len - 1, last_index, 0);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////
// FLOAT ARRAY SORTING ALGORITHM FOR ARRAYS OF ARBITRARY LENGTH, the array must
// contain 8n elements
// float implementation

inline void maskload(int diff, float *p2, __m256i &mask, __m256 &reg1) {
    switch (diff) {
    case 0: {
        mask = _mm256_set_epi32(0, 0, 0, 0, 0, 0, 0, LOAD);
        reg1 = _mm256_maskload_ps(p2, mask);
        __m256 infinity =
            _mm256_set1_ps(std::numeric_limits<float>::infinity());
        reg1 = _mm256_blend_ps(infinity, reg1, 0b00000001);
    } break;
    case 1: {
        mask = _mm256_set_epi32(0, 0, 0, 0, 0, 0, LOAD, LOAD);
        reg1 = _mm256_maskload_ps(p2, mask);
        __m256 infinity =
            _mm256_set1_ps(std::numeric_limits<float>::infinity());
        reg1 = _mm256_blend_ps(infinity, reg1, 0b00000011);
    } break;
    case 2: {
        mask = _mm256_set_epi32(0, 0, 0, 0, 0, LOAD, LOAD, LOAD);
        reg1 = _mm256_maskload_ps(p2, mask);
        __m256 infinity =
            _mm256_set1_ps(std::numeric_limits<float>::infinity());
        reg1 = _mm256_blend_ps(infinity, reg1, 0b00000111);
    } break;
    case 3: {
        mask = _mm256_set_epi32(0, 0, 0, 0, LOAD, LOAD, LOAD, LOAD);
        reg1 = _mm256_maskload_ps(p2, mask);
        __m256 infinity =
            _mm256_set1_ps(std::numeric_limits<float>::infinity());
        reg1 = _mm256_blend_ps(infinity, reg1, 0b00001111);
    } break;
    case 4: {
        mask = _mm256_set_epi32(0, 0, 0, LOAD, LOAD, LOAD, LOAD, LOAD);
        reg1 = _mm256_maskload_ps(p2, mask);
        __m256 infinity =
            _mm256_set1_ps(std::numeric_limits<float>::infinity());
        reg1 = _mm256_blend_ps(infinity, reg1, 0b00011111);
    } break;
    case 5: {
        mask = _mm256_set_epi32(0, 0, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD);
        reg1 = _mm256_maskload_ps(p2, mask);
        __m256 infinity =
            _mm256_set1_ps(std::numeric_limits<float>::infinity());
        reg1 = _mm256_blend_ps(infinity, reg1, 0b00111111);
    } break;
    case 6: {
        mask = _mm256_set_epi32(0, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD);
        reg1 = _mm256_maskload_ps(p2, mask);
        __m256 infinity =
            _mm256_set1_ps(std::numeric_limits<float>::infinity());
        reg1 = _mm256_blend_ps(infinity, reg1, 0b01111111);
    } break;
        //    default:
        // reg1 = _mm256_load_ps(p2);
    };
}

/** @brief compared vectors from top and bottom of array and then gradually
 * compare inner vectors.
 * @param arr pointer to the float array to be sorted
 * @param start index of the first element to be sorted
 * @param end index of the last element to be sorted
 */
inline void compare_full_length_all_cases(float *arr, unsigned start,
                                          unsigned end, unsigned last_index) {
    // std::cout << "start: " << start << "end: " << end
    //          << " last index: " << last_index << std::endl;
    unsigned length = end - start + 1;
    unsigned half = length / 2; // half je index prvega cez polovico
    for (int i = half - 8; i >= 0; i -= 8) {
        // index of last vector to load
        int last_vec_to_load = end - 7 - i;
        int diff = last_index - last_vec_to_load;
        if (UNLIKELY(diff < 0))
            return;
        // define pointers to the start of vectors to load
        float *p1 = arr + start + i;
        float *p2 = arr + last_vec_to_load;
        __m256 vec2;
        __m256i mask;
        if (UNLIKELY(diff < 7))
            maskload(diff, p2, mask, vec2);
        else
            vec2 = _mm256_load_ps(p2);
        { // reverse lover half and compare to upper half
            __m256 vec1 = _mm256_load_ps(p1);
            __m256 reversed_halves =
                _mm256_permute2f128_ps(vec1, vec1, 0b00000001);
            __m256 reversed =
                _mm256_shuffle_ps(reversed_halves, reversed_halves, 0b00011011);
            vec1 = _mm256_min_ps(reversed, vec2);
            vec2 = _mm256_max_ps(reversed, vec2);
            reversed_halves = _mm256_permute2f128_ps(vec1, vec1, 0b00000001);
            vec1 =
                _mm256_shuffle_ps(reversed_halves, reversed_halves, 0b00011011);
            _mm256_store_ps(p1, vec1);
            if (UNLIKELY(diff <= 6))
                _mm256_maskstore_ps(p2, mask, vec2);
            else
                _mm256_store_ps(p2, vec2);
        }
    }
}

/** @brief compared vectors from top and bottom of array and then gradually
 * compare inner vectors.
 * @param arr pointer to the array to be sorted
 * @param start index of the first element to be sorted
 * @param end index of the last element to be sorted
 * @param depth a parameter to follow the depth of recursion
 */
inline void lane_crossing_compare_all_cases(float *arr, unsigned start,
                                            unsigned end, unsigned last_index,
                                            unsigned depth) {
    if (start > last_index) {
        return;
    }
    unsigned length = end - start + 1;
    if (length == 8) {
        int diff = last_index - start;
        if (diff < 1)
            return;
        __m256 reg;
        __m256i load_store_mask;
        if (diff < 7)
            maskload(diff, arr + start, load_store_mask, reg);
        else
            reg = _mm256_load_ps(arr + start);
        {
            // here one compares:
            // 0-4, 1-5, 2-6, 3-7 -> reverse halves
            __m256 reversed_halves =
                _mm256_permute2f128_ps(reg, reg, 0b00000001);
            __m256 mask = _mm256_cmp_ps(reg, reversed_halves, _CMP_LE_OQ);

            __m256i mask_epi32 = _mm256_set_epi32(
                0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000,
                0x00000000, 0x00000000, 0x00000000);
            mask = _mm256_castsi256_ps(
                _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
            reg = _mm256_blendv_ps(reversed_halves, reg, mask);
        }
        {
            //  here one compares:
            // 0-2, 1-3, 4-6, 5-7 -> you have to mix elements inside halves
            __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b01001110);

            __m256 mask = _mm256_cmp_ps(reg, shuffled_reg, _CMP_LE_OQ);

            __m256i mask_epi32 = _mm256_set_epi32(
                0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0xffffffff,
                0xffffffff, 0x00000000, 0x00000000);

            mask = _mm256_castsi256_ps(
                _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));

            reg = _mm256_blendv_ps(shuffled_reg, reg, mask);
        }
        {
            // and finally repeat the first step: *--*  *--*  *--* *--*
            __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b10110001);
            __m256 mask = _mm256_cmp_ps(reg, shuffled_reg, _CMP_LE_OQ);

            __m256i mask_epi32 = _mm256_set_epi32(
                0xffffffff, 0x00000000, 0xffffffff, 0x00000000, 0xffffffff,
                0x00000000, 0xffffffff, 0x00000000);

            mask = _mm256_castsi256_ps(
                _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));

            reg = _mm256_blendv_ps(shuffled_reg, reg, mask);

            if (diff < 7)
                _mm256_maskstore_ps(arr + start, load_store_mask, reg);

            else
                _mm256_store_ps(arr + start, reg);
        }

        return;
    }

    float *p = arr + start;
    for (unsigned i = 0; i < length / 2; i += 8) {
        int diff = last_index - (start + length / 2 + i);
        if (UNLIKELY(diff < 0))
            break;
        __m256 reg1;
        __m256i mask;
        float *p2 = p + length / 2 + i;
        if (UNLIKELY(diff < 7))
            maskload(diff, p2, mask, reg1);
        else
            reg1 = _mm256_load_ps(p2);
        float *p1 = p + i;
        __m256 reg0 = _mm256_load_ps(p1); // i-ti od začetka
        // register 2 vsebuje min vrednosti
        __m256 min = _mm256_min_ps(reg1, reg0);
        // register 1 vsebuje max vrednosti
        reg1 = _mm256_max_ps(reg1, reg0);
        reg0 = min;
        _mm256_store_ps(p1, reg0);
        if (UNLIKELY(diff < 7))
            _mm256_maskstore_ps(p2, mask, reg1);
        else
            _mm256_store_ps(p2, reg1);
    }
    lane_crossing_compare_all_cases(arr, start, (start + end) / 2, last_index,
                                    depth + 1);
    lane_crossing_compare_all_cases(arr, (start + end) / 2 + 1, end, last_index,
                                    depth + 1);
};

/**
 * @brief The function sorts an array with 2^n elements
 * @param array pointer to the start of the array
 * @param start index of the first number to be sorted
 * @param end index of the last number to be sorted
 * @details end-start+1 should be 2^n
 */
inline void sort_vector(aligned_vector<float> &array, unsigned start,
                        unsigned end) {

    unsigned full_length = end - start + 1;
    if (full_length <= 1)
        return;
    else if (full_length < 8) {
        __m256i mask;
        __m256 reg;
        maskload(full_length - 1, array.data(), mask, reg);
        bitonic_sort(reg);
        _mm256_maskstore_ps(array.data(), mask, reg);
    } else if (!(full_length & (full_length - 1)))
        sort_2n_vector(array.data(), start, end);
    else if (mod8(full_length) == 0)
        sort_8n_vector(array.data(), start, end);
    else if (full_length < 16) {
        __m256i mask;
        __m256 reg1, reg2;
        reg1 = _mm256_load_ps(array.data());
        maskload(full_length - 9, array.data() + 8, mask, reg2);
        bitonic_sort(reg1, reg2);
        _mm256_store_ps(array.data(), reg1);
        _mm256_maskstore_ps(array.data() + 8, mask, reg2);
    } else {
        int pow2 = (int)std::ceil(std::log2f(end + 1));
        int imaginary_length = (int)std::pow(2, pow2);
        unsigned full_length = end - start + 1;
        unsigned last_index = end;

        for (unsigned i = start; i <= end - 7; i += 8) {
            __m256 vec1 = _mm256_load_ps(array.data() + i);
            bitonic_sort(vec1);
            _mm256_store_ps(array.data() + i, vec1);
        }
        ///////////////////////////////// load the partial one
        int reminder = mod8(end);
        float *p = array.data() + end - reminder;
        __m256 reg1;
        __m256i mask;
        maskload(reminder, p, mask, reg1);
        bitonic_sort(reg1);
        _mm256_maskstore_ps(array.data() + end - reminder, mask, reg1);

        ///////////////////////////////////////////////////////

        // outer loop
        // len is number of floats in length to be compared
        // each step increases this length by factor of 2.
        // 8 and less has already been done above
        // for (unsigned len = 16; len <= imaginary_length; len *= 2) {
        for (unsigned len = 16; len <= imaginary_length; len *= 2) {
            // std::cout << "len: " << len << std::endl;
            // inner loop goes over all subdivisions
            for (unsigned n = 0; n < imaginary_length; n += len) {
                compare_full_length_all_cases(array.data(), n, n + len - 1,
                                              last_index);
                lane_crossing_compare_all_cases(array.data(), n, n + len - 1,
                                                last_index, 0);
            }
        }
    }
}
#endif

/////////////////////////////////////////////////////////////////////////
// double implementation
 
#ifndef __BITONIC_SORT_DOUBLE__
#define __BITONIC_SORT_DOUBLE__
#endif
/**
 *@brief The function accepts a single __m256d vector and sorts
 *it.
 * @param reg register to be sorted
 */
inline void bitonic_sort(__m256d &reg) {
    {
        __m256d shuffled_reg = _mm256_shuffle_pd(reg, reg, 0b0101);
        __m256d mask = _mm256_cmp_pd(reg, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_pd(_mm256_xor_si256(
            _mm256_castpd_si256(mask),
            _mm256_set_epi64x(0xffffffffffffffff, 0x0000000000000000,
                              0xffffffffffffffff, 0x0000000000000000)));
        // ___m256d max = _mm256_max_pd(reg, shuffled_reg);
        // __m256d min = _mm256_min_pd(reg, shuffled_reg);
        //   dst[63:0] := src1[63:0]
        //   dst[127:64] := src2[63:0]
        //   dst[192:128] := src1[192:128]
        //   dst[255:192] := src2[192:128]
        // this will produce smallest number to in the [0:63]
        // register
        reg = _mm256_blendv_pd(shuffled_reg, reg, mask);
    }
    {
        __m256d shuffled_reg =
            _mm256_permute4x64_pd(reg, _MM_SHUFFLE(0, 1, 2, 3));
        __m256d mask = _mm256_cmp_pd(reg, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_pd(_mm256_xor_si256(
            _mm256_castpd_si256(mask),
            _mm256_set_epi64x(0xffffffffffffffff, 0xffffffffffffffff,
                              0x0000000000000000, 0x0000000000000000)));
        reg = _mm256_blendv_pd(shuffled_reg, reg, mask);
    }
    {
        __m256d shuffled_reg = _mm256_shuffle_pd(reg, reg, 0b0101);
        __m256d mask = _mm256_cmp_pd(reg, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_pd(_mm256_xor_si256(
            _mm256_castpd_si256(mask),
            _mm256_set_epi64x(0xffffffffffffffff, 0x0000000000000000,
                              0xffffffffffffffff, 0x0000000000000000)));
        reg = _mm256_blendv_pd(shuffled_reg, reg, mask);
    }
}

/**
 * @brief The function accepts two __m256d vectors and sorts them.
 * @param reg1 upper vector of numbers - at the end it contains larger
 * values, the largest value is in the upper half of register [255:192]
 *
 * @param reg0 lower vector of numbers - at the end it contains smaller
 * values. The smallest value is in the lowest half of register - at
 * [63:0]
 *
 */
inline void bitonic_sort(__m256d &reg0, __m256d &reg1) {
    bitonic_sort(reg1); // sort first register
    bitonic_sort(reg0); // sort second register
    // print_avx(reg0, "reg0: ");
    // print_avx(reg1, "reg1: ");
    // std::cout << std::endl;
    {
        // reverse one of registers register reg0
        __m256d reverse2 = _mm256_permute4x64_pd(reg0, _MM_SHUFFLE(0, 1, 2, 3));
        // register 2 vsebuje min vrednosti
        reg0 = _mm256_min_pd(reg1, reverse2);
        // register 1 vsebuje max vrednosti
        reg1 = _mm256_max_pd(reg1, reverse2);
        // print_avx(max, "max: ");
        // print_avx(min, "min: ");
    }
    // print_avx(reg0, "reg0: ");
    // print_avx(reg1, "reg1: ");
    // std::cout << std::endl;
    {
        // tu narediš *----* *----*
        __m256d shuffled_reg1 = _mm256_permute4x64_pd(reg1, 0b01001110);
        __m256d mask = _mm256_cmp_pd(reg1, shuffled_reg1, _CMP_LE_OQ);
        mask = _mm256_castsi256_pd(_mm256_xor_si256(
            _mm256_castpd_si256(mask),
            _mm256_set_epi64x(0xffffffffffffffff, 0xffffffffffffffff,
                              0x0000000000000000, 0x0000000000000000)));
        reg1 = _mm256_blendv_pd(shuffled_reg1, reg1, mask);
    }
    // print_avx(reg0, "reg0: ");
    // print_avx(reg1, "reg1: ");
    // std::cout << std::endl;
    {
        __m256d shuffled_reg1 = _mm256_shuffle_pd(reg1, reg1, 0b0101);
        __m256d mask = _mm256_cmp_pd(reg1, shuffled_reg1, _CMP_LE_OQ);
        mask = _mm256_castsi256_pd(_mm256_xor_si256(
            _mm256_castpd_si256(mask),
            _mm256_set_epi64x(0xffffffffffffffff, 0x0000000000000000,
                              0xffffffffffffffff, 0x0000000000000000)));
        reg1 = _mm256_blendv_pd(shuffled_reg1, reg1, mask);
    }
    // print_avx(reg0, "reg0: ");
    // print_avx(reg1, "reg1: ");
    // std::cout << std::endl;
    {
        // tu narediš *----* *----*
        __m256d shuffled_reg0 = _mm256_permute4x64_pd(reg0, 0b01001110);
        __m256d mask = _mm256_cmp_pd(reg0, shuffled_reg0, _CMP_LE_OQ);
        mask = _mm256_castsi256_pd(_mm256_xor_si256(
            _mm256_castpd_si256(mask),
            _mm256_set_epi64x(0xffffffffffffffff, 0xffffffffffffffff,
                              0x0000000000000000, 0x0000000000000000)));
        reg0 = _mm256_blendv_pd(shuffled_reg0, reg0, mask);
    }
    // print_avx(reg0, "reg0: ");
    // print_avx(reg1, "reg1: ");
    // std::cout << std::endl;
    {
        __m256d shuffled_reg0 = _mm256_shuffle_pd(reg0, reg0, 0b0101);
        __m256d mask = _mm256_cmp_pd(reg0, shuffled_reg0, _CMP_LE_OQ);
        mask = _mm256_castsi256_pd(_mm256_xor_si256(
            _mm256_castpd_si256(mask),
            _mm256_set_epi64x(0xffffffffffffffff, 0x0000000000000000,
                              0xffffffffffffffff, 0x0000000000000000)));
        reg0 = _mm256_blendv_pd(shuffled_reg0, reg0, mask);
    }
    // print_avx(reg0, "reg0: ");
    // print_avx(reg1, "reg1: ");
    // std::cout << std::endl;
    return;
}

/**
 * @brief The function accepts two __m256d vectors and sorts them.
 * @param reg1 upper vector of numbers - at the end it contains larger
 * values, the largest value is in the upper half of register [255:192]
 *
 * @param reg0 lower vector of numbers - at the end it contains smaller
 * values. The smallest value is in the lowest half of register - at
 * [63:0]
 *
 */
inline void bitonic_sort(__m256d &reg0, __m256d &reg1, __m256d &reg2,
                         __m256d &reg3) {
    bitonic_sort(reg0);       // sort first register
    bitonic_sort(reg1);       // sort second register
    bitonic_sort(reg2);       // sort third register
    bitonic_sort(reg3);       // sort fourth register
    bitonic_sort(reg0, reg1); // sort third register
    bitonic_sort(reg2, reg3); // sort fourth register
    {
        // reverse one of registers register reg0
        __m256d reverse = _mm256_permute4x64_pd(reg0, _MM_SHUFFLE(0, 1, 2, 3));
        // register 2 vsebuje min vrednosti
        reg0 = _mm256_min_pd(reg3, reverse);
        // register 1 vsebuje max vrednosti
        reg3 = _mm256_max_pd(reg3, reverse);
        // print_avx(max, "max: ");
        // print_avx(min, "min: ");
    }
    {
        // reverse one of registers register reg0
        __m256d reverse = _mm256_permute4x64_pd(reg1, _MM_SHUFFLE(0, 1, 2, 3));
        // register 2 vsebuje min vrednosti
        reg1 = _mm256_min_pd(reg2, reverse);
        // register 1 vsebuje max vrednosti
        reg2 = _mm256_max_pd(reg2, reverse);
        // print_avx(max, "max: ");
        // print_avx(min, "min: ");
    }
    {
        __m256d min = _mm256_min_pd(reg3, reg1);
        __m256d max = _mm256_max_pd(reg3, reg1);
        reg1 = min;
        reg3 = max;

        min = _mm256_min_pd(reg2, reg0);
        max = _mm256_max_pd(reg2, reg0);
        reg0 = min;
        reg2 = max;
    }
    {
        __m256d min = _mm256_min_pd(reg0, reg1);
        __m256d max = _mm256_max_pd(reg0, reg1);
        reg0 = min;
        reg1 = max;

        min = _mm256_min_pd(reg2, reg3);
        max = _mm256_max_pd(reg2, reg3);
        reg2 = min;
        reg3 = max;
    }
    /*    print_avx(reg0, "reg0");
    print_avx(reg1, "reg1");
    print_avx(reg2, "reg2");
    print_avx(reg3, "reg3");*/
    __m256i mask_epi64 =
        _mm256_set_epi64x(0xffffffffffffffff, 0xffffffffffffffff,
                          0x0000000000000000, 0x0000000000000000);

    { // shuffling between 128bit lanes
        __m256d shuffled_reg = _mm256_permute4x64_pd(reg0, 0b01001110);
        __m256d mask = _mm256_cmp_pd(reg0, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_pd(
            _mm256_xor_si256(_mm256_castpd_si256(mask), mask_epi64));
        reg0 = _mm256_blendv_pd(shuffled_reg, reg0, mask);
    }
    { // shuffling between 128bit lanes
        __m256d shuffled_reg = _mm256_permute4x64_pd(reg1, 0b01001110);
        __m256d mask = _mm256_cmp_pd(reg1, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_pd(
            _mm256_xor_si256(_mm256_castpd_si256(mask), mask_epi64));
        reg1 = _mm256_blendv_pd(shuffled_reg, reg1, mask);
    }
    { // shuffling between 128bit lanes
        __m256d shuffled_reg = _mm256_permute4x64_pd(reg2, 0b01001110);
        __m256d mask = _mm256_cmp_pd(reg2, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_pd(
            _mm256_xor_si256(_mm256_castpd_si256(mask), mask_epi64));
        reg2 = _mm256_blendv_pd(shuffled_reg, reg2, mask);
    }
    { // shuffling between 128bit lanes
        __m256d shuffled_reg = _mm256_permute4x64_pd(reg3, 0b01001110);
        __m256d mask = _mm256_cmp_pd(reg3, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_pd(
            _mm256_xor_si256(_mm256_castpd_si256(mask), mask_epi64));
        reg3 = _mm256_blendv_pd(shuffled_reg, reg3, mask);
    }

    mask_epi64 = _mm256_set_epi64x(0xffffffffffffffff, 0x0000000000000000,
                                   0xffffffffffffffff, 0x0000000000000000);
    // shuffle neighbour numbers
    {
        __m256d shuffled_reg = _mm256_shuffle_pd(reg1, reg1, 0b0101);
        __m256d mask = _mm256_cmp_pd(reg1, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_pd(
            _mm256_xor_si256(_mm256_castpd_si256(mask), mask_epi64));
        reg1 = _mm256_blendv_pd(shuffled_reg, reg1, mask);
    }
    {
        __m256d shuffled_reg = _mm256_shuffle_pd(reg2, reg2, 0b0101);
        __m256d mask = _mm256_cmp_pd(reg2, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_pd(
            _mm256_xor_si256(_mm256_castpd_si256(mask), mask_epi64));
        reg2 = _mm256_blendv_pd(shuffled_reg, reg2, mask);
    }
    {
        __m256d shuffled_reg = _mm256_shuffle_pd(reg3, reg3, 0b0101);
        __m256d mask = _mm256_cmp_pd(reg3, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_pd(
            _mm256_xor_si256(_mm256_castpd_si256(mask), mask_epi64));
        reg3 = _mm256_blendv_pd(shuffled_reg, reg3, mask);
    }

    {
        __m256d shuffled_reg = _mm256_shuffle_pd(reg0, reg0, 0b0101);
        __m256d mask = _mm256_cmp_pd(reg0, shuffled_reg, _CMP_LE_OQ);
        mask = _mm256_castsi256_pd(
            _mm256_xor_si256(_mm256_castpd_si256(mask), mask_epi64));
        reg0 = _mm256_blendv_pd(shuffled_reg, reg0, mask);
    }
    return;
}

//////////////////////////////////////////////////////////////////
// 2^N DOUBLE ARRAY SORTING ALGORITHM

/** @brief compared vectors from top and bottom of array and then
 * gradually compare inner vectors.
 * @param arr pointer to the array to be sorted
 * @param start index of the first element to be sorted
 * @param end index of the last element to be sorted
 */
inline void compare_full_length(double *arr, unsigned start, unsigned end) {
    unsigned length = end - start + 1;
    unsigned half = length / 2;
    for (unsigned i = 0; i < half; i += 4) {
        double *p1 = arr + start + i;
        double *p2 = arr + end - 3 - i;

        __m256d vec1 = _mm256_load_pd(p1);
        __m256d vec2 = _mm256_load_pd(p2);
        // reverse one of registers register reg0
        __m256d reverse = _mm256_permute4x64_pd(vec1, _MM_SHUFFLE(0, 1, 2, 3));
        // register 2 vsebuje min vrednosti
        vec1 = _mm256_min_pd(vec2, reverse);
        // register 1 vsebuje max vrednosti
        vec2 = _mm256_max_pd(vec2, reverse);
        // vec1 = _mm256_permute4x64_pd(vec1, _MM_SHUFFLE(0, 1, 2,
        // 3));
        _mm256_store_pd(p1, vec1);
        _mm256_store_pd(p2, vec2);
    }
}

/** @brief compared vectors from top and bottom of array and then
 * gradually compare inner vectors.
 * @param arr pointer to the array to be sorted
 * @param start index of the first element to be sorted
 * @param end index of the last element to be sorted
 * @param depth a parameter to follow the depth of recursion
 */
inline void lane_crossing_compare(double *arr, unsigned start, unsigned end,
                                  unsigned depth) {

    unsigned length = end - start + 1;

    if (length == 4) {
        __m256d reg = _mm256_load_pd(arr + start);
        // this is the ending case do single vector permutations
        { // shuffling between 128bit lanes of 256bit register
          // (3,2,1,0)->(1,0,3,2) one compares 0 and 2 ,  1 and 3
            __m256d shuffled_reg = _mm256_permute4x64_pd(reg, 0b01001110);
            __m256d mask = _mm256_cmp_pd(reg, shuffled_reg, _CMP_LE_OQ);
            mask = _mm256_castsi256_pd(_mm256_xor_si256(
                _mm256_castpd_si256(mask),
                _mm256_set_epi64x(0xffffffffffffffff, 0xffffffffffffffff,
                                  0x0000000000000000, 0x0000000000000000)));
            reg = _mm256_blendv_pd(shuffled_reg, reg, mask);
        }
        // shuffle neighbour numbers
        { // shuffle inside 128 lanes
            // from (3,2,1,0) produce (2,3,0,1)
            __m256d shuffled_reg = _mm256_shuffle_pd(reg, reg, 0b0101);
            __m256d mask = _mm256_cmp_pd(reg, shuffled_reg, _CMP_LE_OQ);
            mask = _mm256_castsi256_pd(_mm256_xor_si256(
                _mm256_castpd_si256(mask),
                _mm256_set_epi64x(0xffffffffffffffff, 0x0000000000000000,
                                  0xffffffffffffffff, 0x0000000000000000)));
            reg = _mm256_blendv_pd(shuffled_reg, reg, mask);
            _mm256_store_pd(arr + start, reg);
        }
        return;
    }
    double *p = arr + start;
    for (unsigned i = 0; i < length / 2; i += 4) {
        {
            double *p1 = p + i;
            double *p2 = p + length / 2 + i;
            // std::cout << "i: " << i << std::endl;
            // std::cout << "length/2 + i: " << length / 2 + i <<
            // std::endl;
            __m256d reg0 = _mm256_load_pd(p1); // i-ti od začetka
            __m256d reg1 = _mm256_load_pd(p2); // ta je prvi čez polovico
            // register 2 vsebuje min vrednosti
            __m256d min = _mm256_min_pd(reg1, reg0);
            // register 1 vsebuje max vrednosti
            reg1 = _mm256_max_pd(reg1, reg0);
            reg0 = min;
            // print_avx(max, "max: ");
            // print_avx(min, "min: ");
            _mm256_store_pd(p1, reg0);
            _mm256_store_pd(p2, reg1);
        }
    }
    lane_crossing_compare(arr, start, (start + end) / 2, depth + 1);
    lane_crossing_compare(arr, (start + end) / 2 + 1, end, depth + 1);
};

/**
 * @brief The function sorts an array with 2^n elements
 * @param array pointer to the start of the array
 * @param start index of the first number to be sorted
 * @param end index of the last number to be sorted
 * @details end-start+1 should be 2^n
 */
inline void sort_2n_vector(double *array, unsigned start, unsigned end) {

    unsigned full_length = end - start + 1; // number of double numbers to sort
    unsigned vec_count = full_length / 4;
    assert((full_length >= 0 && !(full_length & (full_length - 1))) &&
           "The array to be sorted is not the power of 2!");

    //__m256d *arr = avx_vec.data();
    if (full_length == 4) {

        __m256d vec = _mm256_load_pd(array);
        bitonic_sort(vec);
        _mm256_store_pd(array, vec);

    } else if (full_length == 8) {

        __m256d vec1 = _mm256_load_pd(array);
        __m256d vec2 = _mm256_load_pd(array + 4);
        bitonic_sort(vec1, vec2);
        _mm256_store_pd(array, vec1);
        _mm256_store_pd(array + 4, vec2);

    } else if (full_length == 16) {

        __m256d vec1 = _mm256_load_pd(array);
        __m256d vec2 = _mm256_load_pd(array + 4);
        __m256d vec3 = _mm256_load_pd(array + 8);
        __m256d vec4 = _mm256_load_pd(array + 12);
        bitonic_sort(vec1, vec2, vec3, vec4);
        _mm256_store_pd(array, vec1);
        _mm256_store_pd(array + 4, vec2);
        _mm256_store_pd(array + 8, vec3);
        _mm256_store_pd(array + 12, vec4);

    } else if (full_length >= 32) {
        for (unsigned i = start; i < end; i += 4) {
            __m256d vec1 = _mm256_load_pd(array + i);
            bitonic_sort(vec1);
            _mm256_store_pd(array + i, vec1);
        }

        // outer loop
        // len is number of floats in length to be compared
        // each step increases this length by factor of 2.
        for (unsigned len = 8; len <= full_length; len *= 2) {
            // std::cout << "len: " << len << std::endl;
            // inner loop goes over all subdivisions
            for (unsigned n = 0; n < full_length; n += len) {
                compare_full_length(array, n, n + len - 1);
                lane_crossing_compare(array, n, n + len - 1, 0);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////
// 8N DOUBLE ARRAY SORTING ALGORITHM --- the array must contain 8n
// elements

/** @brief compared vectors from top and bottom of array and then
 * gradually compare inner vectors.
 * @param arr pointer to the double array to be sorted
 * @param start index of the first element to be sorted
 * @param end index of the last element to be sorted
 * @param last_index last index of array to be sorted, maximum is
 * array size - 1
 */
inline void compare_full_length(double *arr, unsigned start, unsigned end,
                                unsigned last_index) {

    unsigned length = end - start + 1;
    unsigned half = length / 2; // half je index prvega cez polovico
    for (int i = half - 4; i >= 0; i -= 4) {
        if (UNLIKELY(end - 3 - i > last_index))
            break;
        double *p1 = arr + start + i;
        double *p2 = arr + end - 3 - i;
        {
            __m256d vec1 = _mm256_load_pd(p1);
            __m256d vec2 = _mm256_load_pd(p2);
            // reverse one of registers register reg0
            __m256d reverse =
                _mm256_permute4x64_pd(vec1, _MM_SHUFFLE(0, 1, 2, 3));
            // register 2 vsebuje min vrednosti
            vec1 = _mm256_min_pd(vec2, reverse);
            vec1 = _mm256_permute4x64_pd(vec1, _MM_SHUFFLE(0, 1, 2, 3));
            // register 1 vsebuje max vrednosti
            vec2 = _mm256_max_pd(vec2, reverse);
            _mm256_store_pd(p1, vec1);
            _mm256_store_pd(p2, vec2);
        }
    }
}

/** @brief compared vectors from top and bottom of array and then
 * gradually compare inner vectors.
 * @param arr pointer to the array to be sorted
 * @param start index of the first element to be sorted
 * @param end index of the last element to be sorted
 * @param last_index last index of array to be sorted, maximum is
 * array size - 1
 * @param depth a parameter to follow the depth of recursion
 */
inline void lane_crossing_compare(double *arr, unsigned start, unsigned end,
                                  unsigned last_index, unsigned depth) {

    if (start > last_index) {
        // std::cout << "aborted start index: " << start <<
        // std::endl;
        return;
    }
    unsigned length = end - start + 1;
    if (length == 4) {
        // this is the ending case do single vector permutations
        __m256d reg = _mm256_load_pd(arr + start);
        // this is the ending case do single vector permutations

        { // shuffling between 128bit lanes of 256bit register
            // (3,2,1,0)->(1,0,3,2) one compares 0 and 2 ,  1 and 3
            __m256d shuffled_reg = _mm256_permute4x64_pd(reg, 0b01001110);
            __m256d mask = _mm256_cmp_pd(reg, shuffled_reg, _CMP_LE_OQ);
            mask = _mm256_castsi256_pd(_mm256_xor_si256(
                _mm256_castpd_si256(mask),
                _mm256_set_epi64x(0xffffffffffffffff, 0xffffffffffffffff,
                                  0x0000000000000000, 0x0000000000000000)));
            reg = _mm256_blendv_pd(shuffled_reg, reg, mask);
        }
        // shuffle neighbour numbers
        { // shuffle inside 128 lanes
            // from (3,2,1,0) produce (2,3,0,1)
            __m256d shuffled_reg = _mm256_shuffle_pd(reg, reg, 0b0101);
            __m256d mask = _mm256_cmp_pd(reg, shuffled_reg, _CMP_LE_OQ);
            mask = _mm256_castsi256_pd(_mm256_xor_si256(
                _mm256_castpd_si256(mask),
                _mm256_set_epi64x(0xffffffffffffffff, 0x0000000000000000,
                                  0xffffffffffffffff, 0x0000000000000000)));
            reg = _mm256_blendv_pd(shuffled_reg, reg, mask);
            _mm256_store_pd(arr + start, reg);
        }

        return;
    }
    double *p = arr + start;
    for (unsigned i = 0; i < length / 2; i += 4) {
        if (start + length / 2 + i > last_index)
            break;
        {
            double *p1 = p + i;
            double *p2 = p + length / 2 + i;
            __m256d reg0 = _mm256_load_pd(p1); // i-ti od začetka
            __m256d reg1 = _mm256_load_pd(p2); // ta je prvi čez polovico
            // register 2 vsebuje min vrednosti
            __m256d min = _mm256_min_pd(reg1, reg0);
            // register 1 vsebuje max vrednosti
            reg1 = _mm256_max_pd(reg1, reg0);
            reg0 = min;
            _mm256_store_pd(p1, reg0);
            _mm256_store_pd(p2, reg1);
        }
    }
    lane_crossing_compare(arr, start, (start + end) / 2, last_index, depth + 1);
    lane_crossing_compare(arr, (start + end) / 2 + 1, end, last_index,
                          depth + 1);
};

/**
 * @brief The function sorts an array with 8*n elements
 * @param array pointer to the start of the array
 * @param start index of the first number to be sorted
 * @param end index of the last number to be sorted
 * @details end-start+1 should be 8*n
 */
inline void sort_4n_vector(double *array, unsigned start, unsigned end) {
    int pow2 = (int)std::ceil(std::log2f(end + 1));
    int imaginary_length = (int)std::pow(2, pow2);
    unsigned full_length = end - start + 1;
    unsigned last_index = end - 3; // last index to be loaded
    assert((full_length >= 0 && (mod4(full_length) == 0)) &&
           "The array to be sorted is not a multiple of 4!");

    if (full_length == 4) {
        __m256d vec = _mm256_load_pd(array);
        bitonic_sort(vec);
        _mm256_store_pd(array, vec);
    } else if (full_length == 8) {
        __m256d vec1 = _mm256_load_pd(array);
        __m256d vec2 = _mm256_load_pd(array + 4);
        bitonic_sort(vec1, vec2);
        _mm256_store_pd(array, vec1);
        _mm256_store_pd(array + 4, vec2);

    } else if (full_length == 16) {
        __m256d vec1 = _mm256_load_pd(array);
        __m256d vec2 = _mm256_load_pd(array + 4);
        __m256d vec3 = _mm256_load_pd(array + 8);
        __m256d vec4 = _mm256_load_pd(array + 12);
        bitonic_sort(vec1, vec2, vec3, vec4);
        _mm256_store_pd(array, vec1);
        _mm256_store_pd(array + 4, vec2);
        _mm256_store_pd(array + 8, vec3);
        _mm256_store_pd(array + 12, vec4);

    } else {
        for (unsigned i = start; i < end; i += 4) {
            __m256d vec1 = _mm256_load_pd(array + i);
            bitonic_sort(vec1);
            _mm256_store_pd(array + i, vec1);
        }

        // outer loop
        // len is number of floats in length to be compared
        // each step increases this length by factor of 2.
        for (unsigned len = 8; len <= imaginary_length; len *= 2) {
            // std::cout << "len: " << len << std::endl;
            // inner loop goes over all subdivisions
            for (unsigned n = 0; n < imaginary_length; n += len) {
                compare_full_length(array, n, n + len - 1, last_index);
                lane_crossing_compare(array, n, n + len - 1, last_index, 0);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////
// DOUBLE ARRAY SORTING ALGORITHM FOR ARRAYS OF ARBITRARY LENGTH, the array must
// contain 8n elements
// float implementation

inline void maskload(int diff, double *p2, __m256i &mask, __m256d &reg1) {
    switch (diff) {
    case 0: {
        mask = _mm256_set_epi64x(0, 0, 0, LOAD);
        reg1 = _mm256_maskload_pd(p2, mask);
        __m256d infinity =
            _mm256_set1_pd(std::numeric_limits<double>::infinity());
        reg1 = _mm256_blend_pd(infinity, reg1, 0b0001);
    } break;
    case 1: {
        mask = _mm256_set_epi64x(0, 0, BITONIC_SORT::LOAD, BITONIC_SORT::LOAD);
        reg1 = _mm256_maskload_pd(p2, mask);
        __m256d infinity =
            _mm256_set1_pd(std::numeric_limits<double>::infinity());
        reg1 = _mm256_blend_pd(infinity, reg1, 0b0011);
    } break;
    case 2: {
        mask = _mm256_set_epi64x(0, LOAD, LOAD, LOAD);
        reg1 = _mm256_maskload_pd(p2, mask);
        __m256d infinity =
            _mm256_set1_pd(std::numeric_limits<double>::infinity());
        reg1 = _mm256_blend_pd(infinity, reg1, 0b0111);
    } break;
    };
}

/** @brief compared vectors from top and bottom of array and then gradually
 * compare inner vectors.
 * @param arr pointer to the float array to be sorted
 * @param start index of the first element to be sorted
 * @param end index of the last element to be sorted
 */
inline void compare_full_length_all_cases(double *arr, unsigned start,
                                          unsigned end, unsigned last_index) {
    // std::cout << "start: " << start << "end: " << end
    //          << " last index: " << last_index << std::endl;
    unsigned length = end - start + 1;
    unsigned half = length / 2; // half je index prvega cez polovico
    for (int i = half - 4; i >= 0; i -= 4) {
        // index of last vector to load
        int last_vec_to_load = end - 3 - i;
        int diff = last_index - last_vec_to_load;
        if (diff < 0)
            return;
        // define pointers to the start of vectors to load
        double *p1 = arr + start + i;
        double *p2 = arr + last_vec_to_load;
        __m256d vec2;
        __m256i mask;
        if (diff < 3)
            maskload(diff, p2, mask, vec2);
        else
            vec2 = _mm256_load_pd(p2);

        __m256d vec1 = _mm256_load_pd(p1);
        // reverse one of registers register reg0
        __m256d reverse = _mm256_permute4x64_pd(vec1, _MM_SHUFFLE(0, 1, 2, 3));
        // register 2 vsebuje min vrednosti
        vec1 = _mm256_min_pd(vec2, reverse);
        vec1 = _mm256_permute4x64_pd(vec1, _MM_SHUFFLE(0, 1, 2, 3));
        // register 1 vsebuje max vrednosti
        vec2 = _mm256_max_pd(vec2, reverse);
        _mm256_store_pd(p1, vec1);
        if (diff <= 2)
            _mm256_maskstore_pd(p2, mask, vec2);
        else
            _mm256_store_pd(p2, vec2);
    }
}

/** @brief compared vectors from top and bottom of array and then gradually
 * compare inner vectors.
 * @param arr pointer to the array to be sorted
 * @param start index of the first element to be sorted
 * @param end index of the last element to be sorted
 * @param depth a parameter to follow the depth of recursion
 */
inline void lane_crossing_compare_all_cases(double *arr, unsigned start,
                                            unsigned end, unsigned last_index,
                                            unsigned depth) {
    if (start > last_index) {
        return;
    }
    unsigned length = end - start + 1;
    if (length == 4) {
        int diff = last_index - start;
        if (diff < 1)
            return;
        __m256d reg;
        __m256i load_store_mask;
        if (diff < 3)
            maskload(diff, arr + start, load_store_mask, reg);
        else
            reg = _mm256_load_pd(arr + start);
        // else
        { // shuffling between 128bit lanes of 256bit register
            // (3,2,1,0)->(1,0,3,2) one compares 0 and 2 ,  1 and 3
            __m256d shuffled_reg = _mm256_permute4x64_pd(reg, 0b01001110);
            __m256d mask = _mm256_cmp_pd(reg, shuffled_reg, _CMP_LE_OQ);
            mask = _mm256_castsi256_pd(_mm256_xor_si256(
                _mm256_castpd_si256(mask),
                _mm256_set_epi64x(0xffffffffffffffff, 0xffffffffffffffff,
                                  0x0000000000000000, 0x0000000000000000)));
            reg = _mm256_blendv_pd(shuffled_reg, reg, mask);
        }
        // shuffle neighbour numbers
        { // shuffle inside 128 lanes
            // from (3,2,1,0) produce (2,3,0,1)
            __m256d shuffled_reg = _mm256_shuffle_pd(reg, reg, 0b0101);
            __m256d mask = _mm256_cmp_pd(reg, shuffled_reg, _CMP_LE_OQ);
            mask = _mm256_castsi256_pd(_mm256_xor_si256(
                _mm256_castpd_si256(mask),
                _mm256_set_epi64x(0xffffffffffffffff, 0x0000000000000000,
                                  0xffffffffffffffff, 0x0000000000000000)));
            reg = _mm256_blendv_pd(shuffled_reg, reg, mask);

            if (diff < 3)
                _mm256_maskstore_pd(arr + start, load_store_mask, reg);
            else
                _mm256_store_pd(arr + start, reg);
        }
        return;
    }

    double *p = arr + start;
    for (unsigned i = 0; i < length / 2; i += 4) {
        int diff = last_index - (start + length / 2 + i);
        if (diff < 0)
            break;
        __m256d reg1;
        __m256i mask;
        double *p2 = p + length / 2 + i;
        if (diff < 3)
            maskload(diff, p2, mask, reg1);
        else
            reg1 = _mm256_load_pd(p2);

        double *p1 = p + i;
        __m256d reg0 = _mm256_load_pd(p1); // i-ti od začetka
        // register 2 vsebuje min vrednosti
        __m256d min = _mm256_min_pd(reg1, reg0);
        // register 1 vsebuje max vrednosti
        reg1 = _mm256_max_pd(reg1, reg0);
        reg0 = min;
        _mm256_store_pd(p1, reg0);
        if (diff < 3)
            _mm256_maskstore_pd(p2, mask, reg1);
        else
            _mm256_store_pd(p2, reg1);
    }
    lane_crossing_compare_all_cases(arr, start, (start + end) / 2, last_index,
                                    depth + 1);
    lane_crossing_compare_all_cases(arr, (start + end) / 2 + 1, end, last_index,
                                    depth + 1);
};

/**
 * @brief The function sorts an array with 2^n elements
 * @param array pointer to the start of the array
 * @param start index of the first number to be sorted
 * @param end index of the last number to be sorted
 * @details end - start + 1 should be 2^n
 */
inline void sort_vector(aligned_vector<double> &array, unsigned start,
                        unsigned end) {
    unsigned full_length = end - start + 1;
    if (full_length <= 1)
        return;
    else if (full_length < 4) {
        double *p = array.data() + start;
        __m256d reg1;
        __m256i mask;
        maskload(full_length - 1, p, mask, reg1);
        bitonic_sort(reg1);
        _mm256_maskstore_pd(array.data() + start, mask, reg1);

        return;
    } else if (!(full_length & (full_length - 1)))
        sort_2n_vector(array.data(), start, end);
    else if (mod4(full_length) == 0)
        sort_4n_vector(array.data(), start, end);
    else {
        int pow2 = (int)std::ceil(std::log2f(end + 1));
        int imaginary_length = (int)std::pow(2, pow2);
        unsigned full_length = end - start + 1;
        unsigned last_index = end;

        for (unsigned i = start; i <= end - 3; i += 4) {
            __m256d vec1 = _mm256_load_pd(array.data() + i);
            bitonic_sort(vec1);
            _mm256_store_pd(array.data() + i, vec1);
        }
        ///////////////////////////////// load the partial one
        int reminder = mod4(end);
        double *p = array.data() + end - reminder;
        __m256d reg1;
        __m256i mask;
        maskload(reminder, p, mask, reg1);
        bitonic_sort(reg1);
        _mm256_maskstore_pd(array.data() + end - reminder, mask, reg1);

        ///////////////////////////////////////////////////////

        // outer loop
        // len is number of floats in length to be compared
        // each step increases this length by factor of 2.
        // 8 and less has already been done above
        // for (unsigned len = 16; len <= imaginary_length; len *= 2) {
        for (unsigned len = 8; len <= imaginary_length; len *= 2) {
            // inner loop goes over all subdivisions
            for (unsigned n = 0; n < imaginary_length; n += len) {
                compare_full_length_all_cases(array.data(), n, n + len - 1,
                                              last_index);
                lane_crossing_compare_all_cases(array.data(), n, n + len - 1,
                                                last_index, 0);
            }
        }
    }
}

} // namespace IMPROVED_BITONIC_SORT
#endif
