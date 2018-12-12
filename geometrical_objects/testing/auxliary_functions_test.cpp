#include "standard_includes.hpp"

void test_chebyshev() {

    int num_points = 200;
    std::ofstream out_file0("../testing/data/chebyshev0.dat", std::ios::out);
    if (!out_file0.is_open())
        std::cout << "failed to open file0" << std::endl;
    std::ofstream out_file1("../testing/data/chebyshev1.dat", std::ios::out);
    if (!out_file1.is_open())
        std::cout << "failed to open file1" << std::endl;
    std::ofstream out_file2("../testing/data/chebyshev2.dat", std::ios::out);
    if (!out_file2.is_open())
        std::cout << "failed to open file2" << std::endl;
    std::ofstream out_file3("../testing/data/chebyshev3.dat", std::ios::out);
    if (!out_file3.is_open())
        std::cout << "failed to open file3" << std::endl;
    std::ofstream out_file4("../testing/data/chebyshev4.dat", std::ios::out);
    if (!out_file4.is_open())
        std::cout << "failed to open file4" << std::endl;
    std::ofstream out_file5("../testing/data/chebyshev5.dat", std::ios::out);
    if (!out_file5.is_open())
        std::cout << "failed to open file5" << std::endl;

    for (int i = 0; i < 50; i += 1) {
        __m128 x_vec =
            _mm_set_ps(-1.0 + 4 * i * 0.01, -1.0 + (4 * i + 1) * 0.01,
                       -1.0 + (4 * i + 2) * 0.01, -1.0 + (4 * i + 3) * 0.01);
        __m128 result0 = chebyshev(0, x_vec);
        __m128 result1 = chebyshev(1, x_vec);
        __m128 result2 = chebyshev(2, x_vec);
        __m128 result3 = chebyshev(3, x_vec);
        __m128 result4 = chebyshev(4, x_vec);
        __m128 result5 = chebyshev(5, x_vec);

        result0 = _mm_shuffle_ps(result0, result0, _MM_SHUFFLE(0, 1, 2, 3));
        result1 = _mm_shuffle_ps(result1, result1, _MM_SHUFFLE(0, 1, 2, 3));
        result2 = _mm_shuffle_ps(result2, result2, _MM_SHUFFLE(0, 1, 2, 3));
        result3 = _mm_shuffle_ps(result3, result3, _MM_SHUFFLE(0, 1, 2, 3));
        result4 = _mm_shuffle_ps(result4, result4, _MM_SHUFFLE(0, 1, 2, 3));
        result5 = _mm_shuffle_ps(result5, result5, _MM_SHUFFLE(0, 1, 2, 3));

        float *r0 = (float *)&result0;
        float *r1 = (float *)&result1;
        float *r2 = (float *)&result2;
        float *r3 = (float *)&result3;
        float *r4 = (float *)&result4;
        float *r5 = (float *)&result5;

        out_file0 << *(r0) << " " << *(r0 + 1) << "  " << *(r0 + 2) << " "
                  << *(r0 + 3) << " ";
        out_file1 << *(r1) << " " << *(r1 + 1) << " " << *(r1 + 2) << " "
                  << *(r1 + 3) << " ";
        out_file2 << *(r2) << " " << *(r2 + 1) << " " << *(r2 + 2) << " "
                  << *(r2 + 3) << " ";
        out_file3 << *(r3) << " " << *(r3 + 1) << " " << *(r3 + 2) << " "
                  << *(r3 + 3) << " ";
        out_file4 << *(r4) << " " << *(r4 + 1) << " " << *(r4 + 2) << " "
                  << *(r4 + 3) << " ";
        out_file5 << *(r5) << " " << *(r5 + 1) << " " << *(r5 + 2) << " "
                  << *(r5 + 3) << " ";
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
        file << *((int *)&sse_fac) << " ";
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
            _mm_storeu_si128((__m128i *)&sse_factorials[i - 1], sse_fac);
        }
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto microseconds =
        std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
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
        std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    std::cout << "  scalar version took: " << microseconds.count() << "µs\n";

    int sum = 0;
    for (int i = 0; i < num_fac; i++) {
        sum += scalar_factorials[i];
    }
    std::cout << "sumof first 12 factorials: " << sum << std::endl;

    return;
}

void test_chebyshev_next() {
    __m128 T0 = _mm_set_ps1(1.0);
    std::ofstream out_file2("../testing/data/chebyshev2_next_test.dat",
                            std::ios::out);
    if (!out_file2.is_open())
        std::cout << "Unable to open a file in function test_chebyshev_next!"
                  << std::endl;
    std::ofstream out_file3("../testing/data/chebyshev3_next_test.dat",
                            std::ios::out);
    if (!out_file3.is_open())
        std::cout << "Unable to open a file in function test_chebyshev_next!"
                  << std::endl;
    for (int i = 0; i < 50; i += 1) {
        __m128 x_vec;
        x_vec =
            _mm_set_ps(-1.0 + 4 * i * 0.01, -1.0 + (4 * i + 1) * 0.01,
                       -1.0 + (4 * i + 2) * 0.01, -1.0 + (4 * i + 3) * 0.01);
        __m128 T1 = x_vec;
        __m128 T2 = chebyshev_next(T1, T0, x_vec);
        __m128 T3 = chebyshev_next(T2, T1, x_vec);
        T2 = _mm_shuffle_ps(T2, T2, _MM_SHUFFLE(0, 1, 2, 3));
        T3 = _mm_shuffle_ps(T3, T3, _MM_SHUFFLE(0, 1, 2, 3));

        float *r = (float *)&T2;
        out_file2 << *(r) << " " << *(r + 1) << "  " << *(r + 2) << " "
                  << *(r + 3) << " ";
        r = (float *)&T3;
        out_file3 << *(r) << " " << *(r + 1) << "  " << *(r + 2) << " "
                  << *(r + 3) << " ";
    }
}

void test_sse_cos() {
    __m128 T0 = _mm_set_ps1(1.0);
    std::ofstream out_file0("../testing/data/sse_cos_test.dat", std::ios::out);
    if (!out_file0.is_open())
        std::cout << "Unable to open a file in function test_sse_cos!"
                  << std::endl;
    float min = -M_PI;
    float max = M_PI;
    int steps = 100;
    float korak = (max - min) / (float)(steps - 1);

    for (int i = 0; i < steps / 4; i += 1) {
        __m128 x_vec;
        x_vec =
            _mm_set_ps(min + 4 * i * korak, min + (4 * i + 1) * korak,
                       min + (4 * i + 2) * korak, min + (4 * i + 3) * korak);
        ;
        __m128 cos = sse_cos(x_vec);
        cos = _mm_shuffle_ps(cos, cos, _MM_SHUFFLE(0, 1, 2, 3));
        float *r = (float *)&cos;
        out_file0 << *(r) << " " << *(r + 1) << "  " << *(r + 2) << " "
                  << *(r + 3) << " ";
    }
}

void test_sse_sin() {
    __m128 T0 = _mm_set_ps1(1.0);
    std::ofstream out_file0("../testing/data/sse_sin_test.dat", std::ios::out);
    if (!out_file0.is_open())
        std::cout << "Unable to open a file in function test_sse_sin!"
                  << std::endl;
    float min = -M_PI;
    float max = M_PI;
    int steps = 100;
    float korak = (max - min) / (float)(steps - 1);

    for (int i = 0; i < steps / 4; i += 1) {
        __m128 x_vec;
        x_vec =
            _mm_set_ps(min + 4 * i * korak, min + (4 * i + 1) * korak,
                       min + (4 * i + 2) * korak, min + (4 * i + 3) * korak);
        ;
        __m128 sin = sse_sin(x_vec);
        sin = _mm_shuffle_ps(sin, sin, _MM_SHUFFLE(0, 1, 2, 3));
        float *r = (float *)&sin;
        out_file0 << *(r) << " " << *(r + 1) << "  " << *(r + 2) << " "
                  << *(r + 3) << " ";
    }
}

void test_cross_product() {
    float arr1[4] = {0, 1, 0, 0};
    __m128 a = _mm_load_ps(arr1);

    float arr2[4] = {1, 0, 0, 0};
    __m128 b = _mm_load_ps(arr2);

    __m128 c = cross_product(a, b);
    print_sse_float(c);

    float arr[4] = {1, 0, 0, 0};
    __m128 t = _mm_load_ps(arr);
    print_sse_float(t, "t");
    print_sse_float(a, "a");
}

int main() {
    test_chebyshev();
    test_factorial(1, 10);
    test_chebyshev_next();
    test_sse_cos();
    test_sse_sin();
    test_cross_product();
    return 0;
}
