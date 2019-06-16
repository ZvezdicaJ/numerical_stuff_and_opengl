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

#include "apex_memmove.h"
#include "type_definitions.hpp"
#include "convex_hull.hpp"

#include "auxiliary_functions.hpp"
#include "print_functions.hpp"
#include "Ising_model.hpp"

int main() {

    unsigned size = 20;
    IsingModel<float> alg1(size);
    alg1.set_temperature(0.5);
    int *p = (int *)alg1.get_spin_array();
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            std::cout << *(p + j + size * i) << " ";
        }
        std::cout << std::endl;
    }

    for (int i = 0; i < 10; i++) {
        alg1.flip_cluster_nonrecursive_vectorized();
    }
    /*
    if (algorithm_choice == 'M')
        alg1.metropolis_steps(size * size);
    else if (algorithm_choice == 'W')
        alg1.flip_cluster();
    else if (algorithm_choice == 'N')
        alg1.flip_cluster();
    */
}
