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
 * @brief The function blends two __m256i vectors based on a provided mask.
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
 * @brief The overload of _mm256_blendv_epi32 function with mask being of __m256
 * type.
 * @param a first vector to be blended
 * @param second vector to be blended
 * @param mask mask according to which to blend two vector. For more info check
 * Intel intrinsics guide - _mm256_blendv_ps function.
 */
inline __attribute__((always_inline)) __m256i
_mm256_blendv_epi32(const __m256i &a, const __m256i &b, const __m256 &mask) {
    return _mm256_castps_si256(
        _mm256_blendv_ps(_mm256_castsi256_ps(a), _mm256_castsi256_ps(b), mask));
}

/**
 * @brief shuffles a register according to uint8_t mask and compares shuffled
 * register to the original one. The resulting permutation is then applied to
 * the key.
 * @param reg register to be shuffled and compared
 * @param key key vector to be shuffled based on the result of comparison of
 * shuffled float register to the original one.
 * @param mask_epi32 helper mask to blend two vector. For more info
 * check Intel intrinsics guide - _mm256_blendv_ps function.
 * @param uint8_t mask according to which float register is shuffled.
 */
inline __attribute__((always_inline)) void
shuffle_and_compare(__m256 &reg, __m256i &key, const __m256i &mask_epi32,
                    const uint8_t mask8) {
    __m256 shuffled_reg = _mm256_shuffle_ps(reg, reg, mask8);
    __m256 mask = _mm256_cmp_ps(reg, shuffled_reg, _CMP_LE_OQ);
    mask = _mm256_castsi256_ps(
        _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));

    __m256i shuffled_key = _mm256_shuffle_epi32(key, mask8);
    key = _mm256_blendv_epi32(shuffled_key, key, mask);

    reg = _mm256_blendv_ps(shuffled_reg, reg, mask);
}

/**
 * @brief The function reverses a register according  and
 * compares reversed register to the original one. The resulting permutation is
 * then applied to the key.
 * @param reg register to be shuffled and compared
 * @param key key vector to be shuffled based on the result of comparison of
 * shuffled float register to the original one.
 * @param mask_epi32 helper mask to blend two vector. For more info
 * check Intel intrinsics guide - _mm256_blendv_ps function.
 */
inline __attribute__((always_inline)) void
reverse_and_compare(__m256 &reg, __m256i &key, const __m256i &mask_epi32) {

    __m256 reversed_halves = _mm256_permute2f128_ps(reg, reg, 0b00000001);
    __m256 reversed =
        _mm256_shuffle_ps(reversed_halves, reversed_halves, 0b00011011);
    __m256 mask = _mm256_cmp_ps(reg, reversed, _CMP_LE_OQ);
    mask = _mm256_castsi256_ps(
        _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));

    __m256i reversed_key_halves =
        _mm256_permute2f128_si256(key, key, 0b00000001);
    __m256i reversed_key =
        _mm256_shuffle_epi32(reversed_key_halves, 0b00011011);

    key = _mm256_blendv_epi32(reversed_key, key, _mm256_castps_si256(mask));

    reg = _mm256_blendv_ps(reversed, reg, mask);
}

/**
 * @brief The function accepts __m256 vector of float values and __m256i vector
 * of keys and sorts both according to float values.
 * @param reg register to be sorted
 * @param key the keys are sorted using the same permutation as values
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

    reverse_and_compare(reg, key, mask3);
    shuffle_and_compare(reg, key, mask2, 0b01001110);
    shuffle_and_compare(reg, key, mask1, 0b10110001);
}

/**
 * @brief The function reverses one of the input registers and compares the
 * reversed register to the other supplied register. The resulting permutation
 * is then applied to the key1 and key2 pair.
 * @param reg0 register to be shuffled and compared to be reversed
 * @param reg1 register bo be compared to register reg0
 * @param key1 register of keys to be sorted
 * @param key2 register of keys to be sorted
 */
inline __attribute__((always_inline)) void
reverse_and_compare(__m256 &reg0, __m256 &reg1, __m256i &key0, __m256i &key1) {

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

    key0 = _mm256_blendv_epi32(reversed_key, key1, _mm256_castps_si256(mask));
    key1 = _mm256_blendv_epi32(key1, reversed_key, _mm256_castps_si256(mask));
}

/**
 * @brief The function reverses the halves of the given register and compares it
 * to itself. The resulting permutation is then applied to the key1 and key2
 * pair.
 * @param reg0 register whose halves are reversed and compared to the original
 * register
 * @param key0 register containing keys, which are shuffled using the same
 * permutation as reg0
 * @param mask_epi32 mask which is used to compare
 */
inline __attribute__((always_inline)) void
reverse_halves_and_compare(__m256 &reg0, __m256i &key0,
                           const __m256i &mask_epi32) {
    //
    __m256 reversed_halves = _mm256_permute2f128_ps(reg0, reg0, 0b00000001);
    __m256 mask = _mm256_cmp_ps(reg0, reversed_halves, _CMP_LE_OQ);
    mask = _mm256_castsi256_ps(
        _mm256_xor_si256(_mm256_castps_si256(mask), mask_epi32));
    reg0 = _mm256_blendv_ps(reversed_halves, reg0, mask);

    __m256i reversed_key_halves =
        _mm256_permute2f128_si256(key0, key0, 0b00000001);

    key0 = _mm256_blendv_epi32(reversed_key_halves, key0,
                               _mm256_castps_si256(mask));
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

    reverse_and_compare(reg0, reg1, key0, key1);
    reverse_halves_and_compare(reg0, key0, mask3);
    reverse_halves_and_compare(reg1, key1, mask3);

    shuffle_and_compare(reg0, key0, mask2, 0b01001110);
    shuffle_and_compare(reg0, key0, mask1, 0b10110001);

    shuffle_and_compare(reg1, key1, mask2, 0b01001110);
    shuffle_and_compare(reg1, key1, mask1, 0b10110001);

    return;
}

/**
 * @brief The function accepts four unsorted  __m256 vectors and sorts them. It
 * accepts also four __m256i registers containing keys, which are shuffled using
 * the same permutations as float registers.
 * @param reg3 upper vector of numbers - at the end it contains larger values,
 * the largest value is in the upper half of register [255:192]
 * @param reg2
 * @param reg1
 * @param reg0 lower vector of numbers - at the end it contains smaller values.
 * The smallest value is in the lowest half of register - at [63:0]
 * @param key0
 * @param key1
 * @param key2
 * @param key3
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
    reverse_and_compare(reg0, reg3, key0, key3);
    reverse_and_compare(reg1, reg2, key1, key2);

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

    reverse_halves_and_compare(reg0, key0, mask_epi32);
    reverse_halves_and_compare(reg1, key1, mask_epi32);
    reverse_halves_and_compare(reg2, key2, mask_epi32);
    reverse_halves_and_compare(reg3, key3, mask_epi32);

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

//////////////////////////////////////////////////////////////////
// 2^N FLOAT ARRAY SORTING ALGORITHM

// float implementation
/** @brief The function compares vectors from top and bottom of array and then
 *gradually compare inner vectors.
 *@details It shuffles the vector in such a way that smaller values are in the
 *lower half of resulting vector. The keys are permuted using the same
 *permutations as floats
 * @param arr pointer to the float array to be sorted
 * @param start index of the first element to be sorted
 * @param end index of the last element to be sorted
 */
inline void compare_full_length(float *arr, int *keys, unsigned start,
                                unsigned end) {
    unsigned length = end - start + 1;
    unsigned half = length / 2;
    for (int i = 0; i < half; i += 8) {
        float *p1 = arr + start + i;
        float *p2 = arr + end - 7 - i;
        int *k1 = keys + start + i;
        int *k2 = keys + end - 7 - i;
        { // reverse lover half and compare to upper half
            __m256 vec1 = _mm256_load_ps(p1);
            __m256 vec2 = _mm256_load_ps(p2);
            __m256i key1 = _mm256_load_si256((__m256i *)k1);
            __m256i key2 = _mm256_load_si256((__m256i *)k2);

            reverse_and_compare(vec1, vec2, key1, key2);
            _mm256_store_ps(p1, vec1);
            _mm256_store_ps(p2, vec2);
            _mm256_store_si256((__m256i *)k1, key1);
            _mm256_store_si256((__m256i *)k2, key2);
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
inline void lane_crossing_compare(float *arr, int *keys, unsigned start,
                                  unsigned end, unsigned depth) {
    unsigned length = end - start + 1;
    if (length == 8) {
        __m256 reg = _mm256_load_ps(arr + start);
        __m256i key = _mm256_load_si256((__m256i *)(keys + start));
        // this is the ending case do single vector permutations
        __m256i mask_epi32 =
            _mm256_set_epi32(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                             0x00000000, 0x00000000, 0x00000000, 0x00000000);
        reverse_halves_and_compare(reg, key, mask_epi32);

        mask_epi32 =
            _mm256_set_epi32(0xffffffff, 0xffffffff, 0x00000000, 0x00000000,
                             0xffffffff, 0xffffffff, 0x00000000, 0x00000000);
        shuffle_and_compare(reg, key, mask_epi32, 0b01001110);

        mask_epi32 =
            _mm256_set_epi32(0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
                             0xffffffff, 0x00000000, 0xffffffff, 0x00000000);

        shuffle_and_compare(reg, key, mask_epi32, 0b10110001);
        _mm256_store_ps(arr + start, reg);
        _mm256_store_si256((__m256i *)(keys + start), key);
        return;
    }
    float *p = arr + start;
    int *k = keys + start;

    for (unsigned i = 0; i < length / 2; i += 8) {
        {
            float *p1 = p + i;
            float *p2 = p + length / 2 + i;
            int *k1 = k + i;
            int *k2 = k + length / 2 + i;

            __m256 reg0 = _mm256_load_ps(p1); // i-ti od začetka
            __m256 reg1 = _mm256_load_ps(p2); // ta je prvi čez polovico

            __m256i key0 = _mm256_load_si256((__m256i *)k1);
            __m256i key1 = _mm256_load_si256((__m256i *)k2);

            __m256 mask = _mm256_cmp_ps(reg0, reg1, _CMP_LE_OQ);
            __m256 tmp = _mm256_blendv_ps(reg1, reg0, mask);
            reg1 = _mm256_blendv_ps(reg0, reg1, mask);
            reg0 = tmp;
            __m256i tmp_key = _mm256_blendv_epi32(key1, key0, mask);
            key1 = _mm256_blendv_epi32(key0, key1, mask);
            key0 = tmp_key;

            _mm256_store_ps(p1, reg0);
            _mm256_store_ps(p2, reg1);
            _mm256_store_si256((__m256i *)k1, key0);
            _mm256_store_si256((__m256i *)k2, key1);
        }
    }
    lane_crossing_compare(arr, keys, start, (start + end) / 2, depth + 1);
    lane_crossing_compare(arr, keys, (start + end) / 2 + 1, end, depth + 1);
};

/**
 * @brief The function sorts an array with 8*n elements
 * @param array pointer to the start of the array
 * @param start index of the first number to be sorted
 * @param end index of the last number to be sorted
 * @details end-start+1 should be 8*N. If it's not
 * then add additional elements - like infinity, so that it will be large enough
 */
inline void sort_2n_vector(float *array, int *keys, unsigned start,
                           unsigned end) {

    unsigned full_length = end - start + 1; // number of double numbers to sort
    unsigned vec_count = full_length / 8;
    assert((full_length >= 0 && !(full_length & (full_length - 1))) &&
           "The array to be sorted is not the power of 2!");

    //__m256d *arr = avx_vec.data();
    if (full_length == 8) {

        __m256 vec = _mm256_load_ps(array);
        __m256i key = _mm256_load_si256((__m256i *)keys);
        bitonic_sort(vec, key);
        _mm256_store_ps(array, vec);
        _mm256_store_si256((__m256i *)keys, key);

    } else if (full_length == 16) {

        __m256 vec1 = _mm256_load_ps(array);
        __m256 vec2 = _mm256_load_ps(array + 8);

        __m256i key1 = _mm256_load_si256((__m256i *)keys);
        __m256i key2 = _mm256_load_si256((__m256i *)(keys + 8));

        bitonic_sort(vec1, vec2, key1, key2);

        _mm256_store_ps(array, vec1);
        _mm256_store_ps(array + 8, vec2);

        _mm256_store_si256((__m256i *)keys, key1);
        _mm256_store_si256((__m256i *)(keys + 8), key2);

    } else if (full_length == 32) {

        __m256 vec1 = _mm256_load_ps(array);
        __m256 vec2 = _mm256_load_ps(array + 8);
        __m256 vec3 = _mm256_load_ps(array + 16);
        __m256 vec4 = _mm256_load_ps(array + 24);
        __m256i key1 = _mm256_load_si256((__m256i *)keys);
        __m256i key2 = _mm256_load_si256((__m256i *)(keys + 8));
        __m256i key3 = _mm256_load_si256((__m256i *)(keys + 16));
        __m256i key4 = _mm256_load_si256((__m256i *)(keys + 24));

        bitonic_sort(vec1, vec2, vec3, vec4, key1, key2, key3, key4);

        _mm256_store_ps(array, vec1);
        _mm256_store_ps(array + 8, vec2);
        _mm256_store_ps(array + 16, vec3);
        _mm256_store_ps(array + 24, vec4);

        _mm256_store_si256((__m256i *)keys, key1);
        _mm256_store_si256((__m256i *)(keys + 8), key2);
        _mm256_store_si256((__m256i *)(keys + 16), key3);
        _mm256_store_si256((__m256i *)(keys + 24), key4);

    } else if (full_length >= 64) {
        for (unsigned i = start; i < end; i += 8) {
            __m256 vec1 = _mm256_load_ps(array + i);
            __m256i key = _mm256_load_si256((__m256i *)(keys + i));
            bitonic_sort(vec1, key);
            _mm256_store_ps(array + i, vec1);
            _mm256_store_si256((__m256i *)(keys + i), key);
        }

        // outer loop
        // len is number of floats in length to be compared
        // each step increases this length by factor of 2.
        for (unsigned len = 16; len <= full_length; len *= 2) {
            // std::cout << "len: " << len << std::endl;
            // inner loop goes over all subdivisions
            for (unsigned n = 0; n < full_length; n += len) {
                compare_full_length(array, keys, n, n + len - 1);
                lane_crossing_compare(array, keys, n, n + len - 1, 0);
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
inline void compare_full_length(float *arr, int *keys, unsigned start,
                                unsigned end, unsigned last_index) {
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
        int *k1 = keys + start + i;
        int *k2 = keys + end - 7 - i;
        { // reverse lover half and compare to upper half
            __m256 vec1 = _mm256_load_ps(p1);
            __m256 vec2 = _mm256_load_ps(p2);
            __m256i key1 = _mm256_load_si256((__m256i *)k1);
            __m256i key2 = _mm256_load_si256((__m256i *)k2);

            reverse_and_compare(vec1, vec2, key1, key2);
            _mm256_store_ps(p1, vec1);
            _mm256_store_ps(p2, vec2);
            _mm256_store_si256((__m256i *)k1, key1);
            _mm256_store_si256((__m256i *)k2, key2);
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
inline void lane_crossing_compare(float *arr, int *keys, unsigned start,
                                  unsigned end, unsigned last_index,
                                  unsigned depth) {

    if (start > last_index) {
        // std::cout << "aborted start index: " << start << std::endl;
        return;
    }
    unsigned length = end - start + 1;
    if (length == 8) {
        __m256 reg = _mm256_load_ps(arr + start);
        __m256i key = _mm256_load_si256((__m256i *)(keys + start));
        // this is the ending case do single vector permutations
        __m256i mask_epi32 =
            _mm256_set_epi32(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                             0x00000000, 0x00000000, 0x00000000, 0x00000000);
        reverse_halves_and_compare(reg, key, mask_epi32);

        mask_epi32 =
            _mm256_set_epi32(0xffffffff, 0xffffffff, 0x00000000, 0x00000000,
                             0xffffffff, 0xffffffff, 0x00000000, 0x00000000);
        shuffle_and_compare(reg, key, mask_epi32, 0b01001110);

        mask_epi32 =
            _mm256_set_epi32(0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
                             0xffffffff, 0x00000000, 0xffffffff, 0x00000000);

        shuffle_and_compare(reg, key, mask_epi32, 0b10110001);
        _mm256_store_ps(arr + start, reg);
        _mm256_store_si256((__m256i *)(keys + start), key);
        return;
    }
    float *p = arr + start;
    int *k = keys + start;
    // for (unsigned i = 0; i < length / 2; i += 8) {
    for (unsigned i = 0; i < length / 2; i += 8) {
        if (start + length / 2 + i > last_index)
            break;
        {
            float *p1 = p + i;
            float *p2 = p + length / 2 + i;
            int *k1 = k + i;
            int *k2 = k + length / 2 + i;

            __m256 reg0 = _mm256_load_ps(p1); // i-ti od začetka
            __m256 reg1 = _mm256_load_ps(p2); // ta je prvi čez polovico

            __m256i key0 = _mm256_load_si256((__m256i *)k1);
            __m256i key1 = _mm256_load_si256((__m256i *)k2);

            __m256 mask = _mm256_cmp_ps(reg0, reg1, _CMP_LE_OQ);
            __m256 tmp = _mm256_blendv_ps(reg1, reg0, mask);
            reg1 = _mm256_blendv_ps(reg0, reg1, mask);
            reg0 = tmp;
            __m256i tmp_key = _mm256_blendv_epi32(key1, key0, mask);
            key1 = _mm256_blendv_epi32(key0, key1, mask);
            key0 = tmp_key;

            _mm256_store_ps(p1, reg0);
            _mm256_store_ps(p2, reg1);
            _mm256_store_si256((__m256i *)k1, key0);
            _mm256_store_si256((__m256i *)k2, key1);
        }
    }
    lane_crossing_compare(arr, keys, start, (start + end) / 2, last_index,
                          depth + 1);
    lane_crossing_compare(arr, keys, (start + end) / 2 + 1, end, last_index,
                          depth + 1);
};

/**
 * @brief The function sorts an array with 2^n elements
 * @param array pointer to the start of the array
 * @param start index of the first number to be sorted
 * @param end index of the last number to be sorted
 * @details end-start+1 should be 8*n
 */
inline void sort_8n_vector(float *array, int *keys, unsigned start,
                           unsigned end) {

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

    if (full_length == 8) {

        __m256 vec = _mm256_load_ps(array);
        __m256i key = _mm256_load_si256((__m256i *)keys);
        bitonic_sort(vec, key);
        _mm256_store_ps(array, vec);
        _mm256_store_si256((__m256i *)keys, key);

    } else if (full_length == 16) {
        __m256 vec1 = _mm256_load_ps(array);
        __m256 vec2 = _mm256_load_ps(array + 8);
        __m256i key1 = _mm256_load_si256((__m256i *)keys);
        __m256i key2 = _mm256_load_si256((__m256i *)(keys + 8));

        bitonic_sort(vec1, vec2, key1, key2);

        _mm256_store_ps(array, vec1);
        _mm256_store_ps(array + 8, vec2);
        _mm256_store_si256((__m256i *)keys, key1);
        _mm256_store_si256((__m256i *)(keys + 8), key2);

    } else if (full_length == 32) {
        __m256 vec1 = _mm256_load_ps(array);
        __m256 vec2 = _mm256_load_ps(array + 8);
        __m256 vec3 = _mm256_load_ps(array + 16);
        __m256 vec4 = _mm256_load_ps(array + 24);

        __m256i key1 = _mm256_load_si256((__m256i *)keys);
        __m256i key2 = _mm256_load_si256((__m256i *)(keys + 8));
        __m256i key3 = _mm256_load_si256((__m256i *)(keys + 16));
        __m256i key4 = _mm256_load_si256((__m256i *)(keys + 24));

        bitonic_sort(vec1, vec2, vec3, vec4, key1, key2, key3, key4);

        _mm256_store_ps(array, vec1);
        _mm256_store_ps(array + 8, vec2);
        _mm256_store_ps(array + 16, vec3);
        _mm256_store_ps(array + 24, vec4);

        _mm256_store_si256((__m256i *)keys, key1);
        _mm256_store_si256((__m256i *)(keys + 8), key2);
        _mm256_store_si256((__m256i *)(keys + 16), key3);
        _mm256_store_si256((__m256i *)(keys + 24), key4);

    } else {
        for (unsigned i = start; i < end; i += 8) {

            __m256 vec1 = _mm256_load_ps(array + i);
            __m256i key = _mm256_load_si256((__m256i *)(keys + i));
            bitonic_sort(vec1, key);
            _mm256_store_ps(array + i, vec1);
            _mm256_store_si256((__m256i *)(keys + i), key);
        }

        // outer loop
        // len is number of floats in length to be compared
        // each step increases this length by factor of 2.
        for (unsigned len = 16; len <= imaginary_length; len *= 2) {
            // std::cout << "len: " << len << std::endl;
            // inner loop goes over all subdivisions
            for (unsigned n = 0; n < imaginary_length; n += len) {
                compare_full_length(array, keys, n, n + len - 1, last_index);
                lane_crossing_compare(array, keys, n, n + len - 1, last_index,
                                      0);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////
// FLOAT ARRAY SORTING ALGORITHM FOR ARRAYS OF ARBITRARY LENGTH, the array must
// contain 8n elements
// float implementation

inline void maskload(int diff, float *p, int *k, __m256i &mask, __m256 &reg1,
                     __m256i &key) {
    switch (diff) {
    case 0: {
        mask = _mm256_set_epi32(0, 0, 0, 0, 0, 0, 0, LOAD);
        reg1 = _mm256_maskload_ps(p, mask);
        key = _mm256_maskload_epi32(k, mask);
        __m256 infinity =
            _mm256_set1_ps(std::numeric_limits<float>::infinity());
        reg1 = _mm256_blend_ps(infinity, reg1, 0b00000001);
    } break;
    case 1: {
        mask = _mm256_set_epi32(0, 0, 0, 0, 0, 0, LOAD, LOAD);
        reg1 = _mm256_maskload_ps(p, mask);
        key = _mm256_maskload_epi32(k, mask);
        __m256 infinity =
            _mm256_set1_ps(std::numeric_limits<float>::infinity());
        reg1 = _mm256_blend_ps(infinity, reg1, 0b00000011);
    } break;
    case 2: {
        mask = _mm256_set_epi32(0, 0, 0, 0, 0, LOAD, LOAD, LOAD);
        reg1 = _mm256_maskload_ps(p, mask);
        key = _mm256_maskload_epi32(k, mask);
        __m256 infinity =
            _mm256_set1_ps(std::numeric_limits<float>::infinity());
        reg1 = _mm256_blend_ps(infinity, reg1, 0b00000111);
    } break;
    case 3: {
        mask = _mm256_set_epi32(0, 0, 0, 0, LOAD, LOAD, LOAD, LOAD);
        reg1 = _mm256_maskload_ps(p, mask);
        key = _mm256_maskload_epi32(k, mask);
        __m256 infinity =
            _mm256_set1_ps(std::numeric_limits<float>::infinity());
        reg1 = _mm256_blend_ps(infinity, reg1, 0b00001111);
    } break;
    case 4: {
        mask = _mm256_set_epi32(0, 0, 0, LOAD, LOAD, LOAD, LOAD, LOAD);
        reg1 = _mm256_maskload_ps(p, mask);
        key = _mm256_maskload_epi32(k, mask);
        __m256 infinity =
            _mm256_set1_ps(std::numeric_limits<float>::infinity());
        reg1 = _mm256_blend_ps(infinity, reg1, 0b00011111);
    } break;
    case 5: {
        mask = _mm256_set_epi32(0, 0, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD);
        reg1 = _mm256_maskload_ps(p, mask);
        key = _mm256_maskload_epi32(k, mask);
        __m256 infinity =
            _mm256_set1_ps(std::numeric_limits<float>::infinity());
        reg1 = _mm256_blend_ps(infinity, reg1, 0b00111111);
    } break;
    case 6: {
        mask = _mm256_set_epi32(0, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD);
        reg1 = _mm256_maskload_ps(p, mask);
        key = _mm256_maskload_epi32(k, mask);
        __m256 infinity =
            _mm256_set1_ps(std::numeric_limits<float>::infinity());
        reg1 = _mm256_blend_ps(infinity, reg1, 0b01111111);
    } break;
    };
}

/** @brief compared vectors from top and bottom of array and then gradually
 * compare inner vectors.
 * @param arr pointer to the float array to be sorted
 * @param start index of the first element to be sorted
 * @param end index of the last element to be sorted
 */
inline void compare_full_length_all_cases(float *arr, int *keys, unsigned start,
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
        int *k1 = keys + start + i;
        int *k2 = keys + last_vec_to_load;

        __m256 vec2;
        __m256i key2;
        __m256i mask;

        if (UNLIKELY(diff < 7))
            maskload(diff, p2, k2, mask, vec2, key2);
        else {
            vec2 = _mm256_load_ps(p2);
            key2 = _mm256_load_si256((__m256i *)k2);
        }

        {
            __m256 vec1 = _mm256_load_ps(p1);
            __m256i key1 = _mm256_load_si256((__m256i *)k1);

            reverse_and_compare(vec1, vec2, key1, key2);

            __m256 reversed_halves =
                _mm256_permute2f128_ps(vec1, vec1, 0b00000001);
            vec1 =
                _mm256_shuffle_ps(reversed_halves, reversed_halves, 0b00011011);
            _mm256_store_ps(p1, vec1);

            __m256i reversed_halves_key =
                _mm256_permute2f128_si256(key1, key1, 0b00000001);
            key1 = _mm256_shuffle_epi32(reversed_halves_key, 0b00011011);

            _mm256_store_si256((__m256i *)k1, key1);
            _mm256_store_ps(p1, vec1);

            if (UNLIKELY(diff <= 6)) {
                _mm256_maskstore_ps(p2, mask, vec2);
                _mm256_maskstore_epi32(k2, mask, key2);
            } else {
                _mm256_store_ps(p2, vec2);
                _mm256_store_si256((__m256i *)k2, key2);
            }
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
inline void lane_crossing_compare_all_cases(float *arr, int *keys,
                                            unsigned start, unsigned end,
                                            unsigned last_index,
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
        __m256i key;
        __m256i load_store_mask;
        if (diff < 7) {
            maskload(diff, arr + start, keys + start, load_store_mask, reg,
                     key);
        } else {
            reg = _mm256_load_ps(arr + start);
            key = _mm256_load_si256((__m256i *)(keys + start));
        }
        __m256i mask_epi32 =
            _mm256_set_epi32(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                             0x00000000, 0x00000000, 0x00000000, 0x00000000);
        reverse_halves_and_compare(reg, key, mask_epi32);

        mask_epi32 =
            _mm256_set_epi32(0xffffffff, 0xffffffff, 0x00000000, 0x00000000,
                             0xffffffff, 0xffffffff, 0x00000000, 0x00000000);
        shuffle_and_compare(reg, key, mask_epi32, 0b01001110);

        mask_epi32 =
            _mm256_set_epi32(0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
                             0xffffffff, 0x00000000, 0xffffffff, 0x00000000);

        shuffle_and_compare(reg, key, mask_epi32, 0b10110001);

        if (diff < 7) {
            _mm256_maskstore_ps(arr + start, load_store_mask, reg);
            _mm256_maskstore_epi32(keys + start, load_store_mask, key);
        } else {
            _mm256_store_ps(arr + start, reg);
            _mm256_store_si256((__m256i *)(keys + start), key);
        }
        return;
    }

    float *p = arr + start;
    int *k = keys + start;

    for (unsigned i = 0; i < length / 2; i += 8) {
        int diff = last_index - (start + length / 2 + i);
        if (UNLIKELY(diff < 0))
            break;
        __m256 reg1;
        __m256i key1;
        __m256i load_store_mask;
        float *p2 = p + length / 2 + i;
        int *k2 = k + length / 2 + i;

        if (UNLIKELY(diff < 7))
            maskload(diff, p2, k2, load_store_mask, reg1, key1);
        else {
            reg1 = _mm256_load_ps(p2);
            key1 = _mm256_load_si256((__m256i *)k2);
        }
        float *p1 = p + i;
        int *k1 = k + i;

        __m256 reg0 = _mm256_load_ps(p1); // i-ti od začetka
        __m256i key0 = _mm256_load_si256((__m256i *)k1);

        __m256 mask = _mm256_cmp_ps(reg0, reg1, _CMP_LE_OQ);
        __m256 tmp = _mm256_blendv_ps(reg1, reg0, mask);
        reg1 = _mm256_blendv_ps(reg0, reg1, mask);
        reg0 = tmp;
        __m256i tmp_key = _mm256_blendv_epi32(key1, key0, mask);
        key1 = _mm256_blendv_epi32(key0, key1, mask);
        key0 = tmp_key;

        _mm256_store_si256((__m256i *)k1, key0);
        _mm256_store_ps(p1, reg0);

        if (UNLIKELY(diff < 7)) {
            _mm256_maskstore_ps(p2, load_store_mask, reg1);
            _mm256_maskstore_epi32(k2, load_store_mask, key1);
        } else {
            _mm256_store_ps(p2, reg1);
            _mm256_store_si256((__m256i *)k2, key1);
        }
    }
    lane_crossing_compare_all_cases(arr, keys, start, (start + end) / 2,
                                    last_index, depth + 1);
    lane_crossing_compare_all_cases(arr, keys, (start + end) / 2 + 1, end,
                                    last_index, depth + 1);
};

/**
 * @brief The function sorts an array with 2^n elements
 * @param array pointer to the start of the array
 * @param start index of the first number to be sorted
 * @param end index of the last number to be sorted
 * @details end-start+1 should be 2^n
 */
inline void sort_vector(aligned_vector<float> &array, aligned_vector<int> &keys,
                        unsigned start, unsigned end) {

    unsigned full_length = end - start + 1;

    if (full_length <= 1)
        return;
    else if (full_length < 8) {
        __m256i mask;
        __m256 reg;
        __m256i key;
        maskload(full_length - 1, array.data(), keys.data(), mask, reg, key);
        bitonic_sort(reg, key);
        _mm256_maskstore_ps(array.data(), mask, reg);
        _mm256_maskstore_epi32(keys.data(), mask, key);
    } else if (!(full_length & (full_length - 1)))
        sort_2n_vector(array.data(), keys.data(), start, end);
    else if (mod8(full_length) == 0)
        sort_8n_vector(array.data(), keys.data(), start, end);
    else if (full_length < 16) {
        __m256i mask, key1, key2;
        __m256 reg1, reg2;
        reg1 = _mm256_load_ps(array.data());
        key1 = _mm256_load_si256((__m256i *)keys.data());

        maskload(full_length - 9, array.data() + 8, keys.data() + 8, mask, reg2,
                 key2);

        bitonic_sort(reg1, reg2, key1, key2);

        _mm256_store_ps(array.data(), reg1);
        _mm256_maskstore_ps(array.data() + 8, mask, reg2);

        _mm256_store_si256((__m256i *)keys.data(), key1);
        _mm256_maskstore_epi32(keys.data() + 8, mask, key2);

    } else {
        int pow2 = (int)std::ceil(std::log2f(end + 1));
        int imaginary_length = (int)std::pow(2, pow2);
        unsigned full_length = end - start + 1;
        unsigned last_index = end;

        for (unsigned i = start; i <= end - 7; i += 8) {
            __m256 vec1 = _mm256_load_ps(array.data() + i);
            __m256i key1 = _mm256_load_si256((__m256i *)(keys.data() + i));

            bitonic_sort(vec1, key1);
            _mm256_store_ps(array.data() + i, vec1);
            _mm256_store_si256((__m256i *)(keys.data() + i), key1);
        }

        ///////////////////////////////// load the partial one
        int reminder = mod8(end);
        float *p = array.data() + end - reminder;
        int *k = keys.data() + end - reminder;
        __m256 reg1;
        __m256i mask, key1;
        maskload(reminder, p, k, mask, reg1, key1);

        bitonic_sort(reg1, key1);

        _mm256_maskstore_ps(p, mask, reg1);
        _mm256_maskstore_epi32(k, mask, key1);

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
                compare_full_length_all_cases(array.data(), keys.data(), n,
                                              n + len - 1, last_index);
                lane_crossing_compare_all_cases(array.data(), keys.data(), n,
                                                n + len - 1, last_index, 0);
            }
        }
    }
}
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////VALUE:  DOUBLE  KEY: LONG INT  IMPLEMENTATION
////////////////////////////////////////////////////////

/**
 * @brief The function blends two __m256i vectors based on a provided mask.
 * @param a first vector to be blended
 * @param second vector to be blended
 * @param mask mask according to which to blend two vector. For more into check
 * Intel intrinsics guide - _mm256_blendv_ps function
 */
inline __attribute__((always_inline)) __m256i
_mm256_blendv_epi64(const __m256i &a, const __m256i &b, const __m256i &mask) {
    return _mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(a),
                                                _mm256_castsi256_pd(b),
                                                _mm256_castsi256_pd(mask)));
}

/**
 * @brief The overload of _mm256_blendv_epi64 function with mask being of
 * __m256d type.
 * @param a first vector to be blended
 * @param second vector to be blended
 * @param mask mask according to which to blend two vector. For more info check
 * Intel intrinsics guide - _mm256_blendv_ps function.
 */
inline __attribute__((always_inline)) __m256i
_mm256_blendv_epi64(const __m256i &a, const __m256i &b, const __m256d &mask) {
    return _mm256_castpd_si256(
        _mm256_blendv_pd(_mm256_castsi256_pd(a), _mm256_castsi256_pd(b), mask));
}

/**
 * @brief shuffles a register according to uint8_t mask and compares shuffled
 * register to the original one. The resulting permutation is then applied to
 * the key.
 * @param reg register to be shuffled and compared
 * @param key key vector to be shuffled based on the result of comparison of
 * shuffled float register to the original one.
 * @param mask_epi32 helper mask to blend two vector. For more info
 * check Intel intrinsics guide - _mm256_blendv_ps function.
 * @param uint8_t mask according to which float register is shuffled.
 */
inline __attribute__((always_inline)) void
shuffle_and_compare(__m256d &reg, __m256i &key, const __m256i &mask_epi64,
                    const uint8_t mask8) {

    __m256d shuffled_reg = _mm256_shuffle_pd(reg, reg, mask8);

    __m256i shuffled_key = _mm256_castpd_si256(_mm256_shuffle_pd(
        _mm256_castsi256_pd(key), _mm256_castsi256_pd(key), mask8));

    __m256d mask = _mm256_cmp_pd(reg, shuffled_reg, _CMP_LE_OQ);

    mask = _mm256_castsi256_pd(
        _mm256_xor_si256(_mm256_castpd_si256(mask), mask_epi64));

    reg = _mm256_blendv_pd(shuffled_reg, reg, mask);
    key = _mm256_blendv_epi64(shuffled_key, key, mask);
}

/**
 * @brief shuffles a register according to uint8_t mask and compares shuffled
 * register to the original one. The resulting permutation is then applied to
 * the key.
 * @param reg register to be shuffled and compared
 * @param key key vector to be shuffled based on the result of comparison of
 * shuffled float register to the original one.
 * @param mask_epi32 helper mask to blend two vector. For more info
 * check Intel intrinsics guide - _mm256_blendv_ps function.
 * @param uint8_t mask according to which float register is shuffled.
 */
inline __attribute__((always_inline)) void
permute_and_compare(__m256d &reg, __m256i &key, const __m256i &mask_epi64,
                    const uint8_t mask8) {

    __m256d permuted_reg = _mm256_permute4x64_pd(reg, mask8);

    __m256i permuted_key = _mm256_castpd_si256(
        _mm256_permute4x64_pd(_mm256_castsi256_pd(key), mask8));

    __m256d mask = _mm256_cmp_pd(reg, permuted_reg, _CMP_LE_OQ);

    mask = _mm256_castsi256_pd(
        _mm256_xor_si256(_mm256_castpd_si256(mask), mask_epi64));

    reg = _mm256_blendv_pd(permuted_reg, reg, mask);
    key = _mm256_blendv_epi64(permuted_key, key, mask);
}

/**
 * @brief The function reverses a register according  and
 * compares reversed register to the original one. The resulting permutation is
 * then applied to the key.
 * @param reg register to be shuffled and compared
 * @param key key vector to be shuffled based on the result of comparison of
 * shuffled float register to the original one.
 * @param mask_epi32 helper mask to blend two vector. For more info
 * check Intel intrinsics guide - _mm256_blendv_ps function.
 */
inline __attribute__((always_inline)) void
reverse_and_compare(__m256d &reg, __m256i &key, const __m256i &mask_epi64) {

    __m256d reversed_reg = _mm256_permute4x64_pd(reg, 0b00011011);
    __m256i reversed_key = _mm256_permute4x64_epi64(key, 0b00011011);

    __m256d mask = _mm256_cmp_pd(reg, reversed_reg, _CMP_LE_OQ);

    mask = _mm256_castsi256_pd(
        _mm256_xor_si256(_mm256_castpd_si256(mask), mask_epi64));

    reg = _mm256_blendv_pd(reversed_reg, reg, mask);
    key = _mm256_blendv_epi64(reversed_key, key, mask);
}

/**
 *@brief The function accepts a single __m256d vector and sorts
 *it.
 * @param reg register to be sorted
 */
inline void bitonic_sort(__m256d &reg, __m256i &key) {
    __m256i mask1 = _mm256_set_epi64x(0xffffffffffffffff, 0x0000000000000000,
                                      0xffffffffffffffff, 0x0000000000000000);

    __m256i mask2 = _mm256_set_epi64x(0xffffffffffffffff, 0xffffffffffffffff,
                                      0x0000000000000000, 0x0000000000000000);

    shuffle_and_compare(reg, key, mask1, 0b0101);
    reverse_and_compare(reg, key, mask2);
    shuffle_and_compare(reg, key, mask1, 0b0101);
}

/**
 * @brief The function reverses one of the input registers and compares the
 * reversed register to the other supplied register. The resulting permutation
 * is then applied to the key1 and key2 pair.
 * @param reg0 register to be shuffled and compared to be reversed
 * @param reg1 register bo be compared to register reg0
 * @param key1 register of keys to be sorted
 * @param key2 register of keys to be sorted
 */
inline __attribute__((always_inline)) void reverse_and_compare(__m256d &reg0,
                                                               __m256d &reg1,
                                                               __m256i &key0,
                                                               __m256i &key1) {

    __m256d reversed_reg = _mm256_permute4x64_pd(reg0, 0b00011011);
    __m256i reversed_key = _mm256_permute4x64_epi64(key0, 0b00011011);

    __m256d mask = _mm256_cmp_pd(reg1, reversed_reg, _CMP_LE_OQ);

    reg0 = _mm256_blendv_pd(reversed_reg, reg1, mask);
    reg1 = _mm256_blendv_pd(reg1, reversed_reg, mask);

    key0 = _mm256_blendv_epi64(reversed_key, key1, _mm256_castpd_si256(mask));
    key1 = _mm256_blendv_epi64(key1, reversed_key, _mm256_castpd_si256(mask));
}

/**
 * @brief The function accepts two __m256d vectors and sorts them.
 * @param reg1 upper vector of numbers - at the end it contains larger
 * values, the largest value is in the upper half of register [255:192]
 *
 * @param reg0 lower vector of numbers - at the end it contains smaller
 * values. The smallest value is in the lowest half of register - at
 * [63:0]
 */
inline void bitonic_sort(__m256d &reg0, __m256d &reg1, __m256i &key0,
                         __m256i &key1) {
    bitonic_sort(reg1, key1); // sort first register
    bitonic_sort(reg0, key0); // sort second register

    __m256i mask1 = _mm256_set_epi64x(0xffffffffffffffff, 0xffffffffffffffff,
                                      0x0000000000000000, 0x0000000000000000);

    __m256i mask2 = _mm256_set_epi64x(0xffffffffffffffff, 0x0000000000000000,
                                      0xffffffffffffffff, 0x0000000000000000);

    reverse_and_compare(reg0, reg1, key0, key1);
    permute_and_compare(reg1, key1, mask1, 0b01001110);
    shuffle_and_compare(reg1, key1, mask2, 0b0101);

    permute_and_compare(reg0, key0, mask1, 0b01001110);
    shuffle_and_compare(reg0, key0, mask2, 0b0101);
    return;
}

/**
 * @brief The function reverses one of the input registers and compares the
 * reversed register to the other supplied register. The resulting permutation
 * is then applied to the key1 and key2 pair.
 * @param reg0 register to be shuffled and compared to be reversed
 * @param reg1 register bo be compared to register reg0
 * @param key1 register of keys to be sorted
 * @param key2 register of keys to be sorted
 */
inline __attribute__((always_inline)) void
compare(__m256d &reg0, __m256d &reg1, __m256i &key0, __m256i &key1) {

    __m256d mask = _mm256_cmp_pd(reg1, reg0, _CMP_LE_OQ);

    reg0 = _mm256_blendv_pd(reg0, reg1, mask);
    reg1 = _mm256_blendv_pd(reg1, reg0, mask);

    key0 = _mm256_blendv_epi64(key0, key1, _mm256_castpd_si256(mask));
    key1 = _mm256_blendv_epi64(key1, key0, _mm256_castpd_si256(mask));
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
                         __m256d &reg3, __m256i &key0, __m256i &key1,
                         __m256i &key2, __m256i &key3) {
    bitonic_sort(reg0, key0); // sort first register
    bitonic_sort(reg1, key1); // sort second register
    bitonic_sort(reg2, key2); // sort third register
    bitonic_sort(reg3, key3); // sort fourth register

    bitonic_sort(reg0, reg1, key0, key1); // sort third register
    bitonic_sort(reg2, reg3, key2, key3); // sort fourth register

    reverse_and_compare(reg0, reg3, key0, key3);
    reverse_and_compare(reg1, reg2, key1, key2);

    compare(reg0, reg1, key0, key1);
    compare(reg2, reg3, key2, key3);

    __m256i mask_epi64 =
        _mm256_set_epi64x(0xffffffffffffffff, 0xffffffffffffffff,
                          0x0000000000000000, 0x0000000000000000);

    permute_and_compare(reg0, key0, mask_epi64, 0b01001110);
    permute_and_compare(reg1, key1, mask_epi64, 0b01001110);
    permute_and_compare(reg2, key2, mask_epi64, 0b01001110);
    permute_and_compare(reg3, key3, mask_epi64, 0b01001110);

    mask_epi64 = _mm256_set_epi64x(0xffffffffffffffff, 0x0000000000000000,
                                   0xffffffffffffffff, 0x0000000000000000);
    // shuffle neighbour numbers

    shuffle_and_compare(reg0, key0, mask_epi64, 0b0101);
    shuffle_and_compare(reg1, key1, mask_epi64, 0b0101);
    shuffle_and_compare(reg2, key2, mask_epi64, 0b0101);
    shuffle_and_compare(reg3, key3, mask_epi64, 0b0101);

    return;
}

} // namespace BITONIC_SORT_KEY_VALUE
#endif
#endif
