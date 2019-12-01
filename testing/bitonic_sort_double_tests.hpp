TEST(SORT, BITONIC_AVX_SORT_REG_DOUBLE) {
    {
        __m256d reg = _mm256_setr_pd(0, 1, 2, 3);
        BITONIC_SORT::bitonic_sort(reg);
        double *sol = (double *)&reg;
        double solution[4] = {0, 1, 2, 3};
        for (int i = 0; i < 4; i++)
            ASSERT_EQ(*(sol + i), solution[i]);
    }
    {
        __m256d reg2 = _mm256_setr_pd(-10, 3, 6, -12);
        BITONIC_SORT::bitonic_sort(reg2);
        double *sol2 = (double *)&reg2;
        double solution2[4] = {-12, -10, 3, 6};
        for (int i = 0; i < 4; i++)
            ASSERT_EQ(*(sol2 + i), solution2[i]);
    }
    {
        __m256d reg3 = _mm256_setr_pd(5, 3, 6, 48);
        BITONIC_SORT::bitonic_sort(reg3);
        double *sol3 = (double *)&reg3;
        double solution3[4] = {3, 5, 6, 48};
        for (int i = 0; i < 4; i++)
            ASSERT_EQ(*(sol3 + i), solution3[i]);
    }
    {
        std::vector<double> inp0({random_double(), random_double(),
                                  random_double(),
                                  random_double()});

        __m256d reg0 = _mm256_loadu_pd(inp0.data());
        std::sort(std::begin(inp0), std::end(inp0));

        BITONIC_SORT::bitonic_sort(reg0);
        double *s1 = (double *)&reg0;
        // std::cout << "inp: " << std::endl;
        for (int i = 0; i < 4; i++) {
            ASSERT_EQ(*(s1 + i), inp0[i]);
        }
    }
}

TEST(SORT, BITONIC_AVX_SORT_REG2_DOUBLE) {
    {
        __m256d reg0 = _mm256_setr_pd(1, 2, 3, 4);
        __m256d reg1 = _mm256_setr_pd(5, 6, 7, 8);
        BITONIC_SORT::bitonic_sort(reg0, reg1);
        double *s1 = (double *)&reg0;
        double *s2 = (double *)&reg1;
        double sol1[4] = {1, 2, 3, 4};
        double sol2[4] = {5, 6, 7, 8};
        for (int i = 0; i < 4; i++) {
            ASSERT_EQ(*(s1 + i), sol1[i]);
            ASSERT_EQ(*(s2 + i), sol2[i]);
        }
    }
    {
        __m256d reg0 = _mm256_setr_pd(-1, -2, 18, 29);
        __m256d reg1 = _mm256_setr_pd(500, -80, 2, -5);
        BITONIC_SORT::bitonic_sort(reg0, reg1);
        double *s1 = (double *)&reg0;
        double *s2 = (double *)&reg1;
        double sol1[4] = {-80, -5, -2, -1};
        double sol2[4] = {2, 18, 29, 500};
        for (int i = 0; i < 4; i++) {
            ASSERT_EQ(*(s1 + i), sol1[i]);
            ASSERT_EQ(*(s2 + i), sol2[i]);
        }
    }
    {
        std::vector<double> inp0({random_double(), random_double(),
                                  random_double(),
                                  random_double()});
        std::vector<double> inp1({random_double(), random_double(),
                                  random_double(),
                                  random_double()});

        __m256d reg0 = _mm256_loadu_pd(inp0.data());
        __m256d reg1 = _mm256_loadu_pd(inp1.data());

        inp0.insert(inp0.end(), inp1.begin(), inp1.end());
        std::sort(std::begin(inp0), std::end(inp0));

        BITONIC_SORT::bitonic_sort(reg0, reg1);
        double *s1 = (double *)&reg0;
        double *s2 = (double *)&reg1;
        // std::cout << "inp: " << std::endl;
        for (int i = 0; i < 4; i++) {
            ASSERT_EQ(*(s1 + i), inp0[i]);
            ASSERT_EQ(*(s2 + i), inp0[i + 4]);
        }
    }
}

TEST(SORT, BITONIC_AVX_SORT_4REG_DOUBLE) {
    {
        __m256d reg0 = _mm256_setr_pd(1, 2, 3, 4);
        __m256d reg1 = _mm256_setr_pd(5, 6, 7, 8);
        __m256d reg2 = _mm256_setr_pd(9, 10, 11, 12);
        __m256d reg3 = _mm256_setr_pd(13, 14, 15, 16);

        BITONIC_SORT::bitonic_sort(reg0, reg1, reg2, reg3);

        double *s1 = (double *)&reg0;
        double *s2 = (double *)&reg1;
        double *s3 = (double *)&reg2;
        double *s4 = (double *)&reg3;
        double sol1[4] = {1, 2, 3, 4};
        double sol2[4] = {5, 6, 7, 8};
        double sol3[4] = {9, 10, 11, 12};
        double sol4[4] = {13, 14, 15, 16};
        for (int i = 0; i < 4; i++) {
            ASSERT_EQ(*(s1 + i), sol1[i]);
            ASSERT_EQ(*(s2 + i), sol2[i]);
            ASSERT_EQ(*(s3 + i), sol3[i]);
            ASSERT_EQ(*(s4 + i), sol4[i]);
        }
    }
    {
        __m256d reg0 = _mm256_setr_pd(-1, -2, 18, 29);
        __m256d reg1 = _mm256_setr_pd(500, -80, 2, -5);
        __m256d reg2 = _mm256_setr_pd(-10, 22, 180, -2900);
        __m256d reg3 = _mm256_setr_pd(38, -120, 25, -17);
        BITONIC_SORT::bitonic_sort(reg0, reg1, reg2, reg3);

        double *s1 = (double *)&reg0;
        double *s2 = (double *)&reg1;
        double *s3 = (double *)&reg2;
        double *s4 = (double *)&reg3;
        std::vector<double> sol({-80, -5, -2, -1, -10, 2, 18, 29,
                                 500, 22, 180, -2900, 38, -120, 25,
                                 -17});
        std::sort(sol.begin(), sol.end());
        for (int i = 0; i < 4; i++) {
            ASSERT_EQ(*(s1 + i), sol[i]);
            ASSERT_EQ(*(s2 + i), sol[i + 4]);
            ASSERT_EQ(*(s3 + i), sol[i + 8]);
            ASSERT_EQ(*(s4 + i), sol[i + 12]);
        }
    }

    {
        std::vector<double> inp0({random_double(), random_double(),
                                  random_double(),
                                  random_double()});
        std::vector<double> inp1({random_double(), random_double(),
                                  random_double(),
                                  random_double()});
        std::vector<double> inp2({random_double(), random_double(),
                                  random_double(),
                                  random_double()});
        std::vector<double> inp3({random_double(), random_double(),
                                  random_double(),
                                  random_double()});

        __m256d reg0 = _mm256_loadu_pd(inp0.data());
        __m256d reg1 = _mm256_loadu_pd(inp1.data());
        __m256d reg2 = _mm256_loadu_pd(inp2.data());
        __m256d reg3 = _mm256_loadu_pd(inp3.data());

        inp0.insert(inp0.end(), inp1.begin(), inp1.end());
        inp0.insert(inp0.end(), inp2.begin(), inp2.end());
        inp0.insert(inp0.end(), inp3.begin(), inp3.end());
        std::sort(std::begin(inp0), std::end(inp0));

        BITONIC_SORT::bitonic_sort(reg0, reg1, reg2, reg3);
        double *s1 = (double *)&reg0;
        double *s2 = (double *)&reg1;
        double *s3 = (double *)&reg2;
        double *s4 = (double *)&reg3;
        // std::cout << "inp: " << std::endl;
        for (int i = 0; i < 4; i++) {
            ASSERT_EQ(*(s1 + i), inp0[i]);
            ASSERT_EQ(*(s2 + i), inp0[i + 4]);
            ASSERT_EQ(*(s3 + i), inp0[i + 8]);
            ASSERT_EQ(*(s4 + i), inp0[i + 12]);
        }
    }
}



TEST(SORT, TEST_2N_SORT_DOUBLE_VER) {

    {
        aligned_vector<double> inp0({random_float(), random_float(),
                                     random_float(),
                                     random_float()});
        aligned_vector<double> inp1 = inp0;

        BITONIC_SORT::sort_2n_vector(inp0.data(), 4);
        std::sort(std::begin(inp1), std::end(inp1));

        for (int i = 0; i < 4; i++) {
            ASSERT_EQ(inp1[i], inp0[i]);
        }
    }

    {
        aligned_vector<double> inp0;
        aligned_vector<double> inp1;
        inp0.reserve(8);
        for (int i = 0; i < 8; i++)
            inp0.push_back(random_float());

        inp1 = inp0;
        BITONIC_SORT::sort_2n_vector(inp1.data(), 8);

        std::sort(std::begin(inp0), std::end(inp0));

        for (int i = 0; i < 8; i++) {
            ASSERT_EQ(inp1[i], inp0[i]);
        }
    }
}

TEST(SORT, TEST_2N_SORT_DOUBLE_VER_TEST2) {

    for (int k = 0; k < 5; k++) {
        for (int j = 0; j < 5; j++) {
            unsigned size = 8192;
            size *= (1 << k); //  1 << 2 = (int) Math.pow(2, 2)
            unsigned move_start = 1 << k;
            unsigned move_end = 1 << k;

            aligned_vector<double> inp0;
            aligned_vector<double> inp1;
            size = size + move_start + move_end;

            inp0.reserve(size);

            for (unsigned i = 0; i < size; i++)
                inp0.push_back(random_float());

            inp1 = inp0;

            BITONIC_SORT::sort_2n_vector(inp1.data() + move_start,
                                         size - move_end -
                                             move_start);

            std::sort(std::next(std::begin(inp0), move_start),
                      std::prev(std::end(inp0), move_end));

            for (unsigned i = move_start; i < size - move_end;
                 i++) {
                ASSERT_EQ(inp1[i], inp0[i]);
            }
        }
    }
}



TEST(SORT, TEST_SORT_DOUBLE_4n_VECTOR) {

    for (unsigned size = 4; size <= 2000; size += 4) {
        aligned_vector<double> inp0;
        aligned_vector<double> inp1;
        inp0.reserve(size);
        for (unsigned i = 0; i < size; i++)
            inp0.push_back(random_float());

        inp1 = inp0;
        BITONIC_SORT::sort_4n_vector(inp1.data(), size);

        // std::cout << "end of sort_vector" << std::endl;
        std::sort(std::begin(inp0), std::end(inp0));

        /* for (int i = 0; i < size; i++) {
           std::cout << inp1[i] << std::endl;
           if (mod8(i + 1) == 0)
           std::cout << "\n";
           }*/

        for (int i = 0; i < size; i++) {
            ASSERT_EQ(inp1[i], inp0[i]);
        }
    }

    for (int i = 0; i < 2; i++) {

        unsigned move_start = (random_pos_int() % 4) * 4;
        unsigned move_end = (random_pos_int() % 4) * 4;
        unsigned size =
            move_start + move_end + 4 * random_pos_int();

        aligned_vector<double> inp0;
        aligned_vector<double> inp1;

        inp0.reserve(size);

        for (unsigned i = 0; i < size; i++) {
            inp0.push_back(random_double());
            // std::cout << i << ": " << inp0[i] << std::endl;
        }

        inp1 = inp0;

        auto p1 = std::next(inp0.begin(), move_start);
        auto p2 = std::prev(inp0.end(), move_end);

        BITONIC_SORT::sort_4n_vector(inp1.data() + move_start,
                                     size - move_end - move_start);
        std::sort(p1, p2);

        for (unsigned i = move_start; i < size - move_end; i++) {
            // std::cout << i << ": " << inp1[i] << "  " << inp0[i]
            //          << std::endl;
            ASSERT_EQ(inp1[i], inp0[i]);
        }
    }
}

TEST(SORT, TEST_SORT_DOUBLE_VECTOR_ALL_CASES) {

    for (unsigned size = 1; size < 1000; size++) {

        aligned_vector<double> inp0;
        aligned_vector<double> inp1;
        inp0.reserve(size);
        for (unsigned i = 0; i < size; i++)
            inp0.push_back(random_double());

        inp1 = inp0;
        BITONIC_SORT::sort_vector(inp1.data(), size);

        std::sort(std::begin(inp0), std::end(inp0));

        /* for (int i = 0; i < size; i++) {
           std::cout << inp1[i] << std::endl;
           if (mod8(i + 1) == 0)
           std::cout << "\n";
           }*/

        for (int i = 0; i < size; i++) {
            ASSERT_EQ(inp1[i], inp0[i]);
        }
    }

    for (unsigned size = 1000; size < 5000; size++) {

        aligned_vector<double> inp0;
        aligned_vector<double> inp1;
        inp0.reserve(size);
        for (unsigned i = 0; i < size; i++)
            inp0.push_back(random_double());

        inp1 = inp0;

        unsigned move_start = random_pos_int() % 500;
        unsigned move_end = random_pos_int() % 500;

        BITONIC_SORT::sort_vector(inp1.data() + move_start,
                                  size - move_start - move_end);

        std::sort(std::next(std::begin(inp0), move_start),
                  std::prev(std::end(inp0), move_end));

        for (int i = move_start; i < size - move_end; i++) {
            ASSERT_EQ(inp1[i], inp0[i]);
        }
    }
}
