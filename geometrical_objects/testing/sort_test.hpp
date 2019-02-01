#ifdef __AVX2__

TEST(sort, bitonic_avx_reg_sort_double) {
    __m256d sorted = _mm256_set_pd(4, 3, 2, 1);
    bitonic_avx_reg_sort(sorted);

    __m256d test_print = _mm256_set_pd(256, 192, 128, 64);
    double arr[4];
    print_avx(test_print, "test: ");
    _mm256_storeu_pd(arr, test_print);
    std::cout << arr[0] << " " << arr[1] << " " << arr[2] << " " << arr[3]
              << std::endl;
    double *arr2 = (double *)&sorted;
    std::cout << arr2[0] << " " << arr2[1] << " " << arr2[2] << " " << arr2[3]
              << std::endl;
}

TEST(sort, bitonic_avx_reg_sort_float) {
    __m256 reg = _mm256_setr_ps(0, 1, 2, 3, 4, 5, 6, 7);
    bitonic_avx_reg_sort(reg);
    float *sol = (float *)&reg;
    float solution[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    for (int i = 0; i < 8; i++)
        ASSERT_EQ(*(sol + i), solution[i]);

    __m256 reg2 = _mm256_setr_ps(1, 5, 8, 3, 9, 8, 10, -2);
    bitonic_avx_reg_sort(reg2);
    float *sol2 = (float *)&reg2;
    float solution2[8] = {-2, 1, 3, 5, 8, 8, 9, 10};
    for (int i = 0; i < 8; i++)
        ASSERT_EQ(*(sol2 + i), solution2[i]);
}
#endif
