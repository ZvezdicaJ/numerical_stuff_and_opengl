#pragma once
#include <bitonic_sort.hpp>
/* This file contains implementation of hybrid sort (mixture of quick sort and
 * bitonic sort algorithms).
 */

namespace HYBRID_SORT {

void print_aligned_vector(aligned_vector<float> vec, std::string str) {
    std::cout << str << " " << std::endl;
    for (int i = 0; i < vec.size(); i++) {
        std::cout << vec[i] << std::endl;
        if (mod8(i + 1) == 0)
            std::cout << "\n";
    }
}

/**
 * @brief function merges ordered arrays
 * @param arrayA pointer to the start of the array
 * @param startA index of the first element/vector to be loaded
 * @param endA index of the last vector to be loaded
 * @param endB index of the last vector to be loaded
 */
inline void bitonic_merge_8n(float *array, int startA, int endA, int endB) {
    std::cout << "startA: " << startA << "  endA: " << endA
              << "  endB: " << endB << std::endl;
    __m256 reg1 = _mm256_load_ps(array);
    __m256 reg2 = _mm256_load_ps(array + endA + 8);
    // below indices already account for above loads
    int indexB = endA + 8;
    int indexA = startA;
    int i = startA;
    i += 8;
    while (indexB <= endB && indexA <= endA) {
        BITONIC_SORT::bitonic_merge(reg1, reg2);
        _mm256_store_ps(array + i, reg1);
        if (i == startA) {
            indexB += 8;
            indexA += 8;
        }
        i += 8;

        std::cout << "indexA: " << indexA << std::endl;
        std::cout << "indexB: " << indexB << std::endl;
        

        if (*(array + indexA) < *(array + indexB)) {
            reg1 = _mm256_load_ps(array + indexA);
            indexA += 8;
        } else {
            reg1 = _mm256_load_ps(array + indexB);
            indexB += 8;
        }
    }
    while (indexA <= endA) {

        reg1 = _mm256_load_ps(array + indexA);
        indexA += 8;
        BITONIC_SORT::bitonic_merge(reg1, reg2);
        _mm256_store_ps(array + i, reg1);
        i += 8;
    }
    while (indexB <= endB) {

        reg1 = _mm256_load_ps(array + indexB);
        indexB += 8;
        BITONIC_SORT::bitonic_merge(reg1, reg2);
        _mm256_store_ps(array + i, reg1);
        i += 8;
    }
    _mm256_store_ps(array + i, reg2);
}

/**
 *@brief Hybrid sort function which sorts arrays containing 8n elements
 * @param vec vector to be sorted
 * @param start index of first element to be sorted
 * @param end index of the last element to be sorted
 */
inline void hybrid_sort_8n(aligned_vector<float> &vec, int start, int end) {
    unsigned size = end - start + 1;
    unsigned num_registers = size / 8;
    float *p = vec.data();
    for (int i = start; i < end; i += 8) {
        __m256 reg = _mm256_load_ps(p + i);
        BITONIC_SORT::bitonic_sort(reg);
        _mm256_store_ps(p + i, reg);
    }
    /*
    for (int i = 0; i < size; i++) {
        std::cout << vec[i] << std::endl;
        if (mod8(i + 1) == 0)
            std::cout << "\n";
            }*/

    int pow2 = (int)std::ceil(std::log2f(end + 1)) - 1;
    int imaginary_half = (int)std::pow(2, pow2);
    int imaginary_length = 2 * imaginary_half;

    // width is max width of each of arrays to be merged
    for (int width = 8; width <= imaginary_half; width *= 2) {
        std::cout << "width: " << width << std::endl;
        for (int start = 0; start <= end - 7 - width; start += 2 * width)

            // startA - index of first A vector to be loaded
            // endA - index of the last A vector to be loaded
            // endB - index of the last B vector to be loaded
            bitonic_merge_8n(vec.data(), start, start + width - 8,
                             std::min(start + 2 * width - 8, end - 7));
    }
    // print_aligned_vector(vec, "sorted_vector: ");
}

} // namespace HYBRID_SORT
// namespace HYBRID_SORT
