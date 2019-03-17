#pragma once
#include "bitonic_sort.hpp"
#include "auxiliary_functions.hpp"
/* This file contains implementation of hybrid sort (mixture of quick sort and
 * bitonic sort algorithms).
 */

namespace HYBRID_SORT {

//////////////////////////////////////////
//////// A scalar implementation of Quick sort from
/// https://www.geeksforgeeks.org/quick-sort/
//////////////////////////////////////////

// A utility function to swap two elements
template <typename T>
void swap(T *a, T *b) {
    T t = *a;
    *a = *b;
    *b = t;
}

/* This function takes last element as pivot, places
   the pivot element at its correct position in sorted
    array, and places all smaller (smaller than pivot)
   to left of pivot and all greater elements to right
   of pivot */
template <typename T>
T scalar_partition(T *arr, int low, int high) {
    // pivot is an element around which we partition the given array
    // it can be chosen arbitrarily
    // we partition the given array in two parts:
    // first part: elements bigger than pivot
    // second part: elements smaller than pivot
    T pivot = arr[high];
    // set the starting index one less than the first element of array
    // before the first write it will get incremented by 1
    int i = (low - 1); // Index of smaller element

    // start at the beginning of array
    // check if an element is smaller than the pivot
    // if yes put into the part of array containing smaller elements
    // move that element that was previously there to upper part
    // you are yet to check the upper part - as j increments
    for (int j = low; j <= high - 1; j++) {
        // If current element is smaller than or
        // equal to pivot
        if (arr[j] <= pivot) {
            i++; // increment index of smaller element
            swap(arr + i, arr + j);
        }
    }
    // in the end put the pivot in the middle
    swap(arr + i + 1, arr + high);
    return (i + 1);
}

/* The main function that implements QuickSort
 arr[] --> Array to be sorted,
  low  --> Starting index,
  high  --> Ending index */
template <typename T>
void quickSort(T *arr, int low, int high) {
    if (low < high) {
        /* pi is partitioning index, arr[p] is now
           at right place */
        int pi = scalar_partition(arr, low, high);

        // Separately sort elements before
        // partition and after partition
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}
//////////////////////////////////////////////////////////////////
//////////////// End of scalar Quick sort
///////////////////////////////////////////////////////////////////

void print_aligned_vector(aligned_vector<float> vec, std::string str) {
    std::cout << str << " " << std::endl;
    for (int i = 0; i < vec.size(); i++) {
        std::cout << vec[i] << std::endl;
        if (mod8(i + 1) == 0)
            std::cout << "\n";
    }
}
///////////////////////////////////////////////////////
///////////  This is just a try at my own merge  - which does not work in place
///////////////////////////////////////////////////////

/**
 * @brief function merges ordered arrays
 * @param arrayA pointer to the start of the array
 * @param startA index of the first element/vector to be loaded
 * @param endA index of the last vector to be loaded
 * @param endB index of the last vector to be loaded
 */
inline void bitonic_merge_8n(float *array, int startA, int endA, int endB) {

    aligned_vector<float> temp_vec;
    temp_vec.reserve(endB - startA + 8);
    float *p = temp_vec.data();

    // std::cout << "startA: " << startA << "  endA: " << endA
    //          << "  endB: " << endB << std::endl;

    __m256 reg1 = _mm256_load_ps(array + startA);
    __m256 reg2 = _mm256_load_ps(array + endA + 8);
    // below indices already account for above loads
    int indexB = endA + 16;
    int indexA = startA + 8;
    // int i = startA;
    int i = 0;
    BITONIC_SORT::bitonic_merge(reg1, reg2);
    // _mm256_store_ps(array + i, reg1);
    _mm256_store_ps(p + i, reg1);
    i += 8;

    while (indexB <= endB && indexA <= endA) {

        if (*(array + indexA) < *(array + indexB)) {
            reg1 = _mm256_load_ps(array + indexA);
            indexA += 8;
        } else {
            reg1 = _mm256_load_ps(array + indexB);
            indexB += 8;
        }
        /* std::cout << "indexA: " << indexA << std::endl;
        std::cout << "indexB: " << indexB << std::endl;
        std::cout << "i     : " << i << std::endl;
        print_avx(reg1, "reg1: ");
        print_avx(reg2, "reg2: ");*/
        BITONIC_SORT::bitonic_merge(reg1, reg2);
        /* std::cout << "result:" << std::endl;
        print_avx(reg1, "reg1: ");
        print_avx(reg2, "reg2: ");
        std::cout << "\n\n\n" << std::endl;
        */
        //_mm256_store_ps(array + i, reg1);
        _mm256_store_ps(p + i, reg1);
        i += 8;
    }

    while (indexA <= endA) {
        reg1 = _mm256_load_ps(array + indexA);
        indexA += 8;
        BITONIC_SORT::bitonic_merge(reg1, reg2);
        //_mm256_store_ps(array + i, reg1);
        _mm256_store_ps(p + i, reg1);
        i += 8;
    }

    while (indexB <= endB) {

        reg1 = _mm256_load_ps(array + indexB);
        indexB += 8;
        BITONIC_SORT::bitonic_merge(reg1, reg2);
        //_mm256_store_ps(array + i, reg1);
        _mm256_store_ps(p + i, reg1);
        i += 8;
    }
    //_mm256_store_ps(array + i, reg2);
    _mm256_store_ps(p + i, reg2);

    __m256i *pi = (__m256i *)p;
    __m256i *arr = (__m256i *)(array + startA);
    unsigned num_vec = (endB - startA + 8) / 8;

    for (int i = 0; i < num_vec; i++) {
        __m256i temp = _mm256_stream_load_si256(pi);
        _mm256_stream_si256(arr, temp);
        pi++;
        arr++;
    }
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

    int pow2 = (int)std::ceil(std::log2f(end + 1)) - 1;
    int imaginary_half = (int)std::pow(2, pow2);
    int imaginary_length = 2 * imaginary_half;

    // width is max width of each of arrays to be merged
    for (int width = 8; width <= imaginary_half; width *= 2) {
        //  std::cout << "\n\nWIDTH: " << width << "\n" << std::endl;
        // if (width >= 16)
        //    break;
        for (int start = 0; start <= end - 7 - width; start += 2 * width)

            // startA - index of first A vector to be loaded
            // endA - index of the last A vector to be loaded
            // endB - index of the last B vector to be loaded
            bitonic_merge_8n(vec.data(), start, start + width - 8,
                             std::min(start + 2 * width - 8, end - 7));
    }
    //
}

//////////////////////////////////////////////////////////////
// HYBRID_SORT FROM PAPER: A Novel Hybrid Quicksort Algorithm Vectorized using
// AVX-512 on Intel Skylake
///////////////////////////////////////////////////////

/** @brief Function compresses 256 vector based on a provided mask.
 * @detail more details on
 * https://stackoverflow.com/questions/36932240/avx2-what-is-the-most-efficient-way-to-pack-left-based-on-a-mask
 *
 */
inline __m256 compress256(__m256 src, unsigned int mask /* from movmskps */) {
    uint64_t expanded_mask =
        _pdep_u64(mask, 0x0101010101010101); // unpack each bit to a byte
    expanded_mask *= 0xFF; // mask |= mask<<1 | mask<<2 | ... | mask<<7;
    // ABC... -> AAAAAAAABBBBBBBBCCCCCCCC...: replicate each bit to fill its
    // byte

    const uint64_t identity_indices =
        0x0706050403020100; // the identity shuffle for vpermps, packed to one
                            // index per byte
    uint64_t wanted_indices = _pext_u64(identity_indices, expanded_mask);

    __m128i bytevec = _mm_cvtsi64_si128(wanted_indices);
    __m256i shufmask = _mm256_cvtepu8_epi32(bytevec);

    return _mm256_permutevar8x32_ps(src, shufmask);
}

#ifndef __AVX512__
inline void _mm256_compresstoreu_ps(float *address, unsigned int mask,
                                    __m256 vec_to_store) {
    __m256 compressed = compress256(vec_to_store, mask);
    _mm256_storeu_ps(address, compressed);
}
#endif

/**
 * @brief This function performs simd partition for quick sort algorithm
 * @details The function is an avx2 implementation of an algorithm from the
 * paper: A Novel Hybrid Quicksort Algorithm Vectorized using AVX-512 on
 * Intel Skylake; Author: Berenger Bramas
 * @param array Pointer to the start of whole array
 * @param left First index to be sorted
 * @param right Last index to be sorted - which is also pivot
 *
 */
void simd_partition(float *array, unsigned left, unsigned right) {
    int length = right - left + 1;
    if (length < 16)
        scalar_partition<float>(array, (int)left, (int)right);
    static const unsigned S = 8;
    float pivot = array[right];
    __m256 pivotvec = _mm256_set1_ps(pivot);

    __m256 left_val = _mm256_loadu_ps(array + left);
    int left_w = left;
    left += 8;

    int right_w = right + 1;
    right -= 7;
    __m256 right_val = _mm256_load_ps(array + right);

    while (left + S <= right) {
        const int free_lef = left_left_w;
        const int free_right = right_w - right;

        _mm256 val;
        if (free_left <= free_right) {
            val = _mm256_loadu_ps(array + left);
            left += S;
        } else {
            right -= S;
            val = _mm256_loadu_ps(array + right);
        }
        __m256 mask = _mm256_cmp_ps(val, pivotvec, _CMP_LE_OQ);
        unsigned mask_int = _mm256_movemask_ps(mask);
        unsigned num_bits_set = _mm_popcnt_u32(mask_int);

        _mm256_compresstoreu_ps(array + left_w, mask, val);
        left_w += num_bits_set;
        _mm256_compresstoreu_ps(array + right_w, ~mask, val);
        right_w -= (S - num_bits_set);
    }

    {
        unsigned remaining = right - left;
        __m256 val = _mm256_loadu_ps(array + left);
        left = right;
        unsigned int mask = _mm256_cmp_ps(val, pivotvec, _CMP_LE_OQ);
        unsigned int mask_low = mask & ˜(0xFF << remaining);
        unsigned int mask_high = (˜mask)&˜(0xFF << remaining);
        _mm256_compresstoreu_ps(array + left_w, mask_low, val);
        left_w += _mm_popcnt_u32(mask_low);
        right_w += _mm_popcnt_u32(mask_high);
        _mm256_compresstoreu_ps(array + right_w, mask_high, val);
    }

    {
        unsigned int mask = _mm256_cmp_ps(left_val, pivotvec, _CMP_LE_OQ);
        _mm256_compresstoreu_ps(array + left_w, mask, left_val);
        unsigned int count_low = _mm_popcnt_u32(mask);
        left_w += count_low;
        right_w -= (S - count_low);
        _mm256_compresstoreu_ps(array + right_w, ~mask, left_val);
    }
    {
        unsigned int mask = _mm256_cmp_ps(right_val, pivotvec, _CMP_LE_OQ);
        _mm256_compresstoreu_ps(array + right_w, mask, left_val);
        unsigned int count_low = _mm_popcnt_u32(mask);
        left_w += count_low;
        right_w -= (S - count_low);
        _mm256_compresstoreu_ps(array + right_w, ~mask, left_val);
    }
    return left_w;
}

inline void simd_QS(aligned_vector<float> &vec, unsigned start, unsigned end) {
    return simd_QS_helper(vec.data(), start, end);
}

inline void(float *array, unsigned start, unsigned end) {
    int length = end - start + 1;
    if (length > 8) {
        unsigned partition_bound = simd_partition(array, start, left);
        float temp = *(array + partition_bound);
        *(array + partition_bound) = *(array + right);
        *(array + right) = temp;
    } else if (length == 8) {
        __m256 reg = _mm256_loadu_ps(array + start);
        BITONIC_SORT::bitonic_sort(reg);
        _mm256_storeu_ps(array + start, reg);
    } else {
        __m256 reg;
        int diff = end - start;
        __m256i mask;
        BITONIC_SORT::maskload(diff, array + start, mask, reg);
        BITONIC_SORT::bitonic_sort(reg);
        _mm256_maskstore_ps(array + start, mask, reg);
    }
}

} // namespace HYBRID_SORT
