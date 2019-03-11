#pragma once
#include <bitonic_sort.hpp>
/* This file contains implementation of hybrid sort (mixture of quick sort and
 * bitonic sort algorithms).
 */

namespace HYBRID_SORT {

/**
 * @brief function merges ordered arrays
 * @param arrayA pointer to the start of the array
 * @param startA index of the first element/vector to be loaded
 * @param endA index of the last element to be loaded
 * @param endB index of the last vector to be loaded
 */
inline void bitonic_merge_8n(float *array, int startA, int endA, int endB) {

    reg1 = _mm256_load_ps(array + i);
    reg2 = _mm256_load_ps(array + endA + 8 + i);
    // below indices already account for above loads
    int indexB = endA + 16;
    int indexA = startA + 8;
    int i = startA;
    while (indexB <= endB && indexA <= endA) {

        __m256 max = _mm256_max_ps(reg1, reg2);

        _mm256_store_ps(_mm256_min_ps(reg1, reg2));
        i += 8;
        if (*(array + indexA) < *(array + indexB)) {
            reg1 = _mm256_load_ps(array + indexA);
            reg2 = max;
            indexA += 8;
        } else {
            reg2 = _mm256_load_ps(array + indexB);
            reg1 = max;
            indexB += 8;
        }
    }
}

/**
 *@brief Hybrid sort function which sorts arrays containing 8n elements
 * @param vec vector to be sorted
 * @param start index of first element to be sorted
 * @param end index of the last element to be sorted
 */
inline void hybrid_sort_8n(aligned_vector<float> vec, int start, int end) {
    unsigned size = end - start + 1;
    unsigned num_registers = size / 8;
    float *p = vec.data();
    for (int i = start; i < end; i += 8) {
        __m256 reg = _mm256_load_ps(p + i);
        BITONIC_SORT::bitonic_sort(reg);
        _mm256_store_ps(reg);
    }

    for (int width = 2; width < num_registers / 2 + 1; width *= 2) {
        int width_count = size / width;
    }
}

} // namespace HYBRID_SORT
