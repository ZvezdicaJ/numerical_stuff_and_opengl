#include <array>
#include "overloads_for_std_containers.hpp"
#include <type_traits>

template <typename T>
void test_multiplication_operator() {
    {
        const unsigned size = 15;
        std::array<T, size> arr1;
        std::array<T, size> arr2;
        for (int i = 0; i < size; i++) {
            arr1[i] = (T)random_double();
            arr2[i] = (T)random_double();
        }
        std::array<T, size> result = arr1 * arr2;
        for (int i = 0; i < size; i++) {
            T mul = arr1[i] * arr2[i];
            ASSERT_EQ(result[i], mul);
        }
    }

    {
        const unsigned size = 4;
        std::array<T, size> arr1;
        std::array<T, size> arr2;
        for (int i = 0; i < size; i++) {
            arr1[i] = (T)random_double();
            arr2[i] = (T)random_double();
        }
        std::array<T, size> result = arr1 * arr2;
        for (int i = 0; i < size; i++) {
            T mul = arr1[i] * arr2[i];
            ASSERT_EQ(result[i], mul);
        }
    }
}

TEST(ARRAY_OPERATOR_OVERLOADS, ARRAY_FLOAT_GENERAL_MULTIPLICATION) {
    test_multiplication_operator<float>();
}

TEST(ARRAY_OPERATOR_OVERLOADS,
     ARRAY_DOUBLE_GENERAL_MULTIPLICATION) {
    test_multiplication_operator<double>();
}
TEST(ARRAY_OPERATOR_OVERLOADS, ARRAY_INT_GENERAL_MULTIPLICATION) {
    test_multiplication_operator<int>();
}

template <typename T>
void test_addition_operator() {
    {
        const unsigned size = 15;
        std::array<T, size> arr1;
        std::array<T, size> arr2;
        for (int i = 0; i < size; i++) {
            arr1[i] = (T)random_double();
            arr2[i] = (T)random_double();
        }
        std::array<T, size> result = arr1 + arr2;
        for (int i = 0; i < size; i++) {
            T mul = arr1[i] + arr2[i];
            ASSERT_EQ(result[i], mul);
        }
    }

    {
        const unsigned size = 4;
        std::array<T, size> arr1;
        std::array<T, size> arr2;
        for (int i = 0; i < size; i++) {
            arr1[i] = (T)random_double();
            arr2[i] = (T)random_double();
        }
        std::array<T, size> result = arr1 + arr2;
        for (int i = 0; i < size; i++) {
            T mul = arr1[i] + arr2[i];
            ASSERT_EQ(result[i], mul);
        }
    }
}

TEST(ARRAY_OPERATOR_OVERLOADS, ARRAY_FLOAT_GENERAL_ADDITION) {
    test_addition_operator<float>();
}

TEST(ARRAY_OPERATOR_OVERLOADS, ARRAY_DOUBLE_GENERAL_ADDITION) {
    test_addition_operator<double>();
}
TEST(ARRAY_OPERATOR_OVERLOADS, ARRAY_INT_GENERAL_ADDITION) {
    test_addition_operator<int>();
}

template <typename T>
void test_subtraction_operator() {
    {
        const unsigned size = 15;
        std::array<T, size> arr1;
        std::array<T, size> arr2;
        for (int i = 0; i < size; i++) {
            arr1[i] = (T)random_double();
            arr2[i] = (T)random_double();
        }
        std::array<T, size> result = arr1 - arr2;
        for (int i = 0; i < size; i++) {
            T mul = arr1[i] - arr2[i];
            ASSERT_EQ(result[i], mul);
        }
    }

    {
        const unsigned size = 4;
        std::array<T, size> arr1;
        std::array<T, size> arr2;
        for (int i = 0; i < size; i++) {
            arr1[i] = (T)random_double();
            arr2[i] = (T)random_double();
        }
        std::array<T, size> result = arr1 - arr2;
        for (int i = 0; i < size; i++) {
            T mul = arr1[i] - arr2[i];
            ASSERT_EQ(result[i], mul);
        }
    }
}

TEST(ARRAY_OPERATOR_OVERLOADS, ARRAY_FLOAT_GENERAL_SUBTRACTION) {
    test_subtraction_operator<float>();
}

TEST(ARRAY_OPERATOR_OVERLOADS, ARRAY_DOUBLE_GENERAL_SUBTRACTION) {
    test_subtraction_operator<double>();
}
TEST(ARRAY_OPERATOR_OVERLOADS, ARRAY_INT_GENERAL_SUBTRACTION) {
    test_subtraction_operator<int>();
}

template <typename T>
void test_division_operator() {
    {
        const unsigned size = 15;
        std::array<T, size> arr1;
        std::array<T, size> arr2;
        for (int i = 0; i < size; i++) {
            arr1[i] = (T)random_double();
            arr2[i] = (T)random_double();
        }
        std::array<T, size> result = arr1 / arr2;
        for (int i = 0; i < size; i++) {
            T mul = arr1[i] / arr2[i];
            if constexpr (std::is_same<T, double>::value) {
                EXPECT_THAT(
                    result[i],
                    testing::DoubleNear(arr1[i] / arr2[i], 1e-12));
            }
            if constexpr (std::is_same<T, float>::value) {
                EXPECT_THAT(result[i],
                            testing::FloatNear(mul, 1e-5));
            }

            if constexpr (std::is_same<T, int>::value) {

                ASSERT_EQ(result[i], mul);
            }
        }
    }

    {
        const unsigned size = 4;
        std::array<T, size> arr1;
        std::array<T, size> arr2;
        for (int i = 0; i < size; i++) {
            arr1[i] = (T)random_double();
            arr2[i] = (T)random_double();
        }
        std::array<T, size> result = arr1 / arr2;
        for (int i = 0; i < size; i++) {

            T mul = arr1[i] / arr2[i];
            if constexpr (std::is_same<T, double>::value) {
                EXPECT_THAT(
                    result[i],
                    testing::DoubleNear(arr1[i] / arr2[i], 1e-12));
            }
            if constexpr (std::is_same<T, float>::value) {
                EXPECT_THAT(result[i],
                            testing::FloatNear(mul, 1e-5));
            }

            if constexpr (std::is_same<T, int>::value) {
                ASSERT_EQ(result[i], mul);
            }
        }
    }
}

TEST(ARRAY_OPERATOR_OVERLOADS, ARRAY_FLOAT_GENERAL_DIVISION) {
    test_division_operator<float>();
}

TEST(Array_Operator_Overloads, array_double_general_division) {
    test_division_operator<double>();
}

TEST(Array_Operator_Overloads, array_int_general_division) {
    test_division_operator<int>();
}
