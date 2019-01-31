TEST(sort, bitonic_avx_reg_sort) {
    __m256d a = _mm256_set_pd(4, 3, 2, 1);
    __m256d sorted = bitonic_avx_reg_sort(a);

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
