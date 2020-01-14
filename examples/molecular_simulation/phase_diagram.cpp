#include <xmmintrin.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <ctime>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <random>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <omp.h>
#include <chrono>
#include <fstream>
#include <thread>  // std::this_thread::sleep_for

#include "molecular_monte_carlo.hpp"

void density_temperature_dependance() {
  int finished = 0;
  unsigned num_particles = 100;

  const std::string filename("../data/data_num_particles_" +
                             std::to_string(num_particles) + ".dat");
  std::ofstream file(filename.c_str());

#pragma omp parallel for num_threads(12)
  for (int u = 1; u < 50; u++) {
    for (int i = 1; i < 30; i++) {
      double density = 0.05 * (double)i;
      double temperature = 0.05 + 0.05 * (double)u;
      double boxsize = std::cbrt(num_particles / density);

      std::stringstream density_string;
      density_string << std::fixed << std::setprecision(2) << density;

      std::stringstream temp_string;
      temp_string << std::fixed << std::setprecision(2) << temperature;

      const std::string filename2("../data/error_density" +
                                  density_string.str() + "_temperature" +
                                  temp_string.str() + "_num_particles" +
                                  std::to_string(num_particles) + ".dat");

      std::ofstream file2(filename2.c_str());

      MolecularMonteCarlo<double> sim(num_particles, boxsize);
      sim.set_temperature(temperature);
      sim.set_lattice_positions();

      for (int k = 0; k < 250000; k++) sim.test_period();

      double avg_ener = 0;
      double avg_ener2 = 0;
      double pressure = 0;
      double pressure2 = 0;
      int num_samples = 0;
      std::vector<double> ener_vec;
      std::vector<double> p_vec;
      ener_vec.reserve(9000);
      p_vec.reserve(9000);

      for (int j = 0; j < 10000000; j++) {
        sim.move_particle();

        // std::cout<<"energy: " << sim.get_energy() <<
        // std::endl; std::cout<<" pressure: " <<
        // sim.get_pressure() << std::endl;
        /* for(auto c : sim.get_x_coords())
           std::cout<< c << " ";
        std::cout<<std::endl;
        */

        if (j % 1000 == 0) {
          double e = sim.get_energy();
          double p = sim.get_pressure();
          ener_vec.push_back(e);
          p_vec.push_back(p);

          avg_ener += e;
          avg_ener2 += e * e;
          pressure += p;
          pressure2 += p * p;
          num_samples++;
        }
        // std::cout<<std::endl;
      }
      pressure2 = pressure2 / num_samples;
      pressure = pressure / num_samples;
      avg_ener = avg_ener / num_samples;
      avg_ener2 = avg_ener2 / num_samples;

      std::vector<std::pair<double, int>> c0_ener = c0_estimator(ener_vec);
      std::vector<std::pair<double, int>> c0_p = c0_estimator(p_vec);

      for (auto pair : c0_ener)
        file2 << pair.first << " " << pair.second << " ";
      file2 << "\n";
      for (auto pair : c0_p) file2 << pair.first << " " << pair.second << " ";

#pragma omp critical
      {
        file << temperature << " " << density << " " << avg_ener << " "
             << avg_ener2 << " " << pressure << " " << pressure2 << std::endl;
        finished++;

        std::cout << "step_factor: " << sim.get_step_factor() << std::endl;
        std::cout << "acceptance_ratio: " << sim.get_acceptance_ratio()
                  << std::endl;
        std::cout << "finished:  " << finished
                  << "  density:  " << num_particles / std::pow(boxsize, 3)
                  << "  temperature:  " << temperature << std::endl;

        std::cout << temperature << " " << density << " " << avg_ener << " "
                  << avg_ener2 << " " << pressure << " " << pressure2
                  << std::endl;
        std::cout << "total_acceptance_ration: "
                  << sim.get_total_acceptance_ratio() << "\n"
                  << std::endl;
      }
    }
  }
}

void thermalisation_period_lattice() {
  int finished = 0;
  unsigned num_particles = 100;

#pragma omp parallel for  // num_threads(4)
  for (int u = 1; u < 20; u++) {
    for (int i = 1; i < 11; i++) {
      double density = 0.1 * (double)i;

      double boxsize = std::cbrt(num_particles / density);

      double temperature = 0.1 * (double)u;

      const std::string filename(
          "../data/thermalisation/"
          "density_" +
          std::to_string(density) + "_data2_num_particles_" +
          std::to_string(num_particles) + "temp_" +
          std::to_string(temperature) + ".dat");
      std::ofstream file(filename.c_str());

      MolecularMonteCarlo<double> sim(num_particles, boxsize);
      sim.set_temperature(temperature);
      sim.set_lattice_positions();

      /*#pragma omp critical
      {
      double *p = sim.get_distances2();
      for(int l=0; l<num_particles; l++){
          std::cout << *(p+l) <<std::endl;
      }
      std::cout<<"\n\n"<<std::endl;
      }*/

      for (int k = 0; k < 150000; k++) {
        sim.test_period();

        double ener = sim.get_energy();

        double pressure = sim.get_pressure();
        double delta = sim.get_step_factor();

        file << temperature << " " << density << " " << ener << " " << pressure
             << " " << delta << std::endl;
      }
      std::cout << "acceptance ration: " << sim.get_acceptance_ratio()
                << std::endl;
    }
  }
}

void thermalisation_period_random() {
  int finished = 0;
  unsigned num_particles = 100;

#pragma omp parallel for  // num_threads(4)
  for (int u = 1; u < 20; u++) {
    for (int i = 1; i < 11; i++) {
      double density = 0.1 * (double)i;

      double boxsize = std::cbrt(num_particles / density);

      double temperature = 0.1 * (double)u;

      const std::string filename(
          "../data/thermalisation/"
          "random_density_" +
          std::to_string(density) + "_data2_num_particles_" +
          std::to_string(num_particles) + "temp_" +
          std::to_string(temperature) + ".dat");
      std::ofstream file(filename.c_str());

      MolecularMonteCarlo<double> sim(num_particles, boxsize);
      sim.set_temperature(temperature);

      /*#pragma omp critical
      {
      double *p = sim.get_distances2();
      for(int l=0; l<num_particles; l++){
          std::cout << *(p+l) <<std::endl;
      }
      std::cout<<"\n\n"<<std::endl;
      }*/

      for (int k = 0; k < 150000; k++) {
        sim.test_period();

        double ener = sim.get_energy();

        double pressure = sim.get_pressure();
        double delta = sim.get_step_factor();

        file << temperature << " " << density << " " << ener << " " << pressure
             << " " << delta << std::endl;
      }
      std::cout << "acceptance ration: " << sim.get_acceptance_ratio()
                << std::endl;
    }
  }
}

void energy_pressure_data_generation() {
  int finished = 0;
  unsigned num_particles = 100;

#pragma omp parallel for num_threads(12)
  for (int u = 1; u < 50; u++) {
    for (int i = 1; i < 30; i++) {
      double density = 0.05 * (double)i;
      double temperature = 0.05 + 0.05 * (double)u;
      double boxsize = std::cbrt(num_particles / density);

      std::stringstream density_string;
      density_string << std::fixed << std::setprecision(2) << density;

      std::stringstream temp_string;
      temp_string << std::fixed << std::setprecision(2) << temperature;

      const std::string filename2(
          "../sample_data_mc/energy_pressure_samples_density" +
          density_string.str() + "_temperature" + temp_string.str() +
          "_num_particles" + std::to_string(num_particles) + ".dat");

      std::ofstream file2(filename2.c_str());

      MolecularMonteCarlo<double> sim(num_particles, boxsize);
      sim.set_temperature(temperature);
      sim.set_lattice_positions();

      for (int k = 0; k < 250000; k++) sim.test_period();

      for (int j = 0; j < 1000000; j++) {
        sim.move_particle();

        if (j % 100 == 0) {
          double e = sim.get_energy();
          double p = sim.get_pressure();

          file2 << e << " " << p << std::endl;
        }
      }
      finished++;
      std::cout << "finished: " << finished << std::endl;
    }
  }
}

int main() {
  thermalisation_period_lattice();
  thermalisation_period_random();

  density_temperature_dependance();
  energy_pressure_data_generation();

  return 0;
}
