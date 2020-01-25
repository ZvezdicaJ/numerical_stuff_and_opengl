// test single register sort
TEST(SORT, BITONIC_AVX_REG_SORT_FLOAT) {

    {
        __m256 reg = _mm256_setr_ps(0, 1, 2, 3, 4, 5, 6, 7);
        BITONIC_SORT::bitonic_sort(reg);
        float *sol = (float *)&reg;
        float solution[8] = {0, 1, 2, 3, 4, 5, 6, 7};
        for (int i = 0; i < 8; i++)
            ASSERT_EQ(*(sol + i), solution[i]);
    }
    {
        __m256 reg2 = _mm256_setr_ps(1, 5, 8, 3, 9, 8, 10, -2);
        BITONIC_SORT::bitonic_sort(reg2);
        // print_avx(reg2, "reg2: ");
        float *sol2 = (float *)&reg2;
        float solution2[8] = {-2, 1, 3, 5, 8, 8, 9, 10};
        for (int i = 0; i < 8; i++)
            ASSERT_EQ(*(sol2 + i), solution2[i]);
    }
    {
        __m256 reg3 = _mm256_setr_ps(100, 5, -8, 3, -9, 15, 0, -2);
        BITONIC_SORT::bitonic_sort(reg3);
        // print_avx(reg2, "reg2: ");
        float *sol3 = (float *)&reg3;
        float solution3[8] = {-9, -8, -2, 0, 3, 5, 15, 100};
        for (int i = 0; i < 8; i++)
            ASSERT_EQ(*(sol3 + i), solution3[i]);
    }
    {
        std::vector<float> inp0({random_float(), random_float(),
                                 random_float(), random_float(),
                                 random_float(), random_float(),
                                 random_float(), random_float()});

        __m256 reg0 = _mm256_loadu_ps(inp0.data());

        std::sort(std::begin(inp0), std::end(inp0));

        BITONIC_SORT::bitonic_sort(reg0);
        float *s1 = (float *)&reg0;
        // std::cout << "inp: " << std::endl;
        for (int i = 0; i < 8; i++) {
            ASSERT_EQ(*(s1 + i), inp0[i]);
        }
    }
    // exit(0);
}

// test sorting two already sorted registers
TEST(SORT, BITONIC_AVX_MERGE_FLOAT_TWO_VECTORS) {
    {
        __m256 reg0 = _mm256_setr_ps(1, 2, 3, 4, 5, 6, 7, 8);
        __m256 reg1 = _mm256_setr_ps(9, 10, 11, 12, 13, 14, 15, 16);
        BITONIC_SORT::bitonic_sort(reg0, reg1);
        float *s1 = (float *)&reg0;
        float *s2 = (float *)&reg1;
        float sol1[8] = {1, 2, 3, 4, 5, 6, 7, 8};
        float sol2[8] = {9, 10, 11, 12, 13, 14, 15, 16};
        for (int i = 0; i < 8; i++) {
            ASSERT_EQ(*(s1 + i), sol1[i]);
            ASSERT_EQ(*(s2 + i), sol2[i]);
        }
    }
    {
        __m256 reg0 =
            _mm256_setr_ps(-1, -2, 18, 29, 48, 52, -50, -22);
        __m256 reg1 = _mm256_setr_ps(500, -80, 2, -5, 100, 0, 1, 5);
        BITONIC_SORT::bitonic_sort(reg0, reg1);
        float *s1 = (float *)&reg0;
        float *s2 = (float *)&reg1;
        float sol1[8] = {-80, -50, -22, -5, -2, -1, 0, 1};
        float sol2[8] = {2, 5, 18, 29, 48, 52, 100, 500};
        for (int i = 0; i < 8; i++) {
            ASSERT_EQ(*(s1 + i), sol1[i]);
            ASSERT_EQ(*(s2 + i), sol2[i]);
        }
    }
    for (int k = 0; k < 10; k++) {
        std::vector<float> inp0({random_float(), random_float(),
                                 random_float(), random_float(),
                                 random_float(), random_float(),
                                 random_float(), random_float()});
        std::vector<float> inp1({random_float(), random_float(),
                                 random_float(), random_float(),
                                 random_float(), random_float(),
                                 random_float(), random_float()});

        std::sort(std::begin(inp0), std::end(inp0));
        std::sort(std::begin(inp1), std::end(inp1));

        __m256 reg0 = _mm256_loadu_ps(inp0.data());
        __m256 reg1 = _mm256_loadu_ps(inp1.data());

        inp0.insert(inp0.end(), inp1.begin(), inp1.end());
        std::sort(std::begin(inp0), std::end(inp0));

        BITONIC_SORT::bitonic_sort(reg0, reg1);
        float *s1 = (float *)&reg0;
        float *s2 = (float *)&reg1;
        // std::cout << "inp: " << std::endl;
        for (int i = 0; i < 8; i++) {
            ASSERT_EQ(*(s1 + i), inp0[i]);
            ASSERT_EQ(*(s2 + i), inp0[i + 8]);
        }
    }
}

// test sorting four already sorted float registers
TEST(SORT, BITONIC_AVX_SORT_4REG_FLOAT) {
    {
        __m256 reg0 = _mm256_setr_ps(1, 2, 3, 4, 5, 6, 7, 8);
        __m256 reg1 = _mm256_setr_ps(9, 10, 11, 12, 13, 14, 15, 16);
        __m256 reg2 =
            _mm256_setr_ps(17, 18, 19, 20, 21, 22, 23, 24);
        __m256 reg3 =
            _mm256_setr_ps(25, 26, 27, 28, 29, 30, 31, 32);

        BITONIC_SORT::bitonic_sort(reg0, reg1, reg2, reg3);

        float *s1 = (float *)&reg0;
        float *s2 = (float *)&reg1;
        float *s3 = (float *)&reg2;
        float *s4 = (float *)&reg3;
        float sol1[8] = {1, 2, 3, 4, 5, 6, 7, 8};
        float sol2[8] = {9, 10, 11, 12, 13, 14, 15, 16};
        float sol3[8] = {17, 18, 19, 20, 21, 22, 23, 24};
        float sol4[8] = {25, 26, 27, 28, 29, 30, 31, 32};
        for (int i = 0; i < 8; i++) {
            ASSERT_EQ(*(s1 + i), sol1[i]);
            ASSERT_EQ(*(s2 + i), sol2[i]);
            ASSERT_EQ(*(s3 + i), sol3[i]);
            ASSERT_EQ(*(s4 + i), sol4[i]);
        }
    }
    {
        __m256 reg0 =
            _mm256_setr_ps(-1, -2, 18, 29, 50, 38, -220, -238);
        __m256 reg1 =
            _mm256_setr_ps(500, -80, 2, -5, 12, 100, 29, 31);
        __m256 reg2 =
            _mm256_setr_ps(-10, 22, 180, -2900, 35, 70, 79, -80);
        __m256 reg3 =
            _mm256_setr_ps(38, -120, 25, -17, 12, 15, -220, -450);
        BITONIC_SORT::bitonic_sort(reg0, reg1, reg2, reg3);

        float *s1 = (float *)&reg0;
        float *s2 = (float *)&reg1;
        float *s3 = (float *)&reg2;
        float *s4 = (float *)&reg3;
        std::vector<float> sol(
            {-1,  -2,   18,  29,    50, 38,  -220, -238,
             500, -80,  2,   -5,    12, 100, 29,   31,
             -10, 22,   180, -2900, 35, 70,  79,   -80,
             38,  -120, 25,  -17,   12, 15,  -220, -450});
        std::sort(sol.begin(), sol.end());
        for (int i = 0; i < 8; i++) {
            ASSERT_EQ(*(s1 + i), sol[i]);
            ASSERT_EQ(*(s2 + i), sol[i + 8]);
            ASSERT_EQ(*(s3 + i), sol[i + 16]);
            ASSERT_EQ(*(s4 + i), sol[i + 24]);
        }
    }

    {
        std::vector<float> inp0({random_float(), random_float(),
                                 random_float(), random_float(),
                                 random_float(), random_float(),
                                 random_float(), random_float()});
        std::vector<float> inp1({random_float(), random_float(),
                                 random_float(), random_float(),
                                 random_float(), random_float(),
                                 random_float(), random_float()});
        std::vector<float> inp2({random_float(), random_float(),
                                 random_float(), random_float(),
                                 random_float(), random_float(),
                                 random_float(), random_float()});
        std::vector<float> inp3({random_float(), random_float(),
                                 random_float(), random_float(),
                                 random_float(), random_float(),
                                 random_float(), random_float()});

        __m256 reg0 = _mm256_loadu_ps(inp0.data());
        __m256 reg1 = _mm256_loadu_ps(inp1.data());
        __m256 reg2 = _mm256_loadu_ps(inp2.data());
        __m256 reg3 = _mm256_loadu_ps(inp3.data());

        inp0.insert(inp0.end(), inp1.begin(), inp1.end());
        inp0.insert(inp0.end(), inp2.begin(), inp2.end());
        inp0.insert(inp0.end(), inp3.begin(), inp3.end());
        std::sort(std::begin(inp0), std::end(inp0));

        BITONIC_SORT::bitonic_sort(reg0, reg1, reg2, reg3);
        float *s1 = (float *)&reg0;
        float *s2 = (float *)&reg1;
        float *s3 = (float *)&reg2;
        float *s4 = (float *)&reg3;
        for (int i = 0; i < 8; i++) {
            ASSERT_EQ(*(s1 + i), inp0[i]);
            ASSERT_EQ(*(s2 + i), inp0[i + 8]);
            ASSERT_EQ(*(s3 + i), inp0[i + 16]);
            ASSERT_EQ(*(s4 + i), inp0[i + 24]);
        }
    }
}

// merge 4 presorted registers - all numbers are sorted afterwards
TEST(SORT, BITONIC_AVX_MERGE_4REG_FLOAT) {
    {
        __m256 reg0 = _mm256_setr_ps(1, 2, 3, 4, 5, 6, 7, 8);
        __m256 reg1 = _mm256_setr_ps(9, 10, 11, 12, 13, 14, 15, 16);
        __m256 reg2 =
            _mm256_setr_ps(17, 18, 19, 20, 21, 22, 23, 24);
        __m256 reg3 =
            _mm256_setr_ps(25, 26, 27, 28, 29, 30, 31, 32);

        BITONIC_SORT::bitonic_merge(reg0, reg1, reg2, reg3);

        float *s1 = (float *)&reg0;
        float *s2 = (float *)&reg1;
        float *s3 = (float *)&reg2;
        float *s4 = (float *)&reg3;
        float sol1[8] = {1, 2, 3, 4, 5, 6, 7, 8};
        float sol2[8] = {9, 10, 11, 12, 13, 14, 15, 16};
        float sol3[8] = {17, 18, 19, 20, 21, 22, 23, 24};
        float sol4[8] = {25, 26, 27, 28, 29, 30, 31, 32};
        for (int i = 0; i < 8; i++) {
            ASSERT_EQ(*(s1 + i), sol1[i]);
            ASSERT_EQ(*(s2 + i), sol2[i]);
            ASSERT_EQ(*(s3 + i), sol3[i]);
            ASSERT_EQ(*(s4 + i), sol4[i]);
        }
    }
    for (int k = 0; k < 10; k++) {
        std::vector<float> inp0(
            {random_float(), random_float(), random_float(),
             random_float(), random_float(), random_float(),
             random_float(), random_float(), random_float(),
             random_float(), random_float(), random_float(),
             random_float(), random_float(), random_float(),
             random_float()});
        std::vector<float> inp1(
            {random_float(), random_float(), random_float(),
             random_float(), random_float(), random_float(),
             random_float(), random_float(), random_float(),
             random_float(), random_float(), random_float(),
             random_float(), random_float(), random_float(),
             random_float()});

        std::sort(std::begin(inp0), std::end(inp0));
        std::sort(std::begin(inp1), std::end(inp1));

        __m256 reg0 = _mm256_loadu_ps(inp0.data());
        __m256 reg1 = _mm256_loadu_ps(inp0.data() + 8);
        __m256 reg2 = _mm256_loadu_ps(inp1.data());
        __m256 reg3 = _mm256_loadu_ps(inp1.data() + 8);

        inp0.insert(inp0.end(), inp1.begin(), inp1.end());
        std::sort(std::begin(inp0), std::end(inp0));

        BITONIC_SORT::bitonic_merge(reg0, reg1, reg2, reg3);
        float *s1 = (float *)&reg0;
        float *s2 = (float *)&reg1;
        float *s3 = (float *)&reg2;
        float *s4 = (float *)&reg3;
        for (int i = 0; i < 8; i++) {
            ASSERT_EQ(*(s1 + i), inp0[i]);
            ASSERT_EQ(*(s2 + i), inp0[i + 8]);
            ASSERT_EQ(*(s3 + i), inp0[i + 16]);
            ASSERT_EQ(*(s4 + i), inp0[i + 24]);
        }
    }
}

TEST(SORT, TEST_2N_SORT_FLOAT_VER) {

    {
        aligned_vector<float> inp0(
            {random_float(), random_float(), random_float(),
             random_float(), random_float(), random_float(),
             random_float(), random_float()});
        aligned_vector<float> inp1 = inp0;

        BITONIC_SORT::sort_2n_vector(inp0.data(), 8);
        std::sort(std::begin(inp1), std::end(inp1));

        for (int i = 0; i < 7; i++) {
            ASSERT_EQ(inp1[i], inp0[i]);
        }
    }

    {
        aligned_vector<float> inp0;
        aligned_vector<float> inp1;
        unsigned size = 16 * 16;
        inp0.reserve(size);
        for (int i = 0; i < size; i++)
            inp0.push_back(random_float());

        inp1 = inp0;
        BITONIC_SORT::sort_2n_vector(inp1.data(), size);

        std::sort(std::begin(inp0), std::end(inp0));

        for (int i = 0; i < size; i++) {
            ASSERT_EQ(inp1[i], inp0[i]);
        }
    }
}

TEST(SORT, TEST_2N_SORT_FLOAT_VER_TEST2) {
    for (int k = 0; k < 5; k++) {
        for (int j = 0; j < 5; j++) {
            unsigned size = 8192;
            size *= (1 << k); //  1 << 2 = (int) Math.pow(2, 2)
            unsigned move_start = 1 << k;
            unsigned move_end = 1 << k;

            aligned_vector<float> inp0;
            aligned_vector<float> inp1;
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

TEST(SORT, TEST_8N_SORT_FLOAT_VECTOR_TEST1) {
    {
        unsigned size = 4000;
        aligned_vector<float> inp0;
        aligned_vector<float> inp1;
        inp0.reserve(size);
        for (unsigned i = 0; i < size; i++)
            inp0.push_back(random_float());

        inp1 = inp0;

        BITONIC_SORT::sort_8n_vector(inp1.data(), size);

        std::sort(std::begin(inp0), std::end(inp0));

        for (unsigned i = 0; i < size; i++) {
            ASSERT_EQ(inp1[i], inp0[i]);
        }
    }

    for (int i = 0; i < 2; i++) {

        unsigned move_start = (random_pos_int() % 8) * 8;
        unsigned move_end = (random_pos_int() % 8) * 8;
        unsigned size =
            move_start + move_end + 8 * random_pos_int();

        aligned_vector<float> inp0;
        aligned_vector<float> inp1;

        inp0.reserve(size);

        for (unsigned i = 0; i < size; i++) {
            inp0.push_back(random_float());
            // std::cout << i << ": " << inp0[i] << std::endl;
        }

        inp1 = inp0;

        auto p1 = std::next(inp0.begin(), move_start);
        auto p2 = std::prev(inp0.end(), move_end);

        BITONIC_SORT::sort_8n_vector(inp1.data() + move_start,
                                     size - move_end - move_start);
        std::sort(p1, p2);

        for (unsigned i = move_start; i < size - move_end; i++) {
            // std::cout << i << ": " << inp1[i] << "  " << inp0[i]
            //          << std::endl;
            ASSERT_EQ(inp1[i], inp0[i]);
        }
    }
}

TEST(SORT, TEST_SORT_FLOAT_VECTOR_ALL_CASES) {

    for (unsigned size = 1; size < 4000; size++) {
        aligned_vector<float> inp0;
        aligned_vector<float> inp1;
        inp0.reserve(size);
        for (unsigned i = 0; i < size; i++)
            inp0.push_back(random_float());

        inp1 = inp0;
        BITONIC_SORT::sort_vector(inp1.data(), size);

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
}
