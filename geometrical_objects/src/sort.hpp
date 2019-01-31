__m256d bitonic_avx_reg_sort(__m256d reg) {
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
        //max mora biti pri 256
        reg = _mm256_unpacklo_pd(min, max);
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

    return reg;
}
