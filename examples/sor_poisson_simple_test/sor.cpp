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
#include "poisson_sor.hpp"

int main() {

    unsigned size = 20;
    float dx = 0.1;
    float dy = 0.1;
    PoissonSOR<FORCE::UNIFORM, float> sor(20, 10, dx, dy);
    sor.set_boundary_conditions_uniform(0.5);
    sor.print_solution(2);
    sor.set_q(-0.5);
    sor.set_omega(1.1);

    for (int i = 0; i < 20; i++) {
        sor.iteration();
        std::cout << std::endl;
        sor.print_solution(2);
    }
    return 0;
}
