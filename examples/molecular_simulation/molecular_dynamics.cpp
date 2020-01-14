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
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <omp.h>
#include <chrono>
#include <fstream>
#include <thread>  // std::this_thread::sleep_for

#define EIGEN_USE_MKL_ALL
#include "molecular_dynamics.hpp"

void density_temperature_dependance() {
  int finished = 0;
  constexpr unsigned num_particles = 6;
  const std::string filename(
      "../md_data/"
      "density_temperature_data2_num_particles_" +
      std::to_string(num_particles) + ".dat");

  std::ofstream file(filename.c_str());

#pragma omp parallel for
  for (int u = 10; u < 11; u++) {
    for (int i = 10; i < 11; i++) {
      double density = 0.05 * (double)i;

      double boxsize = std::cbrt(num_particles / density);

      double temperature = 0.05 * (double)u;

      MolecularDynamics<double, num_particles> sim(density, 0.00001, 0.4);

      sim.set_temperature(temperature);

      for (int k = 0; k < 0; k++) sim.bussi_parinello_step();

      double avg_ener = 0;
      double avg_ener2 = 0;
      double pressure = 0;
      double pressure2 = 0;
      int num_samples = 0;

      for (int j = 0; j < 100; j++) {
        sim.position_verlet();

        if (j % 1 == 0) {
          double e = sim.get_potential_energy();
          double p = sim.get_pressure();
          avg_ener += e;
          avg_ener2 += e * e;
          pressure += p;
          pressure2 += p * p;
          num_samples++;
        }
      }
#pragma omp critical
      {
        file << temperature << " " << density << " " << avg_ener / num_samples
             << " " << avg_ener2 / num_samples << " " << pressure / num_samples
             << " " << pressure2 / num_samples << std::endl;
        finished++;
      }

      std::cout << "finished:  " << finished
                << "  density:  " << num_particles / std::pow(boxsize, 3)
                << "  temperature:  " << temperature << std::endl;

      std::cout << temperature << " " << density << " " << avg_ener << " "
                << avg_ener2 << " " << pressure << " " << pressure2
                << std::endl;
    }
  }
}

void energy_conservation_test() {
  int finished = 0;
#pragma omp parallel for
  for (int u = 1; u < 20; u++) {
    for (int i = 1; i < 15; i++) {
      double density = 0.1 * (double)i;
      double temperature = 0.1 * (double)u;

      constexpr unsigned NumParticles = 100;
      double gamma = 0;

      MolecularDynamics<double, NumParticles> sim(density, 0.0001, gamma);

      sim.set_temperature(temperature);
      sim.set_lattice_positions();

      std::stringstream density_string;
      density_string << std::fixed << std::setprecision(2) << density;

      std::stringstream temp_string;
      temp_string << std::fixed << std::setprecision(2) << temperature;

      const std::string filename("../md_data/energy_conservation_density" +
                                 density_string.str() + "_temperature" +
                                 temp_string.str() + "_num_particles" +
                                 std::to_string(NumParticles) + ".dat");

      const std::string filename2("../md_data/pressure_density" +
                                  density_string.str() + "_temperature" +
                                  temp_string.str() + "_num_particles" +
                                  std::to_string(NumParticles) + ".dat");

      std::ofstream energy_file(filename.c_str());
      std::ofstream pressure_file(filename2.c_str());

      for (int i = 0; i < 100000; i++) {
        // sim.position_verlet();
        sim.bussi_parinello_step();
        if (i % 10 == 0) {
          energy_file << sim.get_potential_energy() << " "
                      << sim.get_kinetic_energy() << "\n";

          pressure_file << sim.get_pressure() << std::endl;
        }
      }
#pragma omp critical
      {
        finished++;
        std::cout << "finished: " << finished << std::endl;
      }
    }
  }
}

void thermalisation_period() {
  int finished = 0;
#pragma omp parallel for
  for (int k = 1; k < 11; k++) {
    for (int u = 0; u < 11; u++) {
      for (int i = 0; i < 11; i++) {
        double density = 0.05 + 0.2 * (double)i;
        double temperature = 0.05 + 0.2 * (double)u;

        constexpr unsigned NumParticles = 100;
        double gamma = 0.1 * (double)k;

        MolecularDynamics<double, NumParticles> sim(density, 0.001, gamma);

        sim.set_temperature(temperature);
        sim.set_lattice_positions();

        std::stringstream density_string;
        density_string << std::fixed << std::setprecision(2) << density;

        std::stringstream temp_string;
        temp_string << std::fixed << std::setprecision(2) << temperature;

        std::stringstream gamma_string;
        gamma_string << std::fixed << std::setprecision(2) << gamma;

        const std::string filename(
            "../md_data/thermalisation_density" + density_string.str() +
            "_temperature" + temp_string.str() + "_gamma" + gamma_string.str() +
            "_num_particles" + std::to_string(NumParticles) + ".dat");

        const std::string filename2(
            "../md_data/pressure_density" + density_string.str() +
            "_temperature" + temp_string.str() + "_gamma" + gamma_string.str() +
            "_num_particles" + std::to_string(NumParticles) + ".dat");

        std::ofstream energy_file(filename.c_str());
        std::ofstream pressure_file(filename2.c_str());

        for (int i = 0; i < 100000; i++) {
          // sim.position_verlet();
          sim.bussi_parinello_step();
          if (i % 10 == 0) {
            energy_file << sim.get_potential_energy() << " "
                        << sim.get_kinetic_energy() << "\n";

            pressure_file << sim.get_pressure() << std::endl;
          }
        }
#pragma omp critical
        {
          finished++;
          std::cout << "finished: " << finished << std::endl;
        }
      }
    }
  }
}

template <unsigned NumParticles = 100>
void generate_data() {
  int finished = 0;
  double gamma = 0.4;

  std::stringstream gamma_string;
  gamma_string << std::fixed << std::setprecision(2) << gamma;

  const std::string averages("../md_data_energy_pressure/averages_gamma" +
                             gamma_string.str() + "_num_particles" +
                             std::to_string(NumParticles) + ".dat");
  std::ofstream averages_file(averages.c_str());

#pragma omp parallel for num_threads(12)
  for (int u = 1; u < 31; u++) {
    for (int i = 1; i < 31; i++) {
      double density = 0.05 * (double)i;
      double temperature = 0.05 * (double)u;

      MolecularDynamics<double, NumParticles> sim(density, 0.002, gamma);

      sim.set_temperature(temperature);
      sim.set_lattice_positions();

      std::stringstream density_string;
      density_string << std::fixed << std::setprecision(2) << density;

      std::stringstream temp_string;
      temp_string << std::fixed << std::setprecision(2) << temperature;

      const std::string filename(
          "../md_data_energy_pressure/energy_density" + density_string.str() +
          "_temperature" + temp_string.str() + "_gamma" + gamma_string.str() +
          "_num_particles" + std::to_string(NumParticles) + ".dat");

      const std::string filename2(
          "../md_data_energy_pressure/pressure_density" + density_string.str() +
          "_temperature" + temp_string.str() + "_gamma" + gamma_string.str() +
          "_num_particles" + std::to_string(NumParticles) + ".dat");

      std::ofstream energy_file(filename.c_str());
      std::ofstream pressure_file(filename2.c_str());

      sim.set_time_step(0.002);
      for (int i = 0; i < 100000; i++) sim.bussi_parinello_step();

      double average_pot = 0.0;
      double average_kin = 0.0;
      double average_p = 0.0;
      int samples = 0;
      std::vector<double> pot_energy;
      std::vector<double> kin_energy;
      std::vector<double> _energy;
      sim.set_time_step(0.002);
      for (int i = 0; i < 200000; i++) {
        sim.bussi_parinello_step();
        if (i % 100 == 0) {
          double V = sim.get_potential_energy();
          double Wk = sim.get_kinetic_energy();
          double p = sim.get_pressure();

          average_kin += Wk;
          average_pot += V;
          average_p += p;
          samples++;

          energy_file << V << " " << Wk << "\n";
          pressure_file << p << std::endl;
        }
      }
      averages_file << temperature << " " << density << " "
                    << average_pot / (double)samples << " "
                    << average_kin / (double)samples << " "
                    << average_p / (double)samples << std::endl;

#pragma omp critical
      {
        std::cout << temperature << " " << density << " "
                  << average_pot / (double)samples << " "
                  << average_kin / (double)samples << " "
                  << average_p / (double)samples << std::endl;

        finished++;
        std::cout << "finished: " << finished << std::endl;
      }
    }
  }
}

void generate_data_var_NumParticles() {
  // generate_data<50>();
  // generate_data<100>();
  generate_data<200>();
  // generate_data<200>();
  // generate_data<500>();
  // generate_data<1000>();
}

template <unsigned NumParticles = 1000>
void generate_data_large_num_particles() {
  int finished = 0;
  double gamma = 0.4;

  std::vector<double> temps = {0.05, 0.45, 0.6, 0.9, 1.1, 1.3};

  std::stringstream gamma_string;
  gamma_string << std::fixed << std::setprecision(2) << gamma;

  const std::string averages("../md_data_energy_pressure/averages_gamma" +
                             gamma_string.str() + "_num_particles" +
                             std::to_string(NumParticles) + ".dat");
  std::ofstream averages_file(averages.c_str());

  for (int u = 0; u < temps.size(); u++) {
#pragma omp parallel for num_threads(12)
    for (int i = 1; i < 31; i++) {
      double density = 0.05 * (double)i;
      double temperature = temps[u];

      MolecularDynamics<double, NumParticles> sim(density, 0.002, gamma);

      sim.set_temperature(temperature);
      sim.set_lattice_positions();

      std::stringstream density_string;
      density_string << std::fixed << std::setprecision(2) << density;

      std::stringstream temp_string;
      temp_string << std::fixed << std::setprecision(2) << temperature;

      const std::string filename(
          "../md_data_energy_pressure/energy_density" + density_string.str() +
          "_temperature" + temp_string.str() + "_gamma" + gamma_string.str() +
          "_num_particles" + std::to_string(NumParticles) + ".dat");

      const std::string filename2(
          "../md_data_energy_pressure/pressure_density" + density_string.str() +
          "_temperature" + temp_string.str() + "_gamma" + gamma_string.str() +
          "_num_particles" + std::to_string(NumParticles) + ".dat");

      std::ofstream energy_file(filename.c_str());
      std::ofstream pressure_file(filename2.c_str());

      sim.set_time_step(0.002);
      for (int i = 0; i < 100000; i++) sim.bussi_parinello_step();

      double average_pot = 0.0;
      double average_kin = 0.0;
      double average_p = 0.0;
      int samples = 0;
      std::vector<double> pot_energy;
      std::vector<double> kin_energy;
      std::vector<double> _energy;
      sim.set_time_step(0.002);
      for (int i = 0; i < 200000; i++) {
        sim.bussi_parinello_step();
        if (i % 100 == 0) {
          double V = sim.get_potential_energy();
          double Wk = sim.get_kinetic_energy();
          double p = sim.get_pressure();

          average_kin += Wk;
          average_pot += V;
          average_p += p;
          samples++;

          energy_file << V << " " << Wk << "\n";
          pressure_file << p << std::endl;
        }
      }
      averages_file << temperature << " " << density << " "
                    << average_pot / (double)samples << " "
                    << average_kin / (double)samples << " "
                    << average_p / (double)samples << std::endl;

#pragma omp critical
      {
        std::cout << temperature << " " << density << " "
                  << average_pot / (double)samples << " "
                  << average_kin / (double)samples << " "
                  << average_p / (double)samples << std::endl;

        finished++;
        std::cout << "finished: " << finished << std::endl;
      }
    }
  }
}

template <unsigned NumParticles = 80>
void generate_data_small_param_set() {
  int finished = 0;
  double gamma = 0.4;

  std::vector<double> temps = {0.05, 0.45, 0.9, 1.3};
  std::vector<double> densities = {0.05, 0.45, 0.9};

  std::stringstream gamma_string;
  gamma_string << std::fixed << std::setprecision(2) << gamma;

  const std::string averages("../md_data_energy_pressure/averages_gamma" +
                             gamma_string.str() + "_num_particles" +
                             std::to_string(NumParticles) + ".dat");
  std::ofstream averages_file(averages.c_str());

#pragma omp parallel for collapse(2)
  for (int u = 0; u < temps.size(); u++) {
    for (int i = 0; i < densities.size(); i++) {
      double density = densities[i];
      double temperature = temps[u];

      MolecularDynamics<double, NumParticles> sim(density, 0.002, gamma);

      sim.set_temperature(temperature);
      sim.set_lattice_positions();

      std::stringstream density_string;
      density_string << std::fixed << std::setprecision(2) << density;

      std::stringstream temp_string;
      temp_string << std::fixed << std::setprecision(2) << temperature;

      const std::string filename(
          "../md_data_energy_pressure/energy_density" + density_string.str() +
          "_temperature" + temp_string.str() + "_gamma" + gamma_string.str() +
          "_num_particles" + std::to_string(NumParticles) + ".dat");

      const std::string filename2(
          "../md_data_energy_pressure/pressure_density" + density_string.str() +
          "_temperature" + temp_string.str() + "_gamma" + gamma_string.str() +
          "_num_particles" + std::to_string(NumParticles) + ".dat");

      std::ofstream energy_file(filename.c_str());
      std::ofstream pressure_file(filename2.c_str());

      sim.set_time_step(0.002);
      for (int i = 0; i < 100000; i++) sim.bussi_parinello_step();

      double average_pot = 0.0;
      double average_kin = 0.0;
      double average_p = 0.0;
      int samples = 0;
      std::vector<double> pot_energy;
      std::vector<double> kin_energy;
      std::vector<double> _energy;
      sim.set_time_step(0.002);
      for (int i = 0; i < 200000; i++) {
        sim.bussi_parinello_step();
        if (i % 100 == 0) {
          double V = sim.get_potential_energy();
          double Wk = sim.get_kinetic_energy();
          double p = sim.get_pressure();

          average_kin += Wk;
          average_pot += V;
          average_p += p;
          samples++;

          energy_file << V << " " << Wk << "\n";
          pressure_file << p << std::endl;
        }
      }
      averages_file << temperature << " " << density << " "
                    << average_pot / (double)samples << " "
                    << average_kin / (double)samples << " "
                    << average_p / (double)samples << std::endl;

#pragma omp critical
      {
        std::cout << temperature << " " << density << " "
                  << average_pot / (double)samples << " "
                  << average_kin / (double)samples << " "
                  << average_p / (double)samples << std::endl;

        finished++;
        std::cout << "finished: " << finished << std::endl;
      }
    }
  }
}

int main() {
  // density_temperature_dependance();
  // thermalisation_period();
  // generate_data_var_NumParticles();
  // generate_data_large_num_particles<500>();

  generate_data_small_param_set<400>();
  //  generate_data_small_param_set<150>();
  // generate_data_small_param_set<250>();
  // generate_data_small_param_set<300>();

  return 0;
}
