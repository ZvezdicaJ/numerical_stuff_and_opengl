#pragma once
#include <random>
#include <iostream>
#include <fstream>
#include <functional>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include "immintrin.h"

#include "type_definitions.hpp"

template <typename T>
class MolecularMonteCarlo {
  private:
    aligned_vector<T> x;  /**< an array of x coordinates  */
    aligned_vector<T> y;  /**< an array of y coordinates  */
    aligned_vector<T> z;  /**< an array of z coordinates  */
    aligned_vector<T> vx; /**< an array of vx velocities  */
    aligned_vector<T> vy; /**< an array of vy velocities  */
    aligned_vector<T> vz; /**< an array of vz velocities  */
    T *potentials; /**< an array of potential energies between
                      particles */

    std::random_device rd; /**< std random device  */
    std::mt19937 rng;      /**< std random generator  */
    std::uniform_int_distribution<int>
        random_int; /**< integer distribution, returning an integer
                       from 0 to number of particles - 1 */
    std::uniform_real_distribution<T>
        random_real; /**< real valued distribution returning values
                        from 0 to 1.  */
    std::normal_distribution<double>
        random_normal_number; /**< gaussian distribution with  0
                                 mean and 1 variance */

    T eps = 1;   /**< first parameter for Leonard-Jones */
    T sigma = 1; /**< second parameter for Leonard-Jones */
    T rho = 1;   /**< density  */
    unsigned num_particles; /**< number of particles */
    T temperature;
    T box_size; /**< box size calculated from number of particles
                   and density */

  public:
    MolecularMonteCarlo(MolecularMonteCarlo &&) = delete;
    MolecularMonteCarlo &operator=(MolecularMonteCarlo &&) = delete;
    MolecularMonteCarlo(const MolecularMonteCarlo &) = delete;
    MolecularMonteCarlo &
    operator=(const MolecularMonteCarlo &) = delete;

    /**
        @brief Constructor initializes all the arrays and random
       number generator, sets box size,...
        @details

        \param num_particles_	number of particles in the
       simulation
    */
    MolecularMonteCarlo(const unsigned num_particles_)
        : num_particles(num_particles),
          random_int(std::uniform_int_distribution<int>(
              1, num_particles_)),
          random_real(std::uniform_real_distribution<T>(0, 1)),
          random_normal_number(
              std::normal_distribution<T>(5.0, 2.0)) {
        rng.seed(::time(NULL));
        x.reserve(num_particles_);
        y.reserve(num_particles_);
        z.reserve(num_particles_);
        vx.reserve(num_particles_);
        vy.reserve(num_particles_);
        vz.reserve(num_particles_);
        potentials = (double *)std::aligned_alloc(
            32, num_particles_ * num_particles_ * sizeof(T));
    }

    /**
        @brief calculates Leonard-Jones potential given a relative
       position of two particles
        @details

        \param rx relative position in x direction between two
       particles
       \param ry  relative position in y direction between
       two particle
       \param rz relative position in z direction
       between two particle

        @return <ReturnValue>
    */
    inline T potential(const T &rx, const T &ry, const T &rz) {

        if constexpr (std::is_same_v<T, double>) {
            //if    double execute this
        }
        return eps / std::pow(rx * rx + ry * ry + rz * rz, 6) -
               sigma / std::pow(rx * rx + ry * ry + rz * rz, 3);
    }

    /**
        @brief Calculates potential energy change when moving a
       particle
        @details

        \param particle particle number to be move; from 0 to
       num_particles-1
       \param dx change of dx coordinate
       \param dy  change of dy coordinate
       change of dx coordinate
       \param dz change of dz coordinate
        @return <ReturnValue>
    */
    inline T potential_energy_change(const unsigned &particle,
                                     const T &dx, const T &dy,
                                     const T &dz) {
        T dV = 0.0;
        T xp = x[particle];
        T yp = y[particle];
        T zp = z[particle];
        for (int i = 0; i < particle; i++) {
            T x2 = x[i];
            T y2 = y[i];
            T z2 = z[i];

            T rx = xp - x2;
            rx -= box_size * std::round(rx / box_size);
            T ry = yp - y2;
            ry -= box_size * std::round(ry / box_size);
            T rz = zp - z2;
            rz -= box_size * std::round(rz / box_size);

            dV += (potential(rx + dx, ry + dy, rz + dz) -
                   potential(rx, ry, rz));
        }

        for (int i = particle + 1; i < num_particles; i++) {
            T x2 = x[i];
            T y2 = y[i];
            T z2 = z[i];

            T rx = xp - x2;
            rx -= box_size * std::round(rx / box_size);
            T ry = yp - y2;
            ry -= box_size * std::round(ry / box_size);
            T rz = zp - z2;
            rz -= box_size * std::round(rz / box_size);

            dV += (potential(rx + dx, ry + dy, rz + dz) -
                   potential(rx, ry, rz));
        }
        return dV;
    };

    /**
        @brief The function move particle using Monte Carlo
       algorithm
        @details

        \param
    */
    void move_particle() {

        int particle = random_int(rng);
        T distance = random_normal_number(rng);
        T theta = random_real(rng);
        T fi = random_real(rng);

        T dz = distance * std::cos(theta);
        T sintheta = std::sin(theta);
        T dx = distance * std::cos(fi) * sintheta;
        T dy = distance * std::sin(fi) * sintheta;

        T potential_energy_change =
            potential_change(particle, dx, dy, dz);

        if (potential_energy_change < (T)0) {
            x[particle] += dx;
            y[particle] += dy;
            z[particle] += dz;
        } else if (std::exp(-potential_energy_change /
                            temperature) > random_real()) {
            x[particle] += dx;
            y[particle] += dy;
            z[particle] += dz;
        } else {
        }
    }
};
