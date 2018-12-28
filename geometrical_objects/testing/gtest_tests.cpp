#include <algorithm>
#include <array>
#include <cmath>
#include <ctime>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <xmmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gtest_auxiliary_functions.hpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
