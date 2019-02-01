void bitonic_avx_reg_sort(__m256d &reg) {
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

void bitonic_2reg_sort(__m256d &reg1, __m256d &reg2) {
    {
        __m256d reverse2 = __m256d shuffled_reg =
            _mm256_permute4x64_pd(reg2, _MM_SHUFFLE(0, 1, 2, 3));
        __m256d max = _mm256_max_pd(reg, reverse2);
        __m256d min = _mm256_min_pd(reg, reverse2);
        // register 1 vsebuje max vrednosti
        reg = _mm256_blend_pd(max, min, 0b0000);
        // register 2 vsebuje min vrednosti
        reg2 = _mm256_blend_pd(max, min, 0b1111);
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

    {
        __m256d shuffled_reg2 =
            _mm256_permute4x64_pd(reg2, _MM_SHUFFLE(0, 1, 2, 3));
        __m256d max = _mm256_max_pd(reg2, shuffled_reg);
        __m256d min = _mm256_min_pd(reg2, shuffled_reg);
        // max mora biti pri 256
        reg2 = _mm256_blend_pd(max, min, 0b0011);
    }
    {
        __m256d shuffled_reg2 = _mm256_shuffle_pd(reg2, reg2, 0b0101);
        __m256d max = _mm256_max_pd(reg2, shuffled_reg);
        __m256d min = _mm256_min_pd(reg2, shuffled_reg);
        //   dst[63:0] := src1[63:0]
        //   dst[127:64] := src2[63:0]
        //   dst[192:128] := src1[192:128]
        //   dst[255:192] := src2[192:128]
        // this will place smallest number to the [0:63] register
        reg2 = _mm256_unpacklo_pd(min, max);
    }

    return;
}

inline aligned_vector<__m256d> bitonic_n_sort(aligned_vector<__m256d> &full_vec,
                                              int first_index, int last_index) {
    unsigned num_to_sort = 1 + last_index - first_index;
    if (num_to_sort == 1)
        bitonic_avx_reg_sort(full_vec[first_index]);

    else if (num_to_sort == 2)
        bitonic_2reg_sort(full_vec[first_index], full_vec[last_index]);

    else if (num_to_sort % 2 == 0) {
        for (int i = 0; i < num_to_sort / 2; i++) {
            __m256d reverse2 = __m256d shuffled_reg = _mm256_permute4x64_pd(
                full_vec[first_index + i], _MM_SHUFFLE(0, 1, 2, 3));
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
            _mm256d_set1_pd(std::numeric_limits<double>::max());

        for (int i = 0; i < num_to_sort / 2; i++) {
            __m256d reverse2 = __m256d shuffled_reg = _mm256_permute4x64_pd(
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
