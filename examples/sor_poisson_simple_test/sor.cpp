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
#include "poisson_sor.hpp"

int main() {

    std::ofstream convergence_data;
    convergence_data.open("../sor_convergence_data.dat");
    convergence_data << "omega num_iterations" << std::endl;

    std::ofstream eigenvalue_data;
    eigenvalue_data.open("../sor_eigenvalue_approx.dat");
    eigenvalue_data << "omega eigenvalue_approx" << std::endl;

    for (int k = 0; k < 2000; k++) {
        unsigned size = 20;
        double dx = 0.01;
        double dy = 0.01;
        PoissonSOR<FORCE::UNIFORM, double> sor(16, 16, dx, dy);
        sor.set_boundary_conditions_uniform(0.5);
        // sor.print_solution(2);
        sor.set_q(-0.0);
        double omega = 0.01 + 0.001 * (double)k;
        sor.set_omega(omega);

        int counter = 0;
        do {
            sor.iteration();
            // std::cout << std::endl;
            // sor.print_solution(1);
            counter++;
            // std::cout << "iteration: " << counter << std::endl;
            // std::cout << "sor change: " << sor.get_max_change()
            //          << std::endl;
            if (counter > 15000)
                break;
        } while (sor.get_max_change() > 0.001);

        std::cout << "omega: " << omega << " counter: " << counter
                  << " max eigenvalue: "
                  << sor.calculate_max_eigenvalue_approximation()
                  << std::endl;

        convergence_data << omega << " " << counter << std::endl;
        eigenvalue_data
            << omega << " "
            << sor.calculate_max_eigenvalue_approximation()
            << std::endl;
    }
    // sor.print_solution("../solution.dat");

    return 0;
}
