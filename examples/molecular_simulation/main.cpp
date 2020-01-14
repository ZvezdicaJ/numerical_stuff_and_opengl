#include <xmmintrin.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <ctime>
#include <string>
#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <random>
#include <list>
#include <functional>

#include <thread> // std::this_thread::sleep_for
#include <chrono>
#include <fstream>

#include "molecular_monte_carlo.hpp"

int main() {

    unsigned num_particles = 100;
    double boxsize = 10;
    double temperature = 1;
    MolecularMonteCarlo<double> sim(num_particles, boxsize);
    sim.set_temperature(1);

    for (int k = 0; k < 1000; k++)
        sim.move_particle();

    for (int i = 0; i < 100000000; i++) {
        sim.move_particle();

        if (i % 1000000 == 0)
            std::cout << i << "  energy: " << sim.get_energy()
                      << std::endl;
    }

    return 0;
}
