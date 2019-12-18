#pragma once
#define EIGEN_USE_MKL_ALL
#include <random>
#include <iostream>
#include <fstream>
#include <functional>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cassert>
#include "immintrin.h"
#include <type_traits>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Eigen>

template <typename T, unsigned NumParticles>
class MolecularDynamics {
    static_assert(std::is_floating_point_v<T>);

  private:
    // all vectors are row vectors not column vectors!!

    Eigen::Matrix<T, 1, NumParticles>
        x; /**< an array of x coordinates  */

    Eigen::Matrix<T, 1, NumParticles>
        y; /**< an array of y coordinates  */

    Eigen::Matrix<T, 1, NumParticles>
        z; /**< an array of z coordinates  */

    Eigen::Matrix<T, 1, NumParticles>
        vx; /**< an array of vx velocities  */

    Eigen::Matrix<T, 1, NumParticles>
        vy; /**< an array of vy velocities  */

    Eigen::Matrix<T, 1, NumParticles>
        vz; /**< an array of vz velocities  */

    Eigen::Matrix<T, NumParticles, NumParticles>
        forces; /**< forces acting on particles */

    Eigen::Matrix<T, 1, NumParticles> force_sum;
    /**< forces acting on particles */ /**< distances between
                                          particles*/

    std::random_device rd; /**< std random device  */
    std::mt19937 rng;      /**< std random generator  */
    std::uniform_int_distribution<int>
        random_int; /**< integer distribution, returning an integer
                       from 0 to number of particles - 1 */
    std::uniform_real_distribution<T>
        random_real; /**< real valued distribution returning values
                        from 0 to 1.  */
    std::normal_distribution<T>
        random_normal_number; /**< gaussian distribution with  0
                                 mean and 1 variance */

    T t1 = 1;  /**< first parameter for Leonard-Jones, this is not
                  used */
    T t2 = 1;  /**< second parameter for Leonard-Jones, this is not
                  used */
    T rho = 1; /**< density  */
    T m = 1;
    T dt = 0;
    T gamma;

    T pv1, pv2, pv3, pvf; /**< pv signifies position verlet; these
                        are position verlet constants */
    T bp1, bp2, bp3; /**< bp signifies Bussi-Parinello; these are
                        position verlet constants */

    T temperature;
    T box_size; /**< box size calculated from number of particles
                   and density */
    T energy;   /**< total potential energy of the system */
    T density;  /**< density of the system */

  public:
    MolecularDynamics(MolecularDynamics &&) = delete;
    MolecularDynamics &operator=(MolecularDynamics &&) = delete;
    MolecularDynamics(const MolecularDynamics &) = delete;
    MolecularDynamics &
    operator=(const MolecularDynamics &) = delete;

    /**
        @brief Constructor initializes all the arrays and random
       number generator, sets box size,...
        @details

        \param num_particles_	number of particles in the
       simulation
    */
    MolecularDynamics(const T &box_size_, const T &dt_,
                      const T &gamma_)
        : random_int(std::uniform_int_distribution<int>(
              0, NumParticles - 1)),
          random_real(std::uniform_real_distribution<T>(0, 1)),
          random_normal_number(
              std::normal_distribution<T>(0.0, 1.0)),
          box_size(box_size_), dt(dt_), gamma(gamma_),
          density(NumParticles / box_size_),
          pv1(std::exp(-gamma_ * dt_)),
          pv2((1.0 - std::exp(-gamma_ * dt_)) / gamma_),
          pv3((1.0 - std::exp(-2.0 * gamma_ * dt_)) /
              (2.0 * gamma_)),
          bp1(std::exp(-gamma_ * dt_ / 2)) {
        rng.seed(::time(NULL));

        std::cout << "gamma: " << gamma << "   pv1: " << pv1
                  << "  pv2: " << pv2 << std::endl;

        x = (((box_size)*x.Random()).array()) / 2.0;
        vx.Zero();
        y = (((box_size)*y.Random()).array()) / 2.0;
        vy.Zero();
        z = (((box_size)*z.Random()).array()) / 2.0;
        vz.Zero();
        forces.Zero();
        bp3 = pv2;
        bp2 = std::sqrt(gamma * temperature * bp3);
        pvf = std::sqrt(2 * temperature * gamma * pv3);

        std::cout << "gamma: " << gamma << "   bp1: " << bp1
                  << "  bp2: " << bp2 << "  pvf: " << pvf
                  << std::endl;
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
    inline T potential(T rx, T ry, T rz) {
        T r6 = std::pow(rx * rx + ry * ry + rz * rz, 3);
        return 4 * (1 / (r6 * r6) - 1 / r6);
    }

    inline void compute_forces() {
        for (int i = 0; i < NumParticles; i++) {

            T x1 = x(0, i);
            T y1 = y(0, i);
            T z1 = z(0, i);
            /*            for (int j = i + 1; j < NumParticles; j++)
               { T rx = x1 - x(j,0); T ry = y1 - y(j,0); T rz = z1 -
               z(j,0);

                T tmp = potential_gradient(rx, ry, rz);
                forces(i, j) = tmp;
                forces(j, i) = tmp;
                }*/

            Eigen::Matrix<T, 1, NumParticles> distancex =
                (x.array() - x1);
            Eigen::Matrix<T, 1, NumParticles> distancey =
                (y.array() - y1);
            Eigen::Matrix<T, 1, NumParticles> distancez =
                (z.array() - z1);

            distancex =
                distancex.array() -
                box_size * ((distancex.array() / box_size).round());

            distancey =
                distancey.array() -
                box_size * ((distancey.array() / box_size).round());

            distancez =
                distancez.array() -
                box_size * ((distancez.array() / box_size).round());

            Eigen::Matrix<T, 1, NumParticles> r2vec =
                (distancex.array()).square() +
                (distancey.array()).square() +
                (distancez.array()).square();

            Eigen::Matrix<T, 1, NumParticles> rvec =
                r2vec.array().cwiseSqrt();

            Eigen::Matrix<T, 1, NumParticles> r6vec =
                r2vec.array().cube();

            Eigen::Matrix<T, 1, NumParticles> potentiali =
                (r6vec.cwiseProduct(r2vec).array() / (24.0))
                    .inverse();

            potentiali = potentiali.array().cwiseProduct(
                2.0 * (r6vec.array().inverse()) - 1.0);

            potentiali(0, i) = 0;
            // std::cout << potentiali << std::endl;
            forces.block(i, 0, 1, NumParticles) = potentiali;
            force_sum(0, i) = potentiali.array().sum();
            /*
            if (this->get_potential_energy() > 0) {
                std::cout << "forces: " << potentiali << std::endl;
                std::cout << "force sum: " << force_sum(0, i)
                          << std::endl;
                std::cout << "r: " << rvec << std::endl;
                std::cout
                    << "energy: " << (this->get_potential_energy())
                    << std::endl;
                std::cout << "-------------------------"
                          << std::endl;
                          }
            */
        }
        // std::cout << "\n" << forces << std::endl;
        // std::cout << "diag:\n " << forces.diagonal().transpose()
        //<< std::endl;
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
    inline T potential_gradient(T rx, T ry, T rz) {
        T r2 = (rx * rx + ry * ry + rz * rz);
        T r6 = std::pow(r2, 3);
        T r = std::sqrt(r2);
        return (24 / r6) * (1 / r - 2 / (r6 * r));
    }

    /**
        @brief The function move particle using Monte Carlo
       algorithm
        @details

        \param
    */
    void position_verlet() {
        T random1[NumParticles];
        T random2[NumParticles];
        T random3[NumParticles];

        // std::cout << "\nx before movment: " << x << std::endl;
        // std::cout << "\nvx before movment: " << vx << std::endl;

        const T f = 0; //= pvf;
        for (int u = 0; u < NumParticles; u++) {
            random1[u] = f * random_normal_number(rng);
            random2[u] = f * random_normal_number(rng);
            random3[u] = f * random_normal_number(rng);
        }

        Eigen::Matrix<T, 1, NumParticles> random_force1 =
            Eigen::Map<Eigen::Matrix<T, 1, NumParticles>>(random1);

        Eigen::Matrix<T, 1, NumParticles> random_force2 =
            Eigen::Map<Eigen::Matrix<T, 1, NumParticles>>(random2);

        Eigen::Matrix<T, 1, NumParticles> random_force3 =
            Eigen::Map<Eigen::Matrix<T, 1, NumParticles>>(random3);

        x = x + dt / 2 * vx;
        y = y + dt / 2 * vy;
        z = z + dt / 2 * vz;

        x = x.array() - box_size * ((x.array() / box_size).round());
        y = y.array() - box_size * ((y.array() / box_size).round());
        z = z.array() - box_size * ((z.array() / box_size).round());

        compute_forces();

        vx = (pv1 * vx +
              pv2 * (force_sum.cwiseProduct(x) - x * forces) +
              random_force1);
        vy = (pv1 * vy +
              pv2 * (force_sum.cwiseProduct(y) - y * forces) +
              random_force2);
        vz = (pv1 * vz +
              pv2 * (force_sum.cwiseProduct(z) - z * forces) +
              random_force3);

        x = x + dt / 2 * vx;
        y = y + dt / 2 * vy;
        z = z + dt / 2 * vz;

        x = x.array() - box_size * ((x.array() / box_size).round());
        y = y.array() - box_size * ((y.array() / box_size).round());
        z = z.array() - box_size * ((z.array() / box_size).round());

        // std::cout << "\nx after movment: " << x << std::endl;
        /*std::cout << "\nvx after movment: "
                  << (vx.cwiseProduct(vx) + vz.cwiseProduct(vz) +
                      vy.cwiseProduct(vy))
                  << std::endl;
        std::cout
            << "----------------------------------------------"
            << std::endl;
        */
    }

    inline void print_particle_position(
        const unsigned &particle,
        const std::string &text = std::string()) {
        std::cout << "x pos: " << x(particle, 0)
                  << "y pos: " << y(particle, 0)
                  << "z pos: " << z(particle, 0) << std::endl;
    }

    inline void set_density(const T &density_) {
        density = density_;
        box_size = std::cbrt(NumParticles / density_);
    }

    inline void set_temperature(const T &temperature_) {
        this->temperature = temperature_;
        bp2 = std::sqrt(gamma * temperature * bp3);
        pvf = std::sqrt(2 * temperature * gamma * pv3);
    }

    inline void set_time_step(const T &dt_) {
        dt = dt_;

        pv1 = std::exp(-gamma * dt_);
        pv2 = (1 - std::exp(-gamma * dt_)) / gamma;
        pv3 = (1 - std::exp(-2.0 * gamma * dt_)) / (2.0 * gamma);
        pvf = std::sqrt(2 * temperature * gamma * pv3);

        bp1 = std::exp(-gamma * dt_ / 2.0);
        bp2 = std::sqrt(gamma * temperature * bp3);
        bp3 = pv2;
    }

    inline void set_gamma(const T &gamma_) {
        gamma = gamma_;
        pv1 = std::exp(-gamma_ * dt);
        pv2 = (1 - std::exp(-gamma_ * dt)) / gamma_;
        pv3 = (1 - std::exp(-2.0 * gamma_ * dt)) / (2.0 * gamma_);
        pvf = std::sqrt(2 * temperature * gamma * pv3);

        bp1 = std::exp(-gamma_ * dt / 2.0);
        bp2 = std::sqrt(gamma_ * temperature * bp3);
        bp3 = pv2;
    }

    inline Eigen::Matrix<T, 1, NumParticles> get_velocity2() {
        return (vx.cwiseProduct(vx) + vy.cwiseProduct(vy) +
                vx.cwiseProduct(vz))
            .array()
            .sqrt();
    };

    inline Eigen::Matrix<T, 1, NumParticles> get_xpositions() {
        return x;
    };

    inline Eigen::Matrix<T, 1, NumParticles> get_ypositions() {
        return y;
    };

    inline Eigen::Matrix<T, 1, NumParticles> get_zpositions() {
        return z;
    };

    inline Eigen::Matrix<T, 1, NumParticles>
    get_distances(const unsigned &i) {

        T x1 = x(0, i);
        T y1 = y(0, i);
        T z1 = z(0, i);

        Eigen::Matrix<T, 1, NumParticles> distancex =
            (x.array() - x1);
        Eigen::Matrix<T, 1, NumParticles> distancey =
            (y.array() - y1);
        Eigen::Matrix<T, 1, NumParticles> distancez =
            (z.array() - z1);

        distancex =
            distancex.array() -
            box_size * ((distancex.array() / box_size).round());

        distancey =
            distancey.array() -
            box_size * ((distancey.array() / box_size).round());

        distancez =
            distancez.array() -
            box_size * ((distancez.array() / box_size).round());

        // std::cout << distancex << std::endl;

        Eigen::Matrix<T, 1, NumParticles> rvec =
            ((distancex.array()).square() +
             (distancey.array()).square() +
             (distancez.array()).square())
                .sqrt();
        // std::cout << "\n rvec: " << rvec << "\n" << std::endl;
        return rvec;
    };

    inline Eigen::Matrix<T, 1, NumParticles> get_potential() {
        Eigen::Matrix<T, 1, NumParticles> particle_potential;

        for (int i = 0; i < NumParticles; i++) {
            T x1 = x(0, i);
            T y1 = y(0, i);
            T z1 = z(0, i);

            Eigen::Matrix<T, 1, NumParticles> distancex =
                (x.array() - x1);
            Eigen::Matrix<T, 1, NumParticles> distancey =
                (y.array() - y1);
            Eigen::Matrix<T, 1, NumParticles> distancez =
                (z.array() - z1);

            distancex =
                distancex.array() -
                box_size * ((distancex.array() / box_size).round());

            distancey =
                distancey.array() -
                box_size * ((distancey.array() / box_size).round());

            distancez =
                distancez.array() -
                box_size * ((distancez.array() / box_size).round());

            Eigen::Matrix<T, 1, NumParticles> r2vec =
                (distancex.array()).square() +
                (distancey.array()).square() +
                (distancez.array()).square();

            Eigen::Matrix<T, 1, NumParticles> rvec =
                r2vec.array().cwiseSqrt();
            Eigen::Matrix<T, 1, NumParticles> r6vec =
                r2vec.array().cube();

            Eigen::Matrix<T, 1, NumParticles> tmp =
                4 * ((r6vec.cwiseProduct(r6vec)).array().inverse() -
                     r6vec.array().inverse());
            tmp(0, i) = 0;
            particle_potential(0, i) = tmp.sum();
        }
        return particle_potential;
    }

    inline T get_potential_energy() {
        return (this->get_potential()).sum() / 2;
    }

    inline T get_kinetic_energy() {
        return (vx.dot(vx) + vy.dot(vy) + vz.dot(vz)) / 2;
    }

    inline T get_energy() {
        return (this->get_potential_energy()) +
               (this->get_kinetic_energy());
    }

    /**
        @brief The function move particle using Monte Carlo
       algorithm
        @details

        \param
    */
    void bussi_parinello_step() {
        T random1[NumParticles];
        T random2[NumParticles];
        T random3[NumParticles];

        Eigen::Matrix<T, 1, NumParticles> random_force1;
        Eigen::Matrix<T, 1, NumParticles> random_force2;
        Eigen::Matrix<T, 1, NumParticles> random_force3;

        // std::cout << "Bussi - Parinello: \n\n" << std::endl;
        // std::cout << "\nx before movment: " << x << std::endl;
        // std::cout << "\nvx before movment: " << vx << std::endl;

        T f = bp2;
        for (int u = 0; u < NumParticles; u++) {
            random_force1(0, u) = f * random_normal_number(rng);
            random_force2(0, u) = f * random_normal_number(rng);
            random_force3(0, u) = f * random_normal_number(rng);
        }

        compute_forces();

        vx =
            (bp1 * vx + bp2 * random_force1 +
             (dt / 2.0) * (force_sum.cwiseProduct(x) - x * forces));
        vy =
            (bp1 * vy + bp2 * random_force2 +
             (dt / 2.0) * (force_sum.cwiseProduct(y) - y * forces));
        vz =
            (bp1 * vz + bp2 * random_force3 +
             (dt / 2.0) * (force_sum.cwiseProduct(z) - z * forces));

        x = x + dt * vx;
        y = y + dt * vy;
        z = z + dt * vz;

        x = x.array() - box_size * ((x.array() / box_size).round());
        y = y.array() - box_size * ((y.array() / box_size).round());
        z = z.array() - box_size * ((z.array() / box_size).round());

        compute_forces();

        for (int u = 0; u < NumParticles; u++) {
            random_force1(0, u) = f * random_normal_number(rng);
            random_force2(0, u) = f * random_normal_number(rng);
            random_force3(0, u) = f * random_normal_number(rng);
        }

        vx =
            (bp1 * vx + bp2 * random_force1 +
             (dt / 2.0) * (force_sum.cwiseProduct(x) - x * forces));
        vy =
            (bp1 * vy + bp2 * random_force2 +
             (dt / 2.0) * (force_sum.cwiseProduct(y) - y * forces));
        vz =
            (bp1 * vz + bp2 * random_force3 +
             (dt / 2.0) * (force_sum.cwiseProduct(z) - z * forces));

        // std::cout << "\nx after movment: " << x << std::endl;
        // std::cout << "\nvx after movment: " << vx << std::endl;
    }

    inline T get_virial_pressure() {
        T Vpressure = 0;

        for (int i = 0; i < NumParticles; i++) {
            T x1 = x(0, i);
            T y1 = y(0, i);
            T z1 = z(0, i);

            Eigen::Matrix<T, 1, Eigen::Dynamic> distancex(
                NumParticles - 1 - i);
            distancex = (x.array().block(0, i + 1, 1,
                                         NumParticles - 1 - i) -
                         x1);

            Eigen::Matrix<T, 1, Eigen::Dynamic> distancey(
                NumParticles - 1 - i);
            distancey = (y.array().block(0, i + 1, 1,
                                         NumParticles - 1 - i) -
                         y1);

            Eigen::Matrix<T, 1, Eigen::Dynamic> distancez(
                NumParticles - 1 - i);
            distancez = (z.array().block(0, i + 1, 1,
                                         NumParticles - 1 - i) -
                         z1);

            distancex =
                distancex.array() -
                box_size * ((distancex.array() / box_size).round());

            distancey =
                distancey.array() -
                box_size * ((distancey.array() / box_size).round());

            distancez =
                distancez.array() -
                box_size * ((distancez.array() / box_size).round());

            Eigen::Matrix<T, 1, Eigen::Dynamic> r2vec =
                (distancex.array()).square() +
                (distancey.array()).square() +
                (distancez.array()).square();

            Eigen::Matrix<T, 1, Eigen::Dynamic> rvec =
                r2vec.array().cwiseSqrt();

            Eigen::Matrix<T, 1, Eigen::Dynamic> r6vec =
                r2vec.array().cube();

            Eigen::Matrix<T, 1, Eigen::Dynamic> potentiali =
                (r6vec.array() / 24.0).inverse();

            potentiali = potentiali.array().cwiseProduct(
                rvec.array().inverse() -
                2 * ((r6vec.cwiseProduct(rvec)).array().inverse()));

            Vpressure = potentiali.dot(rvec);
        }
        return Vpressure;
    }

    /*    inline T get_virial_pressure() {
        T virial_pressure = 0;
        for (int k = 0;
             k < (num_particles * (num_particles - 1)) / 2; k++)
    { virial_pressure += std::sqrt(distances2[k]) *
    potential_grad[k];
        }
        return virial_pressure;
    }

    inline void print_potential(const unsigned &particle) {
        std::cout << "potential: " << std::endl;
        for (int i = 0; i < particle; i++) {
            std::cout << *(potentials + num_particles * i -
                           (i * (i + 1)) / 2 + particle - i - 1)
                      << " ";
        }
        for (int i = particle + 1; i < num_particles; i++) {
            std::cout << *(potentials + num_particles * particle
    - (particle * (particle + 1)) / 2 + i - particle - 1)
                      << " " << std::endl;
        }
        std::cout << "\n\n" << std::endl;
    };

    */
};
