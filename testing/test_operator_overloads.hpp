#include <array>
#include "overloads_for_std_containers.hpp"

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

TEST(Array_Operator_Overloads, array_float_general_multiplication) {
    test_multiplication_operator<float>();
}

TEST(Array_Operator_Overloads, array_double_general_multiplication) {
    test_multiplication_operator<double>();
}
TEST(Array_Operator_Overloads, array_int_general_multiplication) {
    test_multiplication_operator<int>();
}
