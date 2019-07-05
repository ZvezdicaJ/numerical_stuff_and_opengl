#pragma once
#include "apex_memmove.h"
#include <iostream>
#include <cstring>
#include <iomanip>
#include <fstream>
#include "type_definitions.hpp"
#include "print_simd_vectors.hpp"

enum class FORCE { UNIFORM, CUSTOM };

template <FORCE K, typename T>
class PoissonSOR {
    static_assert(std::is_same<float, T>::value ||
                      std::is_same<double, T>::value,
                  "Poisson SOR can only be instantiated with float "
                  "or double types ");
    unsigned size1; // number of rows
    unsigned size2; // number of coulmns
    T *p12;         // this is pointer to two 2D arrays for
                    // storage of calulations
    T *result;

    T dx, dy, alpha12, alpha34, alpha0;

    T q;
    T omega;
    T *q_arr;
    T max_change = 0;
    aligned_vector<T> max_eigenvalue_data;

  public:
    PoissonSOR(unsigned size_, const T dx_, const T dy_)
        : size1(size_), size2(size_), dx(dx_), dy(dy_) {
        p12 =
            (T *)std::aligned_alloc(32, sizeof(T) * size_ * size_);

        if constexpr (K == FORCE::CUSTOM) {
            q = (T *)std::aligned_alloc(32,
                                        sizeof(T) * size_ * size_);

            memset((void *)q, 0, sizeof(T) * size1 * size2);
        }
        max_eigenvalue_data.reserve(2000);

        memset((void *)p12, 0, sizeof(T) * size1 * size2);
        alpha12 = 1 / (dx * dx);
        alpha34 = 1 / (dy * dy);
        alpha0 = 2 * (1 / (dx * dx) + 1 / (dy * dy));
    };

    PoissonSOR(unsigned size1_, unsigned size2_, const T dx_,
               const T dy_)
        : size1(size1_), size2(size2_), dx(dx_), dy(dy_) {

        p12 = (T *)std::aligned_alloc(32,
                                      sizeof(T) * size1_ * size2_);
        if constexpr (K == FORCE::CUSTOM) {
            q = (T *)std::aligned_alloc(32, sizeof(T) * size1_ *
                                                size2_);

            memset((void *)q, 0, sizeof(T) * size1_ * size2_);
        }
        max_eigenvalue_data.reserve(2000);
        memset((void *)p12, 0, sizeof(T) * size1_ * size2_);
        alpha12 = 1.0 / (dx * dx);
        alpha34 = 1.0 / (dy * dy);
        alpha0 = 2 * (1 / (dx * dx) + 1 / (dy * dy));
    };

    void set_q(const T q_) { q = q_; }
    void set_omega(const T omega_) { omega = omega_; }
    T get_max_change() { return max_change; }
    void reset();

    template <typename ft = T>
    std::enable_if_t<std::is_same_v<ft, float>> iteration();

    template <typename ft = T>
    std::enable_if_t<std::is_same_v<ft, double>> iteration();

    void solve();

    void set_boundary_conditions_uniform(const T);
    T calculate_max_eigenvalue_approximation();

    void set_boundary_conditions(const T *first_row,
                                 const T *last_row,
                                 const T *first_column,
                                 const T *last_column);
    void print_solution(int);
    void print_solution(std::string filename);
};

template <FORCE K, typename T>
template <typename ft>
inline std::enable_if_t<std::is_same_v<ft, float>>
PoissonSOR<K, T>::iteration() {

    max_change = 0.0;
    __m128i mul_pattern = _mm_set1_epi32(size1);

    __m128i add_pattern = _mm_set_epi32(size1, -size1, 1, -1);

    // print_sse(add_pattern, "add_pattern: ");
    __m128 alpha_vec =
        _mm_set_ps(alpha34, alpha34, alpha12, alpha12);

    __m128 factors = _mm_set_ps(alpha34, alpha34, alpha12, alpha12);

    // std::cout << "alpha0 " << alpha0 << std::endl;
    // i counts x direction
    // j counts y direction

    for (int i = 1; i < size1 - 1; i++) {
        for (int j = 1; j < size2 - 1; j++) {

            T &qref = q;
            if constexpr (K == FORCE::UNIFORM) {
                qref = q;
            } else if constexpr (K == FORCE::CUSTOM) {
                qref = *(q_arr + j * size1 + i);
            }
            // i tells which column we are looking at
            __m128i i_vec = _mm_set1_epi32(i);
            // j tells which row we are looking at
            __m128i j_vec = _mm_set1_epi32(j);

            __m128i indexes_to_load = _mm_add_epi32(
                _mm_add_epi32(_mm_mullo_epi32(j_vec, mul_pattern),
                              add_pattern),
                i_vec);

            // std::cout << "\ni: " << i << " j: " << j <<
            // std::endl;
            // print_sse(indexes_to_load, "indexes to load: ");

            __m128 neighbours =
                _mm_i32gather_ps(p12, indexes_to_load, 4);

            // print_sse(neighbours, "neighours: ");
            __m128 product = _mm_mul_ps(alpha_vec, neighbours);
            // print_sse(product,  "product: ");
            __m128 sum =
                _mm_add_ps(product, _mm_shuffle_ps(product, product,
                                                   0b00011011));

            sum = _mm_add_ps(sum,
                             _mm_shuffle_ps(sum, sum, 0b10110001));
            // print_sse(sum, "sum: ");

            // transition to single array
            float new_value =
                *(p12 + size1 * j + i) +
                omega * ((qref + _mm_cvtss_f32(sum)) / alpha0 -
                         *(p12 + size1 * j + i));

            float change =
                std::fabs(*(p12 + size1 * j + i) - new_value);
            if (change > max_change)
                max_change = change;

            *(p12 + size1 * j + i) = new_value;

            // std::cout << "new val: " << *(p12 + +size1 * j + i)
            //          << std::endl;
        };
        // apex::memcpy((void *)p12, (void *)(p12 + size1 * size2),
        //             size1 * size2 * sizeof(T));
    };
    max_eigenvalue_data.push_back(
        *(p12 + size1 * size2 / 2 + size1 / 2));
};

template <FORCE K, typename T>
template <typename ft>
inline std::enable_if_t<std::is_same_v<ft, double>>
PoissonSOR<K, T>::iteration() {

    max_change = 0.0;
    __m128i mul_pattern = _mm_set1_epi32(size1);

    __m128i add_pattern = _mm_set_epi32(size1, -size1, 1, -1);

    // print_sse(add_pattern, "add_pattern: ");
    __m256d alpha_vec =
        _mm256_set_pd(alpha34, alpha34, alpha12, alpha12);

    __m256d factors =
        _mm256_set_pd(alpha34, alpha34, alpha12, alpha12);

    // std::cout << "alpha0 " << alpha0 << std::endl;
    // i counts x direction
    // j counts y direction

    for (int i = 1; i < size1 - 1; i++) {
        for (int j = 1; j < size2 - 1; j++) {

            T &qref = q;
            if constexpr (K == FORCE::UNIFORM) {
                qref = q;
            } else if constexpr (K == FORCE::CUSTOM) {
                qref = *(q_arr + j * size1 + i);
            }
            // i tells which column we are looking at
            __m128i i_vec = _mm_set1_epi32(i);
            // j tells which row we are looking at
            __m128i j_vec = _mm_set1_epi32(j);

            __m128i indexes_to_load = _mm_add_epi32(
                _mm_add_epi32(_mm_mullo_epi32(j_vec, mul_pattern),
                              add_pattern),
                i_vec);

            // std::cout << "\ni: " << i << " j: " << j <<
            // std::endl;
            // print_sse(indexes_to_load, "indexes to load: ");

            __m256d neighbours =
                _mm256_i32gather_pd(p12, indexes_to_load, 8);

            // print_sse(neighbours, "neighours: ");
            __m256d product = _mm256_mul_pd(alpha_vec, neighbours);
            // print_sse(product,  "product: ");
            __m256d sum = _mm256_add_pd(
                product, _mm256_permute2f128_pd(product, product,
                                                0b00000001));
            // print_avx(sum, "sum1: ");
            sum = _mm256_add_pd(
                sum, _mm256_shuffle_pd(sum, sum, 0b00010001));
            // print_avx(sum, "sum2: ");

            // transition to single array
            double new_value =
                *(p12 + size1 * j + i) +
                omega * ((qref + _mm256_cvtsd_f64(sum)) / alpha0 -
                         *(p12 + size1 * j + i));

            double change =
                std::fabs(*(p12 + size1 * j + i) - new_value);
            if (change > max_change)
                max_change = change;

            *(p12 + size1 * j + i) = new_value;
        };
    };
    max_eigenvalue_data.push_back(
        *(p12 + size1 * size2 / 2 + size1 / 2));
};

template <FORCE K, typename T>
inline void
PoissonSOR<K, T>::set_boundary_conditions_uniform(const T value) {

    typename std::conditional<std::is_same<T, float>::value, __m256,
                              __m256d>::type value_vec;
    int increment;

    if constexpr (std::is_same_v<T, float>) {
        value_vec = _mm256_broadcast_ss(&value);
        increment = 8;
    } else if constexpr (std::is_same_v<T, double>) {
        value_vec = _mm256_broadcast_sd(&value);
        increment = 4;
    }

    T *po = p12 + size1 * size2 - size1;

    for (int i = 0; i < (int)size1 - 7; i += increment) {
        // std::cout << "i: " << i << std::endl;
        _mm256_storeu_pd((p12 + i), value_vec);

        _mm256_storeu_pd((po + i), value_vec);
    }

    int reminder = size1 % increment;
    // std::cout << "reminder: " << reminder << std::endl;
    for (int i = reminder; i > 0; i--) {
        *(p12 + size1 - i) = value;
        *(po + size1 - i) = value;
    }

    for (int i = 1; i < size2; i++) {

        // start of rows
        *(p12 + size1 * i) = value;
        // end of rows
        *(p12 + size1 * i - 1) = value;
    }
}

template <FORCE K, typename T>
inline void PoissonSOR<K, T>::set_boundary_conditions(
    const T *first_row, const T *last_row, const T *first_column,
    const T *last_column) {

    // apex::memcpy(dst, src, size);

    apex::memcpy((void *)p12, first_row, size1 * sizeof(T));

    apex::memcpy((void *)(p12 + size1 * size2 - size1), last_row,
                 size1 * sizeof(T));

    for (int i = 1; i < size2; i++) {
        // first column
        *(p12 + size1 * i) = *(first_column + i);

        // last column
        *(p12 + size1 * i - 1) = *(last_column + i);
    }
}

template <FORCE K, typename T>
inline void PoissonSOR<K, T>::print_solution(int sol_num) {

    std::cout << std::setprecision(3);
    for (int i = 0; i < size2; i++) {
        for (int j = 0; j < size1; j++) {

            std::cout << *(p12 + i * size1 + j) << "   ";
        }
        std::cout << std::endl;
    }
};

template <FORCE K, typename T>
inline void PoissonSOR<K, T>::print_solution(std::string filename) {
    std::ofstream file;
    file.open(filename.c_str());
    file << "x y value" << std::endl;
    for (int j = 0; j < size2; j++) {
        for (int i = 0; i < size1; i++) {
            file << dx * i << " " << dy * j << " "
                 << *(p12 + size1 * j + i) << std::endl;
        }
    }
}

template <FORCE K, typename T>
inline T
PoissonSOR<K, T>::calculate_max_eigenvalue_approximation() {

    int iters = max_eigenvalue_data.size();
    return (max_eigenvalue_data[iters - 1] -
            max_eigenvalue_data[iters - 2]) /
           (max_eigenvalue_data[iters - 2] -
            max_eigenvalue_data[iters - 3]);
}

template <FORCE K, typename T>
inline void PoissonSOR<K, T>::reset() {

    int s = size1 - 2;

    for (int j = 1; j < size2 - 1; j++) {
        memset((void *)(p12 + j * size1 + 1), 0, sizeof(T) * s);
    }
}
