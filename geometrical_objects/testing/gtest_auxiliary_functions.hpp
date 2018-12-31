#include "../src/auxiliary_functions.hpp"

#ifdef __SSE__
TEST(DotProduct, sse) {
    __m128 a = _mm_set_ps1(1);
    __m128 b = _mm_set_ps1(2);
    ASSERT_THAT(8.0, CalcDotProduct(a, b));
    a = _mm_set_ps(1.0, 2.0, 3.0, 4.0);
    b = _mm_set_ps(-1.0, -2.0, -3.0, -4.0);
    ASSERT_THAT(-30.0, CalcDotProduct(a, b));
}
#endif

#ifdef __AVX2__
TEST(DotProduct, avx) {
    __m256d a = _mm256_setr_pd(1.0, 1.0, 1.0, 1.0);
    __m256d b = _mm256_setr_pd(1.0, 1.0, 1.0, 1.0);
    ASSERT_THAT(3.0, CalcDotProduct(a, b));
    // _set_ function sets the elements in reverse order
    // it is not the same as if you load a vector
    // from array
    // effectively it is then {4.0, 3.0, 2.0, 1.0}
    // that is why one should use setr function
    a = _mm256_setr_pd(1.0, 2.0, 3.0, 4.0);
    b = _mm256_setr_pd(-1.0, -2.0, -3.0, -4.0);
    ASSERT_THAT(-14.0, CalcDotProduct(a, b));
}
#endif

#ifdef __SSE__
TEST(load_store, sse) {
    std::array<float, 4> arr(
        {1, 2, 3, 4}); // gmock needs iterators and size() method, that's why
                       // float[] is not ok
    __m128 sse_vec = _mm_load_ps(arr.data());
    float *p = (float *)&sse_vec;
    std::array<float, 4> check;
    for (int i = 0; i < 4; i++)
        check[i] = p[i];
    EXPECT_THAT(check, testing::ElementsAre(1, 2, 3, 4));
    _mm_storeu_ps(check.data(), sse_vec);
    EXPECT_THAT(check, testing::ElementsAre(1, 2, 3, 4));
}
#endif

#ifdef __AVX2__
TEST(load_store, avx2) {
    std::array<double, 4> arr(
        {1, 2, 3, 4}); // gmock needs iterators and size() method, that's why
    // float[] is not ok
    __m256d avx_vec = _mm256_load_pd(arr.data());
    double *p = (double *)&avx_vec;
    std::array<double, 4> check;
    // std::cout << "p: " << std::endl;
    for (int i = 0; i < 4; i++) {
        check[i] = p[i];
        // std::cout << p[i] << std::endl;
    }
    EXPECT_THAT(check, testing::ElementsAre(1, 2, 3, 4));
    _mm256_storeu_pd(check.data(), avx_vec);
    EXPECT_THAT(check, testing::ElementsAre(1, 2, 3, 4));
}
#endif

#ifdef __FMA__
TEST(cross_product, sse) {
    __m128 a = _mm_setr_ps(1.0, 2.0, 3.0, 0.0);
    __m128 b = _mm_setr_ps(5.0, -2.0, 9.0, 0.0);
    __m128 c = cross_product(a, b);
    float *p = (float *)&c;
    std::array<float, 3> check;
    for (int i = 0; i < 3; i++) {
        check[i] = p[i];
    }
    EXPECT_THAT(check, testing::ElementsAre(24, 6, -12));
}
#endif

#ifdef __AVX2__
TEST(cross_product, avx2) {
    __m256d a = _mm256_setr_pd(1.0, 2.0, 3.0, 0.0);
    __m256d b = _mm256_setr_pd(5.0, -2.0, 9.0, 0.0);
    __m256d c = cross_product(a, b);
    double *p = (double *)&c;
    std::array<double, 3> check;
    for (int i = 0; i < 3; i++) {
        check[i] = p[i];
    }
    EXPECT_THAT(check, testing::ElementsAre(24, 6, -12));
}
#endif

TEST(factorial, scalar) {
    ASSERT_EQ(1, scalar_factorial(1));
    ASSERT_EQ(2, scalar_factorial(2));
    ASSERT_EQ(6, scalar_factorial(3));
    ASSERT_EQ(24, scalar_factorial(4));
}

TEST(chebyshev, sse) {
    std::array<float, 4> x_vec({1, 2, 3, 0});
    __m128 x = _mm_loadu_ps(x_vec.data());
    __m128 result = chebyshev(1, x);
    float *p = (float *)&result;
    std::array<float, 4> check;
    for (int i = 0; i < 4; i++) {
        check[i] = p[i];
    }
    ASSERT_THAT(check, testing::ElementsAre(1.0, 2.0, 3.0, 0.0));

    result = chebyshev(0, x);
    p = (float *)&result;
    for (int i = 0; i < 4; i++) {
        check[i] = p[i];
    }
    ASSERT_THAT(check, testing::ElementsAre(1.0, 1.0, 1.0, 1.0));

    x = _mm_setr_ps(1.0, 2.5, 3.6, 4.8);
    result = chebyshev(3, x);
    p = (float *)&result;
    std::array<float, 4> correct_result({1.0, 55.0, 175.824, 427.968});
    for (int j = 0; j < 4; j++)
        EXPECT_THAT(p[j], testing::FloatNear(correct_result[j], 1e-4));
}

#ifdef __AVX2__
TEST(chebyshev, avx2) {
    std::array<double, 4> x_vec({1, 2, 3, 0});
    __m256d x = _mm256_loadu_pd(x_vec.data());
    __m256d result = chebyshev(1, x);
    double *p = (double *)&result;
    std::array<double, 4> check;
    for (int i = 0; i < 4; i++) {
        check[i] = p[i];
    }
    ASSERT_THAT(check, testing::ElementsAre(1.0, 2.0, 3.0, 0.0));

    result = chebyshev(0, x);
    p = (double *)&result;
    for (int i = 0; i < 4; i++) {
        check[i] = p[i];
    }
    ASSERT_THAT(check, testing::ElementsAre(1.0, 1.0, 1.0, 1.0));

    x = _mm256_setr_pd(1.0, 2.5, 3.6, 4.8);
    result = chebyshev(3, x);
    p = (double *)&result;
    std::array<double, 4> correct_result({1.0, 55.0, 175.824, 427.968});
    for (int j = 0; j < 4; j++)
        EXPECT_THAT(p[j], testing::DoubleNear(correct_result[j], 1e-12));
}
#endif

#ifdef __AVX2__
TEST(chebyshev_next, avx2) {
    __m256d x = _mm256_setr_pd(1.0, 2.5, 3.6, 4.8);
    __m256d c0 = _mm256_set1_pd(1.0);
    __m256d c1 = x;
    __m256d c2 = chebyshev_next(x, c0, x);
    __m256d c3 = chebyshev_next(c2, x, x);
    __m256d c4 = chebyshev_next(c3, c2, x);
    __m256d c5 = chebyshev_next(c4, c3, x);
    __m256d c6 = chebyshev_next(c5, c4, x);

    double *p = (double *)&c2;
    std::array<double, 4> correct_result2(
        {1.0000000000000000000, 11.500000000000000000, 24.920000000000001705,
         45.079999999999998295});
    for (int i = 0; i < 4; i++) {
        EXPECT_THAT(p[i], testing::DoubleNear(correct_result2[i], 1e-11));
    }

    p = (double *)&c3;
    std::array<double, 4> correct_result3(
        {1.0000000000000000000, 55.000000000000000000, 175.82400000000001228,
         427.96799999999996089});
    for (int i = 0; i < 4; i++) {
        EXPECT_THAT(p[i], testing::DoubleNear(correct_result3[i], 1e-11));
    }

    p = (double *)&c4;
    std::array<double, 4> correct_result4(
        {1.0000000000000000000, 263.50000000000000000, 1241.0128000000001975,
         4063.4127999999996064});
    for (int i = 0; i < 4; i++) {
        EXPECT_THAT(p[i], testing::DoubleNear(correct_result4[i], 1e-11));
    }

    p = (double *)&c5;
    std::array<double, 4> correct_result5(
        {1.0000000000000000000, 1262.5000000000000000, 8759.4681600000003527,
         38580.794879999994009});
    for (int i = 0; i < 4; i++) {
        EXPECT_THAT(p[i], testing::DoubleNear(correct_result5[i], 1e-11));
    }
}
#endif

#ifdef __AVX2__
TEST(chebyshev_next, avx2_test2) {
    __m256d x = _mm256_setr_pd(-1.0, -0.5, 0.5, 1.0);
    __m256d c0 = _mm256_set1_pd(1.0);
    __m256d c1 = x;
    __m256d c2 = chebyshev_next(x, c0, x);
    __m256d c3 = chebyshev_next(c2, x, x);
    __m256d c4 = chebyshev_next(c3, c2, x);
    __m256d c5 = chebyshev_next(c4, c3, x);
    __m256d c6 = chebyshev_next(c5, c4, x);

    double *p = (double *)&c2;
    std::array<double, 4> correct_result2({1.0, -0.5, -0.5, 1.0});
    for (int i = 0; i < 4; i++) {
        EXPECT_THAT(p[i], testing::DoubleNear(correct_result2[i], 1e-14));
    }

    p = (double *)&c3;
    std::array<double, 4> correct_result3({-1.0, 1.0, -1.0, 1.0});
    for (int i = 0; i < 4; i++) {
        EXPECT_THAT(p[i], testing::DoubleNear(correct_result3[i], 1e-14));
    }

    p = (double *)&c4;
    std::array<double, 4> correct_result4({1.0, -0.5, -0.5, 1.0});
    for (int i = 0; i < 4; i++) {
        EXPECT_THAT(p[i], testing::DoubleNear(correct_result4[i], 1e-14));
    }

    p = (double *)&c5;
    std::array<double, 4> correct_result5({-1.0, -0.5, 0.5, 1.0});
    for (int i = 0; i < 4; i++) {
        EXPECT_THAT(p[i], testing::DoubleNear(correct_result5[i], 1e-14));
    }
}
#endif

#ifdef __AVX2__
TEST(chebyshev_next, avx2_test3) {
    __m256d x = _mm256_setr_pd(-0.770000000000, -0.3300000000000,
                               0.370000000000, 0.89000000000);
    __m256d c0 = _mm256_set1_pd(1.0);
    __m256d c1 = x;
    __m256d c2 = chebyshev_next(x, c0, x);
    __m256d c3 = chebyshev_next(c2, x, x);
    __m256d c4 = chebyshev_next(c3, c2, x);
    __m256d c5 = chebyshev_next(c4, c3, x);
    __m256d c6 = chebyshev_next(c5, c4, x);

    double *p = (double *)&c2;
    std::array<double, 4> correct_result2(
        {0.18579999999999996518, -0.78220000000000000639,
         -0.72619999999999995666, 0.58420000000000005258});
    for (int i = 0; i < 4; i++) {
        EXPECT_TRUE(std::fabs(p[i] - correct_result2[i]) < 1e-13);
    }

    p = (double *)&c3;
    std::array<double, 4> correct_result3(
        {0.48386800000000007582, 0.84625200000000000422,
         -0.90738799999999997237, 0.14987600000000000922});
    for (int i = 0; i < 4; i++) {
        EXPECT_TRUE(std::fabs(p[i] - correct_result3[i]) < 1e-13);
    }

    p = (double *)&c4;
    std::array<double, 4> correct_result4(
        {-0.93095672000000007085, 0.22367368000000009687,
         0.054732879999999983767, -0.31742071999999987852});
    for (int i = 0; i < 4; i++) {

        EXPECT_TRUE(std::fabs(p[i] - correct_result4[i]) < 1e-13);
    }

    p = (double *)&c5;
    std::array<double, 4> correct_result5(
        {0.94980534880000000442, -0.99387662880000005039,
         0.94789033119999988042, -0.71488488159999996618});
    for (int i = 0; i < 4; i++) {
        EXPECT_TRUE(std::fabs(p[i] - correct_result5[i]) < 1e-13);
    }
}
#endif

TEST(cos, sse) {
    std::array<float, 4> x_vec({-M_PI, 0, M_PI / 2, M_PI});
    __m128 x = _mm_loadu_ps(x_vec.data());
    __m128 result = sse_cos(x);
    float *p = (float *)&result;
    std::array<float, 4> correct_result({-1.0, 1.0, 0.0, -1.0});

    for (int j = 0; j < 4; j++)
        EXPECT_THAT(p[j], testing::FloatNear(correct_result[j], 5e-6));
}

#ifdef __AVX2__
TEST(cos, avx2) {
    std::array<double, 4> x_vec({-M_PI, 0.0, M_PI / 2.0, M_PI});
    __m256d x = _mm256_loadu_pd(x_vec.data());
    __m256d result = avx_cos(x);
    double *p = (double *)&result;
    std::array<double, 4> correct_result({-1.0, 1.0, 0.0, -1.0});

    for (int j = 0; j < 4; j++)
        EXPECT_THAT(p[j], testing::DoubleNear(correct_result[j], 1e-7));
}
#endif

TEST(tan, sse) {
    std::array<float, 4> x_vec({-0.7, -0.3, 0.33, 1.553});
    __m128 x = _mm_loadu_ps(x_vec.data());
    __m128 result = sse_tan(x);
    float *p = (float *)&result;
    std::array<float, 4> correct_result(
        {-0.84228838046307941134, -0.30933624960962324835,
         0.34252486753003896780, 56.185438688872594071});
    for (int j = 0; j < 4; j++) {
        double tol = 1e-7;
        if (j == 3)
            tol = 1e-4;
        EXPECT_THAT(p[j], testing::FloatNear(correct_result[j], tol));
    }
}

#ifdef __AVX2__
TEST(tan, avx2) {
    std::array<double, 4> x_vec({-0.7, -0.3, 0.33, 1.553});
    __m256d x = _mm256_loadu_pd(x_vec.data());
    __m256d result = avx_tan(x);
    double *p = (double *)&result;
    std::array<double, 4> correct_result(
        {-0.84228838046307941134, -0.30933624960962324835,
         0.34252486753003896780, 56.185438688872594071});
    for (int j = 0; j < 4; j++) {
        double tol = 1e-11;
        if (j == 3)
            tol = 1e-9;
        EXPECT_THAT(p[j], testing::DoubleNear(correct_result[j], tol));
    }
}
#endif

TEST(arctan, sse) {
    std::array<float, 4> x_vec({-50, -0.4, 0.37, 0.97});
    __m128 x = _mm_loadu_ps(x_vec.data());
    __m128 result = sse_arctan(x);
    float *p = (float *)&result;
    std::array<float, 4> correct_result(
        {-1.55079899282, -0.380506377112, 0.354379919123, 0.770170914020});
    for (int j = 0; j < 4; j++)
        EXPECT_THAT(p[j], testing::FloatNear(correct_result[j], 1e-7));

    x_vec = std::array<float, 4>({0.0, -0.1, 0.3333, 0.9999});
    x = _mm_loadu_ps(x_vec.data());
    result = sse_arctan(x);
    p = (float *)&result;
    correct_result = std::array<float, 4>(
        {0.0, -0.0996686524912, 0.321720554097, 0.785348160897});
    for (int j = 0; j < 4; j++)
        EXPECT_THAT(p[j], testing::FloatNear(correct_result[j], 2e-7));
}

TEST(arctan, avx2) {
    std::array<double, 4> x_vec({-50, -0.4, 0.37, 0.97});
    __m256d x = _mm256_loadu_pd(x_vec.data());
    __m256d result = avx_arctan(x);
    double *p = (double *)&result;
    std::array<double, 4> correct_result(
        {-1.5507989928217460862, -0.38050637711236490190,
         0.35437991912343780321, 0.77017091402033099889});
    for (int j = 0; j < 4; j++)
        EXPECT_THAT(p[j], testing::DoubleNear(correct_result[j], 1e-13));

    x_vec = std::array<double, 4>({-100.0, -0.1, 0.3333, 0.9999});
    x = _mm256_loadu_pd(x_vec.data());
    result = avx_arctan(x);
    p = (double *)&result;
    correct_result =
        std::array<double, 4>({-1.5607966601082313810, -0.099668652491162038065,
                               0.32172055409664818137, 0.78534816089736503297});
    for (int j = 0; j < 4; j++)
        EXPECT_THAT(p[j], testing::DoubleNear(correct_result[j], 1e-13));
}
