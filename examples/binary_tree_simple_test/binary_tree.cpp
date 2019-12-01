#include <xmmintrin.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <ctime>
#include <string>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <random>
#include <list>
#include <functional>
#include <boost/align/aligned_allocator.hpp> // this is for aligned std::vector
#include <thread> // std::this_thread::sleep_for
#include <chrono>
#include <fstream>

#include "apex_memmove.h"
#include "type_definitions.hpp"
#include "convex_hull.hpp"

#include "auxiliary_functions.hpp"
#include "print_functions.hpp"
#include "binary_tree.hpp"

std::default_random_engine generator(std::time(0));
std::uniform_real_distribution<float> float_dist(-100, 100);
std::uniform_real_distribution<double> double_dist(-100, 100);
std::uniform_int_distribution<int> int_dist(-10000000, 10000000);
std::uniform_int_distribution<int> pos_int_dist(1, 10000000);
auto random_float = std::bind(float_dist, generator);
auto random_double = std::bind(double_dist, generator);
auto random_int = std::bind(int_dist, generator);
auto random_pos_int = std::bind(pos_int_dist, generator);

int main() {

    unsigned size = 10;
    aligned_vector<int> vec;
    for (int i = 0; i < size; i++) {
        vec.push_back(random_pos_int() % 10);
    }

    binary_tree<int, TREE_TYPE::BALANCED> tree(vec.data(),
                                               vec.size());
    tree.print_to_file("../tree_structure.dat");
    return 0;
}
