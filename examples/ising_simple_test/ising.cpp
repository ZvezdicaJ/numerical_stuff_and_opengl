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
#include <iomanip>
#include <sstream>

#include "apex_memmove.h"
#include "type_definitions.hpp"
#include "convex_hull.hpp"

#include "auxiliary_functions.hpp"
#include "print_functions.hpp"
#include "Ising_model.hpp"

int main() {

    unsigned size = 130;
    IsingModel<float> alg1(size);
    alg1.set_temperature(2.2);

    /*
    // for (int i = 0; i < 100; i++) {
        alg1.metropolis_steps(128 * 128);

        std::cout << "magnetization: " << alg1.get_magnetization()
                  << std::endl;
                  }*/
    int *p = (int *)alg1.get_spin_array();
    /*
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            std::cout << *(p + j + size * i) << " ";
        }
        std::cout << std::endl;
    }

    for (int i = 0; i < 10; i++) {
        alg1.flip_cluster_nonrecursive_vectorized();
    }
    */
    // int tau = 25;

    for (int tau = 25; tau < 101; tau += 25) {
        double temp = 2.2;             // 5
        double max_field = 0.1*temp; // 1.5;
        int samples = 1500;
        std::ostringstream stream_data;
        // Set Fixed -Point Notation
        stream_data << std::fixed;
        // Set precision to 2 digits
        stream_data << std::setprecision(2);
        // Add double to stream
        stream_data << temp << "_max_field_" << max_field << "_tau_"
                    << tau;
        // Get string from output string stream
        std::string strObj3 = stream_data.str();

        std::string file_name("../data/work_distribution_temp_" +
                              stream_data.str() + ".dat");
        alg1.calculate_work_distribution(temp, max_field, tau,
                                         samples, file_name);
        std::cout << "tau: " << tau << std::endl;
    }
}
