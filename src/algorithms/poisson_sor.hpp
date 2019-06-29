#pragma once
#include "apex_memmove.h"
#include <iostream>
#include <cstring>
#include <iomanip>

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

  public:
    PoissonSOR(unsigned size_, const T dx_, const T dy_)
        : size1(size_), size2(size_), dx(dx_), dy(dy_) {
        p12 = (T *)std::aligned_alloc(32, 2 * sizeof(T) * size_ *
                                              size_);

        if constexpr (K == FORCE::CUSTOM) {
            q = (T *)std::aligned_alloc(32,
                                        sizeof(T) * size_ * size_);

            memset((void *)q, 0, sizeof(T) * size1 * size2);
        }

        memset((void *)p12, 0, 2 * sizeof(T) * size1 * size2);
        alpha12 = 1 / (dx * dx);
        alpha34 = 1 / (dy * dy);
        alpha0 = 2 / (dx * dx + dy * dy);
    };

    PoissonSOR(unsigned size1_, unsigned size2_, const T dx_,
               const T dy_)
        : size1(size1_), size2(size2_), dx(dx_), dy(dy_) {

        p12 = (T *)std::aligned_alloc(32, 2 * sizeof(T) * size1_ *
                                              size2_);
        if constexpr (K == FORCE::CUSTOM) {
            q = (T *)std::aligned_alloc(32, sizeof(T) * size1_ *
                                                size2_);

            memset((void *)q, 0, sizeof(T) * size1_ * size2_);
        }

        memset((void *)p12, 0, 2 * sizeof(T) * size1_ * size2_);
        alpha12 = 1.0 / (dx * dx);
        alpha34 = 1.0 / (dy * dy);
        alpha0 = 2 / (dx * dx + dy * dy);
    };

    void set_q(const T q_) { q = q_; }
    void set_omega(const T omega_) { omega = omega_; }

    std::enable_if_t<std::is_same_v<T, float>> iteration();
    void solve();
    std::enable_if_t<std::is_same_v<T, float>>
    set_boundary_conditions_uniform(const float);

    void set_boundary_conditions(const T *first_row,
                                 const T *last_row,
                                 const T *first_column,
                                 const T *last_column);
    void print_solution(int);
};

template <FORCE K, typename T>
inline std::enable_if_t<std::is_same_v<T, float>>
PoissonSOR<K, T>::iteration() {

    float *p3 = (float *)std::aligned_alloc(32, 1 * sizeof(float) *
                                                    size1 * size2);

    __m128i mul_pattern = _mm_set1_epi32(size1);

    __m128i add_pattern = _mm_set_epi32(
        size1, size1 * size2 - size1, 1, size1 * size2 - 1);

    __m128 alpha_vec =
        _mm_set_ps(alpha12, alpha12, alpha34, alpha34);

    __m128 factors = _mm_set_ps(alpha34, alpha34, alpha12, alpha12);

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
                _mm_add_epi32(_mm_mul_epi32(j_vec, mul_pattern),
                              add_pattern),
                i_vec);

            __m128 neighbours =
                _mm_i32gather_ps(p12, indexes_to_load, 8);

            __m128 product = _mm_mul_ps(alpha_vec, neighbours);
            // print_sse(product,  "product: ");
            __m128 sum =
                _mm_add_ps(product, _mm_shuffle_ps(product, product,
                                                   0b00011011));

            sum = _mm_add_ps(sum,
                             _mm_shuffle_ps(sum, sum, 0b10110001));
            // print_sse(sum, "sum: ");
            *(p12 + size1 * size2 + size1 * j + i) *
                    (p12 + size1 * size2 + size1 * j + i) +
                omega *((qref + _mm_cvtss_f32(sum)) / alpha0 -
                        *(p12 + size1 * j + i));
        };
        apex::memcpy((void *)p12, (void *)(p12 + size1 * size2),
                     size1 * size2 * sizeof(T));
    };
};

template <FORCE K, typename T>
inline std::enable_if_t<std::is_same_v<T, float>>
PoissonSOR<K, T>::set_boundary_conditions_uniform(
    const float value) {

    __m256 value_vec = _mm256_broadcast_ss(&value);

    float *po2 = p12 + size1 * size2;
    float *po3 = p12 + size1 * size2 - size1;
    float *po4 = p12 + 2 * size1 * size2 - size1;

    for (int i = 0; i < (int)size1 - 7; i += 8) {
        std::cout << "i: " << i << std::endl;
        _mm256_storeu_ps((p12 + i), value_vec);

        _mm256_storeu_ps((po2 + i), value_vec);

        _mm256_storeu_ps((po3 + i), value_vec);

        _mm256_storeu_ps((po4 + i), value_vec);
    }

    int reminder = size1 % 8;
    std::cout << "reminder: " << reminder << std::endl;
    for (int i = reminder; i > 0; i--) {
        *(p12 + size1 - i) = value;
        *(po2 + size1 - i) = value;
        *(po3 + size1 - i) = value;
        *(po4 + size1 - i) = value;
    }

    for (int i = 1; i < 2 * size2; i++) {

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

    apex::memcpy((void *)(p12 + size1 * size2), first_row,
                 size1 * sizeof(T));

    apex::memcpy((void *)(p12 + size1 * size2 - size1), last_row,
                 size1 * sizeof(T));

    apex::memcpy((void *)(p12 + 2 * size1 * size2 - size1),
                 last_row, size1 * sizeof(T));

    for (int i = 1; i < size2; i++) {
        // first column
        *(p12 + size1 * i) = *(first_column + i);
        *(p12 + size1 * size2 + size1 * i) = *(first_column + i);

        // last column
        *(p12 + size1 * i - 1) = *(last_column + i);
        *(p12 + size1 * size2 + size1 * i - 1) = *(last_column + i);
    }
}

template <FORCE K, typename T>
inline void PoissonSOR<K, T>::print_solution(int sol_num) {

    T *p;
    if (sol_num == 1)
        p = p12;
    else if (sol_num == 2)
        p = p12 + size1 * size2;

    std::cout << std::setprecision(3);
    for (int i = 0; i < size2; i++) {
        for (int j = 0; j < size1; j++) {

            std::cout << *(p + i * size1 + j) << "   ";
        }
        std::cout << std::endl;
    }
};
