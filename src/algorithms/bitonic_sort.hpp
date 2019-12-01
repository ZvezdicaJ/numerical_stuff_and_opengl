#pragma once

#ifdef __AVX2__
#include <bitset>

inline __attribute__((always_inline)) int mod2(const int number) {
    return number & 0b00000001;
}

inline __attribute__((always_inline)) int mod4(const int number) {
    return number & 0b00000011;
}

inline __attribute__((always_inline)) int mod8(const int number) {
    return number & 0b00000111;
}

namespace BITONIC_SORT {

static const int LOAD = 0xffffffff;
static const int STORE = 0xffffffff;

} // namespace BITONIC_SORT

// int implementation
#include "bitonic_sort_int.hpp"

// float implementation
#include "bitonic_sort_float.hpp"

// double implementation
#include "bitonic_sort_double.hpp"

#endif
