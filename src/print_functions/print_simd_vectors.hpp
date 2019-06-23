#pragma once
#include "immintrin.h"
#include <iostream>

inline void print_sse(__m128 reg, std::string name = "") {
    std::cout << name << " ";
    float *r = (float *)&reg;
    std::cout << *r << " " << *(r + 1) << " " << *(r + 2) << " "
              << *(r + 3) << std::endl;
}

inline void print_sse(__m128i reg, std::string name = "") {
    std::cout << name << " ";
    int *r = (int *)&reg;
    std::cout << *r << " " << *(r + 1) << " " << *(r + 2) << " "
              << *(r + 3) << std::endl;
}

inline void print_avx(__m256d reg, std::string name = "") {
    std::cout << name << " ";
    double *r = (double *)&reg;
    std::cout << *r << " " << *(r + 1) << " " << *(r + 2) << " "
              << *(r + 3) << std::endl;
}

inline void print_avx(__m256 reg, std::string name = "") {
    std::cout << name << " ";
    float *r = (float *)&reg;
    std::cout << *r << " " << *(r + 1) << " " << *(r + 2) << " "
              << *(r + 3) << " " << *(r + 4) << " " << *(r + 5)
              << " " << *(r + 6) << " " << *(r + 7) << std::endl;
}

inline void print_avx(__m256i reg, std::string name = "") {
    std::cout << name << " ";
    int *r = (int *)&reg;
    std::cout << *r << " " << *(r + 1) << " " << *(r + 2) << " "
              << *(r + 3) << " " << *(r + 4) << " " << *(r + 5)
              << " " << *(r + 6) << " " << *(r + 7) << std::endl;
}
