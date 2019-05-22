#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <immintrin.h>
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
#include <thread>                            // std::this_thread::sleep_for
#include <chrono>

#include "apex_memmove.h"
#include "type_definitions.hpp"
#include "convex_hull.hpp"

#include "auxiliary_functions.hpp"
#include "print_functions.hpp"

#include "ising_shader.hpp"
#include "spin_array.hpp"

#include "ising_windows.hpp"

void settings_window(IsingModel<float> &algorithm, char &algorithm_choice,
                     std::vector<float> &energy,
                     std::vector<float> &magnetization) {

    static int counter = 0;
    counter++;
    std::cout << "counter: " << counter << std::endl;
    bool bvar = true;
    int ivar = 12345678;
    double dvar = 3.1415926;
    float fvar = (float)dvar;
    std::string strval = "A string";
    test_enum enumval = Item2;
}
