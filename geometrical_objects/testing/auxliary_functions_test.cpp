#include "standard_includes.hpp"
#include "../src/auxiliary_functions.hpp"


void test_chebyshev() {
    __m128 x_vec;
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
        x_vec = _mm_set_ps(-1.0 + 4*i * 0.01, -1.0 + (4*i + 1) * 0.01,
                           -1.0 + (4*i + 2) * 0.01, -1.0 + (4*i + 3) * 0.01);
        __m128 result0 = chebyshev(0, x_vec);
        __m128 result1 = chebyshev(1, x_vec);
        __m128 result2 = chebyshev(2, x_vec);
        __m128 result3 = chebyshev(3, x_vec);
        __m128 result4 = chebyshev(4, x_vec);
        __m128 result5 = chebyshev(5, x_vec);

        result0=_mm_shuffle_ps(result0, result0, _MM_SHUFFLE(0, 1, 2, 3));
        result1=_mm_shuffle_ps(result1, result1, _MM_SHUFFLE(0, 1, 2, 3));
        result2=_mm_shuffle_ps(result2, result2, _MM_SHUFFLE(0, 1, 2, 3));
        result3=_mm_shuffle_ps(result3, result3, _MM_SHUFFLE(0, 1, 2, 3));
        result4=_mm_shuffle_ps(result4, result4, _MM_SHUFFLE(0, 1, 2, 3));
        result5=_mm_shuffle_ps(result5, result5, _MM_SHUFFLE(0, 1, 2, 3));


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

int main() {
    test_chebyshev();
    return 0;
}
