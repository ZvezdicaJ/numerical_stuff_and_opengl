#ifdef __AVX2__

#ifndef __BITONIC_SORT_FLOAT__
#define __BITONIC_SORT_FLOAT__
namespace BITONIC_SORT {

void bitonic_sort(__m256 &reg, __m256i key) {
    {
        // prvi korak: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b10110001);
        __m256i shuffled_key = _mm256_shuffle_ps(reg, reg, 0b10110001);
        __m256 max = _mm256_max_ps(reg, shuffled_reg);
        __m256 min = _mm256_min_ps(reg, shuffled_reg);

        __m256 mask = _mm256_cmp_ps(reg, shuffled_reg, _CMP_GE_OQ);
        unsigned mask_u32 = _mm256_movemask_ps(mask_vec);
        key = _mm256_blendv_epi32(key, shuffled_key, mask_u32);
        reg = _mm256_blend_ps(min, max, 0b10101010);
    }
    {
        // drugi korak *----* *--*     *----* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b00011011);
        __m256i shuffled_key = _mm256_shuffle_ps(key, key, 0b00011011);
        __m256 max = _mm256_max_ps(reg, shuffled_reg);
        __m256 min = _mm256_min_ps(reg, shuffled_reg);

        __m256 mask = _mm256_cmp_ps(key, shuffled_key, _CMP_GE_OQ);
        unsigned mask_u32 = _mm256_movemask_ps(mask_vec);
        key = _mm256_blendv_epi32(key, shuffled_key, mask_u32);
        reg = _mm256_blend_ps(max, min, 0b00110011);
    }
    {
        // ponovimo prvi korak: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b10110001);
        __m256i shuffled_key = _mm256_shuffle_ps(key, key, 0b10110001);

        __m256 max = _mm256_max_ps(reg, shuffled_reg);
        __m256 min = _mm256_min_ps(reg, shuffled_reg);

        __m256 mask = _mm256_cmp_ps(reg, shuffled_reg, _CMP_GE_OQ);
        unsigned mask_u32 = _mm256_movemask_ps(mask_vec);
        key = _mm256_blendv_epi32(key, shuffled_key, mask_u32);
        reg = _mm256_blend_ps(min, max, 0b10101010);
    }
    {
        // now we have to reverse register:
        // *--------* *------* *----* *--*
        // the following line first reverses the two halves of register
        // in the next step it reverses both halfs
        __m256 reversed_halves = _mm256_permute2f128_ps(reg, reg, 0b00000001);
        __m256 reversed =
            _mm256_shuffle_ps(reversed_halves, reversed_halves, 0b00011011);

        __m256i reversed_halves_key =
            _mm256_permute2f128_epi32(key, key, 0b00000001);
        __m256i reversed_key = _mm256_shuffle_epi32(
            reversed_halves_key, reversed_halves, 0b00011011);

        __m256 max = _mm256_max_ps(reg, reversed);
        __m256 min = _mm256_min_ps(reg, reversed);
        // first unpack 4 minimum and then 4 maximums
        // 1 unpack an elements from min
        // 1 unpack an element from max
        reg = _mm256_blend_ps(max, min, 0b00001111);
    }
    {
        //  *----* *----*     *----* *----*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b01001110);
        __m256i shuffled_key = _mm256_shuffle_epi32(key, key, 0b01001110);
        __m256 max = _mm256_max_ps(reg, shuffled_reg);
        __m256 min = _mm256_min_ps(reg, shuffled_reg);
        // max mora biti pri 256
        // min is located at the start of register (at 0 - lower half)
        __m256 mask = _mm256_cmp_ps(reg, shuffled_reg, _CMP_GE_OQ);
        unsigned mask_u32 = _mm256_movemask_ps(mask_vec);
        key = _mm256_blendv_epi32(key, shuffled_key, mask_u32);
        reg = _mm256_blend_ps(max, min, 0b00110011);
    }
    {
        // and finally repeat the first step: *--*  *--*  *--* *--*
        __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, 0b10110001);
        __m256 max = _mm256_max_ps(reg, shuffled_reg);
        __m256 min = _mm256_min_ps(reg, shuffled_reg);

        __m256 mask = _mm256_cmp_ps(reg, shuffled_reg, _CMP_GE_OQ);
        unsigned mask_u32 = _mm256_movemask_ps(mask_vec);
        key = _mm256_blendv_epi32(key, shuffled_key, mask_u32);
        reg = _mm256_blend_ps(min, max, 0b10101010);
    }
}

} // namespace BITONIC_SORT

#ENDIF
