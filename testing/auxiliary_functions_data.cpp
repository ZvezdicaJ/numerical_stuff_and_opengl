#include <xmmintrin.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <ctime>
#include <string>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <random>
#include <list>
#include <functional>
#include <boost/align/aligned_allocator.hpp> // this is for aligned std::vector

#include "apex_memmove.h"
#include "type_definitions.hpp"
#include "convex_hull.hpp"
#include "auxiliary_functions.hpp"
#include "print_functions.hpp"

static const std::string base_dir("../testing/data/");

void test_chebyshev() {

    int num_points = 200;
    std::ofstream out_file0("../testing/data/chebyshev0.dat",
                            std::ios::out);
    if (!out_file0.is_open())
        std::cout << "failed to open file0" << std::endl;
    std::ofstream out_file1("../testing/data/chebyshev1.dat",
                            std::ios::out);
    if (!out_file1.is_open())
        std::cout << "failed to open file1" << std::endl;
    std::ofstream out_file2("../testing/data/chebyshev2.dat",
                            std::ios::out);
    if (!out_file2.is_open())
        std::cout << "failed to open file2" << std::endl;
    std::ofstream out_file3("../testing/data/chebyshev3.dat",
                            std::ios::out);
    if (!out_file3.is_open())
        std::cout << "failed to open file3" << std::endl;
    std::ofstream out_file4("../testing/data/chebyshev4.dat",
                            std::ios::out);
    if (!out_file4.is_open())
        std::cout << "failed to open file4" << std::endl;
    std::ofstream out_file5("../testing/data/chebyshev5.dat",
                            std::ios::out);
    if (!out_file5.is_open())
        std::cout << "failed to open file5" << std::endl;

    for (int i = 0; i < 50; i += 1) {
        __m128 x_vec = _mm_set_ps(
            -1.0 + 4 * i * 0.01, -1.0 + (4 * i + 1) * 0.01,
            -1.0 + (4 * i + 2) * 0.01, -1.0 + (4 * i + 3) * 0.01);
        __m128 result0 = chebyshev(0, x_vec);
        __m128 result1 = chebyshev(1, x_vec);
        __m128 result2 = chebyshev(2, x_vec);
        __m128 result3 = chebyshev(3, x_vec);
        __m128 result4 = chebyshev(4, x_vec);
        __m128 result5 = chebyshev(5, x_vec);

        result0 = _mm_shuffle_ps(result0, result0,
                                 _MM_SHUFFLE(0, 1, 2, 3));
        result1 = _mm_shuffle_ps(result1, result1,
                                 _MM_SHUFFLE(0, 1, 2, 3));
        result2 = _mm_shuffle_ps(result2, result2,
                                 _MM_SHUFFLE(0, 1, 2, 3));
        result3 = _mm_shuffle_ps(result3, result3,
                                 _MM_SHUFFLE(0, 1, 2, 3));
        result4 = _mm_shuffle_ps(result4, result4,
                                 _MM_SHUFFLE(0, 1, 2, 3));
        result5 = _mm_shuffle_ps(result5, result5,
                                 _MM_SHUFFLE(0, 1, 2, 3));

        float *r0 = (float *)&result0;
        float *r1 = (float *)&result1;
        float *r2 = (float *)&result2;
        float *r3 = (float *)&result3;
        float *r4 = (float *)&result4;
        float *r5 = (float *)&result5;

        out_file0 << *(r0) << " " << *(r0 + 1) << "  " << *(r0 + 2)
                  << " " << *(r0 + 3) << " ";
        out_file1 << *(r1) << " " << *(r1 + 1) << " " << *(r1 + 2)
                  << " " << *(r1 + 3) << " ";
        out_file2 << *(r2) << " " << *(r2 + 1) << " " << *(r2 + 2)
                  << " " << *(r2 + 3) << " ";
        out_file3 << *(r3) << " " << *(r3 + 1) << " " << *(r3 + 2)
                  << " " << *(r3 + 3) << " ";
        out_file4 << *(r4) << " " << *(r4 + 1) << " " << *(r4 + 2)
                  << " " << *(r4 + 3) << " ";
        out_file5 << *(r5) << " " << *(r5 + 1) << " " << *(r5 + 2)
                  << " " << *(r5 + 3) << " ";
    }
    out_file0 << std::endl;
    out_file1 << std::endl;
    out_file2 << std::endl;
    out_file3 << std::endl;
    out_file4 << std::endl;
    out_file5 << std::endl;
}

void test_factorial(int n_min, int n_max) {
    std::string file_path = "../testing/data/";
    std::string file_name = "sse_factorial_test_data.dat";
    std::ofstream file(file_path + file_name, std::ios::out);
    file << "sse_version scalar_version" << std::endl;
    for (int i = n_min; i <= n_max; i++) {
        __m128i sse_fac = sse_factorial(i);
        file << _mm_extract_epi32(sse_fac, 0b00000000) << " ";
        int sca_fac = scalar_factorial(i);
        file << sca_fac << std::endl;
    }

    int num_fac = 12;
    int sse_factorials[num_fac + 4];
    int scalar_factorials[num_fac];
    auto start = std::chrono::high_resolution_clock::now();
    for (int j = 0; j < 10000; j++) {
        for (int i = 1; i <= num_fac; i++) {
            __m128i sse_fac = sse_factorial(i);
            _mm_storeu_si128((__m128i *)&sse_factorials[i - 1],
                             sse_fac);
        }
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto microseconds =
        std::chrono::duration_cast<std::chrono::microseconds>(
            finish - start);
    std::cout << "factorials timing:\n  sse version took: "
              << microseconds.count() << "µs\n";

    start = std::chrono::high_resolution_clock::now();
    int k;
    for (int j = 0; j < 10000; j++) {
        for (int i = 1; i <= num_fac; i++) {
            k = scalar_factorial(i);
            scalar_factorials[i - 1] = k;
        }
    }
    finish = std::chrono::high_resolution_clock::now();
    microseconds =
        std::chrono::duration_cast<std::chrono::microseconds>(
            finish - start);
    std::cout << "  scalar version took: " << microseconds.count()
              << "µs\n";

    int sum = 0;
    for (int i = 0; i < num_fac; i++) {
        sum += scalar_factorials[i];
    }
    std::cout << "sumof first 12 factorials: " << sum << std::endl;

    return;
}

void test_chebyshev_next() {
    __m128 T0 = _mm_set_ps1(1.0);
    std::ofstream out_file2(
        "../testing/data/chebyshev2_next_test.dat", std::ios::out);
    if (!out_file2.is_open())
        std::cout << "Unable to open a file in function "
                     "test_chebyshev_next!"
                  << std::endl;
    std::ofstream out_file3(
        "../testing/data/chebyshev3_next_test.dat", std::ios::out);
    if (!out_file3.is_open())
        std::cout << "Unable to open a file in function "
                     "test_chebyshev_next!"
                  << std::endl;
    for (int i = 0; i < 50; i += 1) {
        __m128 x_vec;
        x_vec = _mm_set_ps(
            -1.0 + 4 * i * 0.01, -1.0 + (4 * i + 1) * 0.01,
            -1.0 + (4 * i + 2) * 0.01, -1.0 + (4 * i + 3) * 0.01);
        __m128 T1 = x_vec;
        __m128 T2 = chebyshev_next(T1, T0, x_vec);
        __m128 T3 = chebyshev_next(T2, T1, x_vec);
        T2 = _mm_shuffle_ps(T2, T2, _MM_SHUFFLE(0, 1, 2, 3));
        T3 = _mm_shuffle_ps(T3, T3, _MM_SHUFFLE(0, 1, 2, 3));

        float *r = (float *)&T2;
        out_file2 << *(r) << " " << *(r + 1) << "  " << *(r + 2)
                  << " " << *(r + 3) << " ";
        r = (float *)&T3;
        out_file3 << *(r) << " " << *(r + 1) << "  " << *(r + 2)
                  << " " << *(r + 3) << " ";
    }
}

void test_cross_product() {
    float arr1[4] = {0, 1, 0, 0};
    __m128 a = _mm_loadu_ps(arr1);

    float arr2[4] = {1, 0, 0, 0};
    __m128 b = _mm_loadu_ps(arr2);

    __m128 c = cross_product(a, b);
    print_sse(c);

    float arr[4] = {1, 0, 0, 0};
    __m128 t = _mm_load_ps(arr);
    print_sse(t, "t");
    print_sse(a, "a");
}

void test_avxd_trigonometric(__m256d (*f)(const __m256d &),
                             double min, double max,
                             std::string file_to_write) {

    __m256d T0 = _mm256_set1_pd(1.0);

    std::ofstream out_file0((base_dir + file_to_write).c_str(),
                            std::ios::out);
    if (!out_file0.is_open())
        std::cout << "Unable to open a file in function test_cos!"
                  << std::endl;
    int steps = 100;
    double korak = (max - min) / (double)(steps - 1);

    for (int i = 0; i < steps / 4; i += 1) {
        __m256d x_vec;
        x_vec = _mm256_setr_pd(
            min + 4 * i * korak, min + (4 * i + 1) * korak,
            min + (4 * i + 2) * korak, min + (4 * i + 3) * korak);
        __m256d val = f(x_vec);
        //        cos = _mm256_shuffle_pd(cos, cos, _MM_SHUFFLE(0,
        //        1, 2, 3));
        double *x = (double *)&x_vec;
        double *r = (double *)&val;
        out_file0 << std::setprecision(18) << *(x) << " " << *(r)
                  << std::endl;
        out_file0 << std::setprecision(18) << *(x + 1) << " "
                  << *(r + 1) << std::endl;
        out_file0 << std::setprecision(18) << *(x + 2) << " "
                  << *(r + 2) << std::endl;
        out_file0 << std::setprecision(18) << *(x + 3) << " "
                  << *(r + 3) << std::endl;
    }
}

void test_ssef_trigonometric(__m128 (*f)(const __m128 &), float min,
                             float max, std::string file_to_write) {

    __m128 T0 = _mm_set1_ps(1.0);

    std::ofstream out_file0((base_dir + file_to_write).c_str(),
                            std::ios::out);
    if (!out_file0.is_open())
        std::cout << "Unable to open a file in function test_cos!"
                  << std::endl;
    int steps = 100;
    float korak = (max - min) / (float)(steps - 1);

    for (int i = 0; i < steps / 4; i += 1) {
        __m128 x_vec;
        x_vec = _mm_setr_ps(
            min + 4 * i * korak, min + (4 * i + 1) * korak,
            min + (4 * i + 2) * korak, min + (4 * i + 3) * korak);
        __m128 val = f(x_vec);
        //        cos = _mm_shuffle_pd(cos, cos, _MM_SHUFFLE(0,
        //        1, 2, 3));
        float *x = (float *)&x_vec;
        float *r = (float *)&val;
        out_file0 << std::setprecision(18) << *(x) << " " << *(r)
                  << std::endl;
        out_file0 << std::setprecision(18) << *(x + 1) << " "
                  << *(r + 1) << std::endl;
        out_file0 << std::setprecision(18) << *(x + 2) << " "
                  << *(r + 2) << std::endl;
        out_file0 << std::setprecision(18) << *(x + 3) << " "
                  << *(r + 3) << std::endl;
    }
}

int main() {
    test_chebyshev();
    test_factorial(1, 10);
    test_chebyshev_next();
    test_cross_product();

    test_avxd_trigonometric(cos, -M_PI, M_PI, "avxd_cos.dat");
    test_ssef_trigonometric(cos, -M_PI, M_PI, "ssef_cos.dat");
    test_avxd_trigonometric(sin, -M_PI, M_PI, "avxd_sin.dat");
    test_ssef_trigonometric(sin, -M_PI, M_PI, "ssef_sin.dat");

    test_avxd_trigonometric(tan, -M_PI, M_PI, "avxd_tan.dat");
    test_ssef_trigonometric(tan, -M_PI, M_PI, "ssef_tan.dat");

    test_avxd_trigonometric(arctan, -1000, 1000, "avxd_arctan.dat");
    test_ssef_trigonometric(arctan, -1000, 1000, "ssef_arctan.dat");

    test_ssef_trigonometric(arccos, -1.0, 1.0, "ssef_arccos.dat");
    test_ssef_trigonometric(arccos_ver2, -1.0, 1.0,
                            "ssef_arccos_ver2.dat");
    test_ssef_trigonometric(arccos_ver3, -1.0, 1.0,
                            "ssef_arccos_ver3.dat");

    __m128 t = _mm_setr_ps(0.4, -0.3, -1.4, -1.27);
    arctan(t);
    return 0;
}
