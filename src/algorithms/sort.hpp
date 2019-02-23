#pragma once
#ifdef __AVX2__
#define __BITONIC_SORT_FLOAT__
/**
 *@brief The function accepts a single __m256d vector and sorts it.
 * @param reg register to be sorted
 */
void bitonic_sort(__m256 &reg) {
    {
        // print_avx(reg, "reg: ");
        // prvi korak: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b10110001);
        // print_avx(shuffled_reg, "shuffled: ");
        __m256 max = _mm256_max_ps(reg, shuffled_reg);
        __m256 min = _mm256_min_ps(reg, shuffled_reg);
        // print_avx(min, "min: ");
        // print_avx(max, "max: ");
        //   dst[63:0] := src1[31:0]
        //   dst[127:64] := src2[63:32]
        //   dst[192:128] := src1[95:64]
        //   dst[255:192] := src2[127:96]
        // this will produce smallest number to in the [0:63] register
        reg = _mm256_blend_ps(min, max, 0b10101010);
        // print_avx(reg, "reg: ");
    }
    // print_avx(reg, "reg: ");
    {
        // drugi korak *----* *--*     *----* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b00011011);
        // print_avx(shuffled_reg, "shuffled: ");
        __m256 max = _mm256_max_ps(reg, shuffled_reg);
        __m256 min = _mm256_min_ps(reg, shuffled_reg);
        // max mora biti pri 256
        // min is located at the start of register (at 0 - lower half)
        reg = _mm256_blend_ps(max, min, 0b00110011);
        // print_avx(reg, "reg: ");
    }
    // print_avx(reg, "reg: ");
    {
        // ponovimo prvi korak: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b10110001);
        __m256 max = _mm256_max_ps(reg, shuffled_reg);
        __m256 min = _mm256_min_ps(reg, shuffled_reg);
        //   dst[63:0] := src1[63:0]
        //   dst[127:64] := src2[63:0]
        //   dst[192:128] := src1[192:128]
        //   dst[255:192] := src2[192:128]
        // this will produce smallest number to in the [0:63] register
        reg = _mm256_blend_ps(min, max, 0b10101010);
    }
    // print_avx(reg, "reg: ");
    {
        // now we have to reverse register:
        // *--------* *------* *----* *--*
        // the following line first reverses the two halves of register
        // in the next step it reverses both halfs
        __m256 reversed_halves = _mm256_permute2f128_ps(reg, reg, 0b00000001);
        __m256 reversed =
            _mm256_shuffle_ps(reversed_halves, reversed_halves, 0b00011011);
        __m256 max = _mm256_max_ps(reg, reversed);
        __m256 min = _mm256_min_ps(reg, reversed);
        // first unpack 4 minimum and then 4 maximums
        // 1 unpack an elements from min
        // 1 unpack an element from max
        reg = _mm256_blend_ps(max, min, 0b00001111);
    }
    // print_avx(reg, "reg: ");
    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b01001110);
        __m256 max = _mm256_max_ps(reg, shuffled_reg);
        __m256 min = _mm256_min_ps(reg, shuffled_reg);
        // max mora biti pri 256
        // min is located at the start of register (at 0 - lower half)
        reg = _mm256_blend_ps(max, min, 0b00110011);
    }
    // print_avx(reg, "reg: ");
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b10110001);
        __m256 max = _mm256_max_ps(reg, shuffled_reg);
        __m256 min = _mm256_min_ps(reg, shuffled_reg);
        //   dst[63:0] := src1[31:0]
        //   dst[127:64] := src2[63:32]
        //   dst[192:128] := src1[95:64]
        //   dst[255:192] := src2[127:96]
        // this will produce smallest number to in the [0:63] register
        reg = _mm256_blend_ps(min, max, 0b10101010);
    }
}
#endif

/**
 * @brief The function accepts two __m256d vectors and sorts them.
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
    // print_avx(reg0, "reg0: ");
    // print_avx(reg1, "reg1: ");
    // std::cout << std::endl;
    {
        // reverse one of registers register reg0
        __m256 reversed_halves = _mm256_permute2f128_ps(reg0, reg0, 0b00000001);
        __m256 reversed =
            _mm256_shuffle_ps(reversed_halves, reversed_halves, 0b00011011);

        // register 2 vsebuje min vrednosti
        reg0 = _mm256_min_ps(reg1, reversed);
        // register 1 vsebuje max vrednosti
        reg1 = _mm256_max_ps(reg1, reversed);
        // print_avx(max, "max: ");
        // print_avx(min, "min: ");
    }
    // print_avx(reg0, "reg0: ");
    // print_avx(reg1, "reg1: ");
    // std::cout << std::endl;
    {
        // tu narediš *----* *----* *----* *----*
        __m256 reversed_halves = _mm256_permute2f128_ps(reg0, reg0, 0b00000001);
        __m256 max = _mm256_max_ps(reg0, reversed_halves);
        __m256 min = _mm256_min_ps(reg0, reversed_halves);
        // max mora biti pri 256
        reg0 = _mm256_blend_ps(max, min, 0b00001111);
    }
    {
        // tu narediš *----* *----* *----* *----*
        __m256 reversed_halves = _mm256_permute2f128_ps(reg1, reg1, 0b00000001);
        __m256 max = _mm256_max_ps(reg1, reversed_halves);
        __m256 min = _mm256_min_ps(reg1, reversed_halves);
        // max mora biti pri 256
        reg1 = _mm256_blend_ps(max, min, 0b00001111);
    }

    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg0, reg0, 0b01001110);
        __m256 max = _mm256_max_ps(reg0, shuffled_reg);
        __m256 min = _mm256_min_ps(reg0, shuffled_reg);
        // max mora biti pri 256
        // min is located at the start of register (at 0 - lower half)
        reg0 = _mm256_blend_ps(max, min, 0b00110011);
    }
    // print_avx(reg, "reg: ");
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg0, reg0, 0b10110001);
        __m256 max = _mm256_max_ps(reg0, shuffled_reg);
        __m256 min = _mm256_min_ps(reg0, shuffled_reg);
        //   dst[63:0] := src1[31:0]
        //   dst[127:64] := src2[63:32]
        //   dst[192:128] := src1[95:64]
        //   dst[255:192] := src2[127:96]
        // this will produce smallest number to in the [0:63] register
        reg0 = _mm256_blend_ps(min, max, 0b10101010);
    }

    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg1, reg1, 0b01001110);
        __m256 max = _mm256_max_ps(reg1, shuffled_reg);
        __m256 min = _mm256_min_ps(reg1, shuffled_reg);
        // max mora biti pri 256
        // min is located at the start of register (at 0 - lower half)
        reg1 = _mm256_blend_ps(max, min, 0b00110011);
    }
    // print_avx(reg, "reg: ");
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg1, reg1, 0b10110001);
        __m256 max = _mm256_max_ps(reg1, shuffled_reg);
        __m256 min = _mm256_min_ps(reg1, shuffled_reg);
        //   dst[63:0] := src1[31:0]
        //   dst[127:64] := src2[63:32]
        //   dst[192:128] := src1[95:64]
        //   dst[255:192] := src2[127:96]
        // this will produce smallest number to in the [0:63] register
        reg1 = _mm256_blend_ps(min, max, 0b10101010);
    }
    return;
}

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
    // compare (reg1, reg3) and  (reg0, reg2)
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
    // compare (reg0, reg2) and  (reg1, reg3)
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
    { // mix across avx lanes
        __m256 shuffled_reg = _mm256_permute2f128_ps(reg0, reg0, 0b00000001);
        __m256 max = _mm256_max_ps(reg0, shuffled_reg);
        __m256 min = _mm256_min_ps(reg0, shuffled_reg);
        reg0 = _mm256_blend_ps(max, min, 0b00001111);
    }
    { // mix across avx lanes
        __m256 shuffled_reg = _mm256_permute2f128_ps(reg1, reg1, 0b00000001);
        __m256 max = _mm256_max_ps(reg1, shuffled_reg);
        __m256 min = _mm256_min_ps(reg1, shuffled_reg);
        reg1 = _mm256_blend_ps(max, min, 0b00001111);
    }
    { // mix across avx lanes
        __m256 shuffled_reg = _mm256_permute2f128_ps(reg2, reg2, 0b00000001);
        __m256 max = _mm256_max_ps(reg2, shuffled_reg);
        __m256 min = _mm256_min_ps(reg2, shuffled_reg);
        reg2 = _mm256_blend_ps(max, min, 0b00001111);
    }
    { // mix across avx lanes
        __m256 shuffled_reg = _mm256_permute2f128_ps(reg3, reg3, 0b00000001);
        __m256 max = _mm256_max_ps(reg3, shuffled_reg);
        __m256 min = _mm256_min_ps(reg3, shuffled_reg);
        reg3 = _mm256_blend_ps(max, min, 0b00001111);
    }

    // mix inside avx lanes each register separately, but jump single value
    {
        // register 0:  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg0, reg0, 0b01001110);
        __m256 max = _mm256_max_ps(reg0, shuffled_reg);
        __m256 min = _mm256_min_ps(reg0, shuffled_reg);
        // max mora biti pri 256
        // min is located at the start of register (at 0 - lower half)
        reg0 = _mm256_blend_ps(max, min, 0b00110011);
    }

    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg1, reg1, 0b01001110);
        __m256 max = _mm256_max_ps(reg1, shuffled_reg);
        __m256 min = _mm256_min_ps(reg1, shuffled_reg);
        // max mora biti pri 256
        // min is located at the start of register (at 0 - lower half)
        reg1 = _mm256_blend_ps(max, min, 0b00110011);
    }
    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg2, reg2, 0b01001110);
        __m256 max = _mm256_max_ps(reg2, shuffled_reg);
        __m256 min = _mm256_min_ps(reg2, shuffled_reg);
        // max mora biti pri 256
        // min is located at the start of register (at 0 - lower half)
        reg2 = _mm256_blend_ps(max, min, 0b00110011);
    }
    // print_avx(reg, "reg: ");
    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg3, reg3, 0b01001110);
        __m256 max = _mm256_max_ps(reg3, shuffled_reg);
        __m256 min = _mm256_min_ps(reg3, shuffled_reg);
        // max mora biti pri 256
        // min is located at the start of register (at 0 - lower half)
        reg3 = _mm256_blend_ps(max, min, 0b00110011);
    }
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg0, reg0, 0b10110001);
        __m256 max = _mm256_max_ps(reg0, shuffled_reg);
        __m256 min = _mm256_min_ps(reg0, shuffled_reg);
        // this will produce smallest number to in the [0:63] register
        reg0 = _mm256_blend_ps(min, max, 0b10101010);
    }
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg1, reg1, 0b10110001);
        __m256 max = _mm256_max_ps(reg1, shuffled_reg);
        __m256 min = _mm256_min_ps(reg1, shuffled_reg);
        // this will produce smallest number to in the [0:63] register
        reg1 = _mm256_blend_ps(min, max, 0b10101010);
    }
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg2, reg2, 0b10110001);
        __m256 max = _mm256_max_ps(reg2, shuffled_reg);
        __m256 min = _mm256_min_ps(reg2, shuffled_reg);
        // this will produce smallest number to in the [0:63] register
        reg2 = _mm256_blend_ps(min, max, 0b10101010);
    }
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg3, reg3, 0b10110001);
        __m256 max = _mm256_max_ps(reg3, shuffled_reg);
        __m256 min = _mm256_min_ps(reg3, shuffled_reg);
        // this will produce smallest number to in the [0:63] register
        reg3 = _mm256_blend_ps(min, max, 0b10101010);
    }
}

#ifdef __AVX2__
#define __BITONIC_SORT_DOUBLE__
/**
 *@brief The function accepts a single __m256d vector and sorts it.
 * @param reg register to be sorted
 */
inline void bitonic_sort(__m256d &reg) {
    {
        __m256d shuffled_reg = _mm256_shuffle_pd(reg, reg, 0b0101);
        __m256d max = _mm256_max_pd(reg, shuffled_reg);
        __m256d min = _mm256_min_pd(reg, shuffled_reg);
        //   dst[63:0] := src1[63:0]
        //   dst[127:64] := src2[63:0]
        //   dst[192:128] := src1[192:128]
        //   dst[255:192] := src2[192:128]
        // this will produce smallest number to in the [0:63] register
        reg = _mm256_unpacklo_pd(min, max);
    }
    {
        __m256d shuffled_reg =
            _mm256_permute4x64_pd(reg, _MM_SHUFFLE(0, 1, 2, 3));
        __m256d max = _mm256_max_pd(reg, shuffled_reg);
        __m256d min = _mm256_min_pd(reg, shuffled_reg);
        // max mora biti pri 256
        reg = _mm256_blend_pd(max, min, 0b0011);
    }
    {
        __m256d shuffled_reg = _mm256_shuffle_pd(reg, reg, 0b0101);
        __m256d max = _mm256_max_pd(reg, shuffled_reg);
        __m256d min = _mm256_min_pd(reg, shuffled_reg);
        //   dst[63:0] := src1[63:0]
        //   dst[127:64] := src2[63:0]
        //   dst[192:128] := src1[192:128]
        //   dst[255:192] := src2[192:128]
        // this will produce smallest number to in the [0:63] register
        reg = _mm256_unpacklo_pd(min, max);
    }
}

/**
 * @brief The function accepts two __m256d vectors and sorts them.
 * @param reg1 upper vector of numbers - at the end it contains larger
 * values, the largest value is in the upper half of register [255:192]
 *
 * @param reg0 lower vector of numbers - at the end it contains smaller
 * values. The smallest value is in the lowest half of register - at [63:0]
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
        __m256d max = _mm256_max_pd(reg1, shuffled_reg1);
        __m256d min = _mm256_min_pd(reg1, shuffled_reg1);
        // max mora biti pri 256
        reg1 = _mm256_blend_pd(max, min, 0b0011);
    }
    // print_avx(reg0, "reg0: ");
    // print_avx(reg1, "reg1: ");
    // std::cout << std::endl;
    {
        __m256d shuffled_reg1 = _mm256_shuffle_pd(reg1, reg1, 0b0101);
        __m256d max = _mm256_max_pd(reg1, shuffled_reg1);
        __m256d min = _mm256_min_pd(reg1, shuffled_reg1);
        //   dst[63:0] := src1[63:0]
        //   dst[127:64] := src2[63:0]
        //   dst[192:128] := src1[192:128]
        //   dst[255:192] := src2[192:128]
        // this will produce smallest number to in the [0:63] register
        reg1 = _mm256_unpacklo_pd(min, max);
    }
    // print_avx(reg0, "reg0: ");
    // print_avx(reg1, "reg1: ");
    // std::cout << std::endl;
    {
        // tu narediš *----* *----*
        __m256d shuffled_reg0 = _mm256_permute4x64_pd(reg0, 0b01001110);
        __m256d max = _mm256_max_pd(reg0, shuffled_reg0);
        __m256d min = _mm256_min_pd(reg0, shuffled_reg0);
        // max mora biti pri 256
        reg0 = _mm256_blend_pd(max, min, 0b0011);
    }
    // print_avx(reg0, "reg0: ");
    // print_avx(reg1, "reg1: ");
    // std::cout << std::endl;
    {
        __m256d shuffled_reg0 = _mm256_shuffle_pd(reg0, reg0, 0b0101);
        __m256d max = _mm256_max_pd(reg0, shuffled_reg0);
        __m256d min = _mm256_min_pd(reg0, shuffled_reg0);
        //   dst[63:0] := src1[63:0]
        //   dst[127:64] := src2[63:0]
        //   dst[192:128] := src1[192:128]
        //   dst[255:192] := src2[192:128]
        // this will place smallest number to the [0:63] register
        reg0 = _mm256_unpacklo_pd(min, max);
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
 * values. The smallest value is in the lowest half of register - at [63:0]
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
    { // shuffling between 128bit lanes
        __m256d shuffled_reg = _mm256_permute4x64_pd(reg0, 0b01001110);
        __m256d max = _mm256_max_pd(reg0, shuffled_reg);
        __m256d min = _mm256_min_pd(reg0, shuffled_reg);
        // max mora biti pri 256
        reg0 = _mm256_blend_pd(max, min, 0b0011);
    }
    { // shuffling between 128bit lanes
        __m256d shuffled_reg = _mm256_permute4x64_pd(reg1, 0b01001110);
        __m256d max = _mm256_max_pd(reg1, shuffled_reg);
        __m256d min = _mm256_min_pd(reg1, shuffled_reg);
        // max mora biti pri 256
        reg1 = _mm256_blend_pd(max, min, 0b0011);
    }
    { // shuffling between 128bit lanes
        __m256d shuffled_reg = _mm256_permute4x64_pd(reg2, 0b01001110);
        __m256d max = _mm256_max_pd(reg2, shuffled_reg);
        __m256d min = _mm256_min_pd(reg2, shuffled_reg);
        // max mora biti pri 256
        reg2 = _mm256_blend_pd(max, min, 0b0011);
    }
    { // shuffling between 128bit lanes
        __m256d shuffled_reg = _mm256_permute4x64_pd(reg3, 0b01001110);
        __m256d max = _mm256_max_pd(reg3, shuffled_reg);
        __m256d min = _mm256_min_pd(reg3, shuffled_reg);
        // max mora biti pri 256
        reg3 = _mm256_blend_pd(max, min, 0b0011);
    }
    // shuffle neighbour numbers
    {
        __m256d shuffled_reg1 = _mm256_shuffle_pd(reg1, reg1, 0b0101);
        __m256d max = _mm256_max_pd(reg1, shuffled_reg1);
        __m256d min = _mm256_min_pd(reg1, shuffled_reg1);
        // this will produce smallest number to in the [0:63] register
        reg1 = _mm256_unpacklo_pd(min, max);
    }
    {
        __m256d shuffled_reg = _mm256_shuffle_pd(reg2, reg2, 0b0101);
        __m256d max = _mm256_max_pd(reg2, shuffled_reg);
        __m256d min = _mm256_min_pd(reg2, shuffled_reg);
        // this will produce smallest number to in the [0:63] register
        reg2 = _mm256_unpacklo_pd(min, max);
    }
    {
        __m256d shuffled_reg = _mm256_shuffle_pd(reg3, reg3, 0b0101);
        __m256d max = _mm256_max_pd(reg3, shuffled_reg);
        __m256d min = _mm256_min_pd(reg3, shuffled_reg);
        // this will produce smallest number to in the [0:63] register
        reg3 = _mm256_unpacklo_pd(min, max);
    }

    {
        __m256d shuffled_reg0 = _mm256_shuffle_pd(reg0, reg0, 0b0101);
        __m256d max = _mm256_max_pd(reg0, shuffled_reg0);
        __m256d min = _mm256_min_pd(reg0, shuffled_reg0);
        // this will place smallest number to the [0:63] register
        reg0 = _mm256_unpacklo_pd(min, max);
    }
    // print_avx(reg0, "reg0: ");
    // print_avx(reg1, "reg1: ");
    // std::cout << std::endl;
    return;
}

/**
 *@brief The function accepts a vector of __m256d vectors and sorts them -
 *sort number which they contain.
 *@details The function is recursive and accepts two additional parameters.
 *They tell which part of the vector should be sorted.
 * @param full_vec vector containing all subvectors (all the numbers to be
 *sorted)
 * @param first_index is the starting index of element to be sorted
 * @param last_index is the last index of element to be sorted
 */
inline void bitonic_sort(aligned_vector<__m256d> &full_vec,
                         unsigned first_index = 0, unsigned last_index = 0) {
    if (last_index == 0)
        last_index = full_vec.size() - 1;

    unsigned num_to_sort = 1 + last_index - first_index;
    if (num_to_sort == 1)
        bitonic_sort(full_vec[first_index]);

    else if (num_to_sort == 2)
        bitonic_sort(full_vec[first_index], full_vec[last_index]);

    else if (num_to_sort % 2 == 0) {
        for (unsigned i = 0; i < num_to_sort / 2; i++) {
            __m256d reverse2 = _mm256_permute4x64_pd(full_vec[first_index + i],
                                                     _MM_SHUFFLE(0, 1, 2, 3));
            __m256d max = _mm256_max_pd(full_vec[last_index - i], reverse2);
            __m256d min = _mm256_min_pd(full_vec[last_index - i], reverse2);
            // register 1 vsebuje max vrednosti
            full_vec[last_index - i] = _mm256_blend_pd(max, min, 0b0000);
            // register 2 vsebuje min vrednosti
            full_vec[first_index + i] = _mm256_blend_pd(max, min, 0b1111);
        }
        bitonic_sort(full_vec, first_index + num_to_sort / 2 - 1,
                     first_index + num_to_sort / 2);
        bitonic_sort(full_vec, first_index + num_to_sort / 2, last_index);
    }
    // temporary solution until
    else {
        __m256d additional_reg =
            _mm256_set1_pd(std::numeric_limits<double>::max());

        for (unsigned i = 0; i < num_to_sort / 2; i++) {
            __m256d reverse2 = _mm256_permute4x64_pd(
                full_vec[first_index + i + 1], _MM_SHUFFLE(0, 1, 2, 3));
            __m256d max = _mm256_max_pd(full_vec[last_index - i], reverse2);
            __m256d min = _mm256_min_pd(full_vec[last_index - i], reverse2);
            // register 1 vsebuje max vrednosti
            full_vec[last_index - i] = _mm256_blend_pd(max, min, 0b0000);
            // register 2 vsebuje min vrednosti
            full_vec[first_index + i] = _mm256_blend_pd(max, min, 0b1111);
        }

        bitonic_sort(full_vec, first_index + num_to_sort / 2 - 1,
                     first_index + num_to_sort / 2);
        bitonic_sort(full_vec, first_index + num_to_sort / 2, last_index);
    }
}

// float implementation
inline void compare_full_length(__m256 *arr, unsigned start, unsigned end) {
    unsigned length = end - start;
    unsigned half = length / 2;
    for (unsigned i = 0; i < length / 2; i++) {
        { // reverse lover half and compare to upper half
            __m256 &vec1 = arr[i];
            __m256 &vec2 = arr[length - i];

            __m256 reversed_halves1 =
                _mm256_permute2f128_ps(vec1, vec1, 0b00000001);
            __m256 reversed1 = _mm256_shuffle_ps(reversed_halves1,
                                                 reversed_halves1, 0b00011011);
            vec1 = _mm256_min_ps(reversed1, vec2);
            vec2 = _mm256_max_ps(reversed1, vec2);
        }
    }
}

inline void lane_crossing_compare(__m256 *arr, unsigned start, unsigned end,
                                  unsigned depth = 0) {
    unsigned length = end - start;

    if (start == end) {
        __m256 &reg = arr[start];
        { // mix across avx lanes
            __m256 shuffled_reg = _mm256_permute2f128_ps(reg, reg, 0b00000001);
            __m256 max = _mm256_max_ps(reg, shuffled_reg);
            __m256 min = _mm256_min_ps(reg, shuffled_reg);
            reg = _mm256_blend_ps(max, min, 0b00001111);
        }

        // mix inside avx lanes each register separately, but jump single
        // value
        {
            // register 0:  *----* *----*     *----* *----*
            __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b01001110);
            __m256 max = _mm256_max_ps(reg, shuffled_reg);
            __m256 min = _mm256_min_ps(reg, shuffled_reg);
            // max mora biti pri 256
            // min is located at the start of register (at 0 - lower half)
            reg = _mm256_blend_ps(max, min, 0b00110011);
        }
    }
}

// double implementation
/** @brief compared vectors from top and bottom of array and then gradually
 * compare inner vectors.
 * @param arr pointer to the array to be sorted
 * @param start index of the first element to be sorted
 * @param end index of the last element to be sorted
 */
inline void compare_full_length(double *arr, unsigned start, unsigned end) {
    unsigned length = end - start + 1;
    unsigned half = length / 2;
    for (unsigned i = 0; i < half; i + 4) {
        double *p1 = arr + start + i;
        double *p2 = arr + end - 3 - i;
        __m256d vec1 = _mm256_loadu_pd(p1);
        __m256d vec2 = _mm256_loadu_pd(p2);
        // reverse one of registers register reg0
        __m256d reverse = _mm256_permute4x64_pd(vec1, _MM_SHUFFLE(0, 1, 2, 3));
        // register 2 vsebuje min vrednosti
        vec1 = _mm256_min_pd(vec2, reverse);
        // register 1 vsebuje max vrednosti
        vec2 = _mm256_max_pd(vec2, reverse);
        __m256d vec1 = _mm256_storeu_pd(p1, vec1);
        __m256d vec2 = _mm256_storeu_pd(p2, vec2);
    }
}

/** @brief compared vectors from top and bottom of array and then gradually
 * compare inner vectors.
 * @param arr pointer to the array to be sorted
 * @param start index of the first element to be sorted
 * @param end index of the last element to be sorted
 * @param depth a parameter to follow the depth of recursion
 */
inline void lane_crossing_compare(double *arr, unsigned start, unsigned end,
                                  unsigned depth) {
    if (depth > 10)
        exit(0);
    unsigned length = end - start;

    std::cout << "start: " << start << std::endl;
    std::cout << "end: " << end << std::endl;
    std::cout << "depth " << depth << std::endl;
    std::cout << "new end: " << (start + end) / 2 << std::endl;
    if (start == end) {
        __m256d &reg = arr[start];
        // this is the ending case do single vector permutations
        { // shuffling between 128bit lanes of 256bit register
            __m256d shuffled_reg = _mm256_permute4x64_pd(reg, 0b01001110);
            __m256d max = _mm256_max_pd(reg, shuffled_reg);
            __m256d min = _mm256_min_pd(reg, shuffled_reg);
            // max mora biti pri 256
            reg = _mm256_blend_pd(max, min, 0b0011);
        }
        std::cout << "ok\n" << std::endl;
        // shuffle neighbour numbers
        {
            __m256d shuffled_reg = _mm256_shuffle_pd(reg, reg, 0b0101);
            __m256d max = _mm256_max_pd(reg, shuffled_reg);
            __m256d min = _mm256_min_pd(reg, shuffled_reg);
            // this will produce smallest number to in the [0:63]
            // register
            reg = _mm256_unpacklo_pd(min, max);
        }
        std::cout << "ok\n" << std::endl;
        return;
    }

    for (unsigned i = 0; i < length / 2; i++) {
        {
            __m256d &reg0 = arr[i];              // i-ti od začetka
            __m256d &reg1 = arr[length / 2 + i]; // ta je prvi čez polovico
            // reverse one of registers
            __m256d reverse =
                _mm256_permute4x64_pd(reg0, _MM_SHUFFLE(0, 1, 2, 3));
            // register 2 vsebuje min vrednosti
            reg0 = _mm256_min_pd(reg1, reverse);
            // register 1 vsebuje max vrednosti
            reg1 = _mm256_max_pd(reg1, reverse);
            // print_avx(max, "max: ");
            // print_avx(min, "min: ");
        }
    }
    lane_crossing_compare(arr, start, (start + end) / 2, depth + 1);
    lane_crossing_compare(arr, (start + end) / 2 + 1, end, depth + 1);
};

inline aligned_vector<__m256d> partition(double *arr, unsigned start,
                                         unsigned end) {

    aligned_vector<__m256d> avx_vec;
    unsigned full_length = end - start + 1;
    avx_vec.reserve(full_length / 4);
    avx_vec.resize(full_length / 4);
    for (unsigned i = start; i <= end; i += 4) {
        __m256i tmp = _mm256_stream_load_si256((__m256i *)(arr + i));
        print_avx((__m256d)tmp, "loaded avx reg: ");
        //_mm256_stream_pd(avx_vec.data() + i / 4, (__m256d)tmp);
        avx_vec.push_back((__m256d)tmp);
        print_avx(avx_vec[0], "loaded avx reg: ");
    }
    return avx_vec;
}

inline void sort_2n_vector(double *array, unsigned start, unsigned end) {

    unsigned full_length = end - start + 1; // number of double numbers to sort
    unsigned vec_count = full_length / 4;
    std::cout << "vec_count: " << vec_count << std::endl;
    std::cout << "vec_count: " << full_length << std::endl;
    std::cout << "vec_count: " << vec_count << std::endl;
    assert((full_length >= 0 && !(full_length & (full_length - 1))) &&
           "The array to be sorted is not the power of 2!");

    //__m256d *arr = avx_vec.data();
    if (full_length == 4) {

        __m256d vec = _mm_loadu_pd(array);
        bitonic_sort(vec);
        _mm256_storeu_pd(array, vec);

    } else if (full_length == 8) {

        __m256d vec1 = _mm_loadu_pd(array);
        __m256d vec2 = _mm_loadu_pd(array + 4);
        bitonic_sort(vec1, vec2);
        _mm256_storeu_pd(array, vec1);
        _mm256_storeu_pd(array, vec2);

    } else if (full_length == 16) {

        __m256d vec1 = _mm_loadu_pd(array);
        __m256d vec2 = _mm_loadu_pd(array + 4);
        __m256d vec3 = _mm_loadu_pd(array + 8);
        __m256d vec4 = _mm_loadu_pd(array + 12);
        bitonic_sort(vec1, vec2, vec3, vec4);
        _mm256_storeu_pd(array, vec1);
        _mm256_storeu_pd(array, vec2);
        _mm256_storeu_pd(array, vec3);
        _mm256_storeu_pd(array, vec4);

    } else if (full_length >= 32) {
        for (unsigned i = start; i <= end; i++)
            bitonic_sort(avx_vec[i]);

        // outer loop

        // len is a number of __m256d vectors in a loop
        // this is the outer most loop
        for (unsigned len = 2; len <= vec_count; len *= 2) {

            // inner loop goes over all subdivisions
            for (unsigned n = 0; n < vec_count; n += len) {
                // first compare full length of divisions
                compare_full_length(avx_vec, n, n + len - 1);
                // then do cross comparison
                // below function is recursive ---
                // like a loop over all sizes of subdivision
                //   std::cout << "n: " << n << std::endl;
                // std::cout << "len: " << len << std::endl;
                // std::cout << "n + len -1: " << n + len - 1 << std::endl;
                // lane_crossing_compare(arr, n, n + len - 1, 0);
            }
        }
    }
    // print_avx(*arr, "test arr print: ");
}

#endif
