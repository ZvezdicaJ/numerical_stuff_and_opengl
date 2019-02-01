#ifdef __AVX__
#define __BITONIC_SORT_FLOAT__
/**
 *@brief The function accepts a single __m256d vector and sorts it.
 * @param reg register to be sorted
 */
void bitonic_avx_reg_sort(__m256 &reg) {
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
        // repeat second step  *----* *--*     *----* *--*
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

#ifdef __AVX2__
#define __BITONIC_SORT_DOUBLE__
/**
 *@brief The function accepts a single __m256d vector and sorts it.
 * @param reg register to be sorted
 */
inline void bitonic_avx_reg_sort(__m256d &reg) {
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
 * @param reg1 upper vector of numbers - at the end it contains larger values
 * @param reg0 lower vector of numbers - at the end it contains smaller values
 */
inline void bitonic_2reg_sort(__m256d &reg0, __m256d &reg1) {
    bitonic_avx_reg_sort(reg1); // sort first register
    bitonic_avx_reg_sort(reg0); // sort second register
    //print_avx(reg0, "reg0: ");
    //print_avx(reg1, "reg1: ");
    //std::cout << std::endl;
    {
        // reverse one of registers register reg0
        __m256d reverse2 = _mm256_permute4x64_pd(reg0, _MM_SHUFFLE(0, 1, 2, 3));
        __m256d max = _mm256_max_pd(reg1, reverse2);
        __m256d min = _mm256_min_pd(reg1, reverse2);
        // print_avx(max, "max: ");
        // print_avx(min, "min: ");
        // register 1 vsebuje max vrednosti
        reg1 = _mm256_blend_pd(max, min, 0b0000);
        // register 2 vsebuje min vrednosti
        reg0 = _mm256_blend_pd(max, min, 0b1111);
    }
    //print_avx(reg0, "reg0: ");
    //print_avx(reg1, "reg1: ");
    //std::cout << std::endl;
    {
        // tu narediš *----* *----*
        __m256d shuffled_reg1 = _mm256_permute4x64_pd(reg1, 0b01001110);
        __m256d max = _mm256_max_pd(reg1, shuffled_reg1);
        __m256d min = _mm256_min_pd(reg1, shuffled_reg1);
        // max mora biti pri 256
        reg1 = _mm256_blend_pd(max, min, 0b0011);
    }
    //print_avx(reg0, "reg0: ");
    //print_avx(reg1, "reg1: ");
    //std::cout << std::endl;
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
    //print_avx(reg0, "reg0: ");
    //print_avx(reg1, "reg1: ");
    //std::cout << std::endl;
    {
        // tu narediš *----* *----*
        __m256d shuffled_reg0 = _mm256_permute4x64_pd(reg0, 0b01001110);
        __m256d max = _mm256_max_pd(reg0, shuffled_reg0);
        __m256d min = _mm256_min_pd(reg0, shuffled_reg0);
        // max mora biti pri 256
        reg0 = _mm256_blend_pd(max, min, 0b0011);
    }
    //print_avx(reg0, "reg0: ");
    //print_avx(reg1, "reg1: ");
    //std::cout << std::endl;
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
    //std::cout << std::endl;
    return;
}

/**
 *@brief The function accepts a vector of __m256d vectors and sorts them - sort
 *number which they contain.
 *@details The function is recursive and accepts two additional parameters. They
 *tell which part of the vector should be sorted.
 * @param full_vec vector containing all subvectors (all the numbers to be
 *sorted)
 * @param first_index is the starting index of element to be sorted
 * @param last_index is the last index of element to be sorted
 */
inline void bitonic_n_sort(aligned_vector<__m256d> &full_vec,
                           unsigned first_index = 0, unsigned last_index = 0) {
    if (last_index == 0)
        last_index = full_vec.size() - 1;

    unsigned num_to_sort = 1 + last_index - first_index;
    if (num_to_sort == 1)
        bitonic_avx_reg_sort(full_vec[first_index]);

    else if (num_to_sort == 2)
        bitonic_2reg_sort(full_vec[first_index], full_vec[last_index]);

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
        bitonic_n_sort(full_vec, first_index + num_to_sort / 2 - 1,
                       first_index + num_to_sort / 2);
        bitonic_n_sort(full_vec, first_index + num_to_sort / 2, last_index);
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

        bitonic_n_sort(full_vec, first_index + num_to_sort / 2 - 1,
                       first_index + num_to_sort / 2);
        bitonic_n_sort(full_vec, first_index + num_to_sort / 2, last_index);
    }
}
#endif
