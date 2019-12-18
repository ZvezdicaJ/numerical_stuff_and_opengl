#pragma once
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

template <typename T>
class MolecularMonteCarlo {
    static_assert(std::is_floating_point_v<T>);

  private:
    std::vector<T> x;  /**< an array of x coordinates  */
    std::vector<T> y;  /**< an array of y coordinates  */
    std::vector<T> z;  /**< an array of z coordinates  */
    std::vector<T> vx; /**< an array of vx velocities  */
    std::vector<T> vy; /**< an array of vy velocities  */
    std::vector<T> vz; /**< an array of vz velocities  */
    T *potentials;     /**< an array of potential energies between
                          particles */
    T *potential_grad; /**< an array of derivatives of potential
                      with respect to r*/
    T *distances2;     /**< distances between particles*/

    std::vector<T> new_potentials; /**< temporary storage for new
                                      interparticle potential - to
                                      avoid repeated calculation*/

    std::vector<T> new_distances2; /**< temporary storage for new
                                      interparticle distances - to
                                      avoid repeated calculation*/

    std::vector<T>
        new_potential_grad; /**< temporary storage for new
                           gradient of two particle potential - to
                           avoid repeated calculation*/

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
    unsigned num_particles; /**< number of particles */
    T temperature;
    T box_size; /**< box size calculated from number of particles
                   and density */

    T energy = 0;  /**< total potential energy of the system */
    T density = 0; /**< density of the system */
    int accepted = 0;
    int rejected = 0;
    int total_accepted = 0;
    int total_rejected = 0;
    int total_acceptance_ratio = 0;

    T step_factor = 1.0;
    T acceptance_ratio;

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
    MolecularMonteCarlo(const unsigned num_particles_, T box_size_)
        : num_particles(num_particles_),
          random_int(std::uniform_int_distribution<int>(
              0, num_particles_ - 1)),
          random_real(std::uniform_real_distribution<T>(0, 1)),
          random_normal_number(
              std::normal_distribution<T>(0.0, 1.0)),
          box_size(box_size_),
          density(num_particles_ /
                  (box_size_ * box_size_ * box_size_)) {
        rng.seed(::time(NULL));
        x.reserve(num_particles_);
        y.reserve(num_particles_);
        z.reserve(num_particles_);
        vx.reserve(num_particles_);
        vy.reserve(num_particles_);
        vz.reserve(num_particles_);

        new_potentials.reserve(num_particles_ - 1);
        new_potentials.resize(num_particles_ - 1);
        new_potential_grad.reserve(num_particles_ - 1);
        new_potential_grad.resize(num_particles_ - 1);
        new_distances2.reserve(num_particles_ - 1);
        new_distances2.resize(num_particles_ - 1);

        potentials = (T *)std::aligned_alloc(
            32, ((num_particles_ - 1) * num_particles_) / 2 *
                    sizeof(T));

        potential_grad = (T *)std::aligned_alloc(
            32, ((num_particles_ - 1) * num_particles_) / 2 *
                    sizeof(T));

        distances2 = (T *)std::aligned_alloc(
            32, ((num_particles_ - 1) * num_particles_) / 2 *
                    sizeof(T));

        set_random_positions();
        initialize_array();
        std::cout << "box size: " << box_size_ << std::endl;
    }

    void set_random_positions() {
        for (int i = 0; i < num_particles; i++) {
            x.push_back(box_size * (random_real(rng) - 0.5));
            y.push_back(box_size * (random_real(rng) - 0.5));
            z.push_back(box_size * (random_real(rng) - 0.5));
        }
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
        @brief The function initializes array of inter-particle
       potentials.
        @details The array saves pairwise potentials so that one
       does not have to calculate potential every time. To access an
       potential between particles i and j one should access the
       following:  k=max(i,j) and u = min(i,j)
        *(potentials + u*num_particles + k*(k+1)/2 )
    */
    void initialize_array() {
        int k = 0;
        energy = 0.0;
        for (int i = 0; i < num_particles; i++) {

            T x1 = x[i];
            T y1 = y[i];
            T z1 = z[i];

            for (int j = i + 1; j < num_particles; j++) {

                T x2 = x[j];
                T y2 = y[j];
                T z2 = z[j];

                T rx = x1 - x2;
                T ry = y1 - y2;
                T rz = z1 - z2;

                rx -= box_size * std::round(rx / (box_size));
                ry -= box_size * std::round(ry / (box_size));
                rz -= box_size * std::round(rz / (box_size));

                *(distances2 + k) = rx * rx + ry * ry + rz * rz;

                T tmp = potential(rx, ry, rz);
                T grad = potential_gradient(rx, ry, rz);

                energy += tmp;

                *(potentials + k) = tmp;
                *(potential_grad + k) = grad;
                k++;
            }
        }
    }

    inline T get_potential(const unsigned &i, const unsigned &j) {
#ifdef __DEBUG__
        if (i == j)
            assert(i != j &&
                   "get_potential: potential can only be "
                   "calculated for different molecules, "
                   "you tried to get potential for i==j!");
#endif

        unsigned min = (i < j ? i : j);
        unsigned max = (i > j ? i : j);
        return *(potentials + num_particles * min -
                 (min * (min + 1)) / 2 + max - min - 1);
    }

    inline T get_potential_grad(const unsigned &i,
                                const unsigned &j) {
#ifdef __DEBUG__
        if (i == j)
            assert(i != j &&
                   "get_potential: potential can only be "
                   "calculated for different molecules, "
                   "you tried to get potential for i==j!");
#endif

        unsigned min = (i < j ? i : j);
        unsigned max = (i > j ? i : j);
        return *(potential_grad + num_particles * min -
                 (min * (min + 1)) / 2 + max - min - 1);
    }

    inline void update_arrays(const unsigned &particle) {
        int k = 0;

        for (int i = 0; i < particle; i++) {

            *(potentials + num_particles * i - (i * (i + 1)) / 2 +
              particle - i - 1) = new_potentials[k];

            *(potential_grad + num_particles * i -
              (i * (i + 1)) / 2 + particle - i - 1) =
                new_potential_grad[k];

            *(distances2 + num_particles * i - (i * (i + 1)) / 2 +
              particle - i - 1) = new_distances2[k];

            k++;
        }

        for (int i = particle + 1; i < num_particles; i++) {

            *(potentials + num_particles * particle -
              (particle * (particle + 1)) / 2 + i - particle - 1) =
                new_potentials[k];

            *(potential_grad + num_particles * particle -
              (particle * (particle + 1)) / 2 + i - particle - 1) =
                new_potential_grad[k];

            *(distances2 + num_particles * i - (i * (i + 1)) / 2 +
              particle - i - 1) = new_distances2[k];

            k++;
        }
    };

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
    inline T new_potential_energy(const unsigned &particle,
                                  const T &xp, const T &yp,
                                  const T &zp) {
        T V = 0.0;
        int k = 0;
        for (int i = 0; i < particle; i++) {
            T x2 = x[i];
            T y2 = y[i];
            T z2 = z[i];

            T rx = xp - x2;
            T ry = yp - y2;
            T rz = zp - z2;

            rx -= box_size * std::round(rx / (box_size));
            ry -= box_size * std::round(ry / (box_size));
            rz -= box_size * std::round(rz / (box_size));

            T tmp = potential(rx, ry, rz);
            T grad = potential_gradient(rx, ry, rz);
            V += tmp;

            new_potentials[k] = tmp;
            new_potential_grad[k] = grad;
            new_distances2[k] = rx * rx + ry * ry + rz * rz;
            k++;
        }

        for (int i = particle + 1; i < num_particles; i++) {
            T x2 = x[i];
            T y2 = y[i];
            T z2 = z[i];

            T rx = xp - x2;
            T ry = yp - y2;
            T rz = zp - z2;

            rx -= box_size * std::round(rx / (box_size));
            ry -= box_size * std::round(ry / (box_size));
            rz -= box_size * std::round(rz / (box_size));

            T tmp = potential(rx, ry, rz);
            T grad = potential_gradient(rx, ry, rz);

            V += tmp;
            new_potentials[k] = tmp;
            new_potential_grad[k] = grad;
            new_distances2[k] = rx * rx + ry * ry + rz * rz;
            k++;
        }
        return V;
    };

    inline T new_potential_energy(const unsigned &particle) {
        T xp = x[particle];
        T yp = y[particle];
        T zp = z[particle];
        return potential_energy(particle, xp, yp, zp);
    };

    inline T new_potential_gradient(const unsigned &particle) {
        T xp = x[particle];
        T yp = y[particle];
        T zp = z[particle];
        return potential_gradient(particle, xp, yp, zp);
    };

    inline std::vector<T> get_x_coords() { return x; }

    inline std::vector<T> get_y_coords() { return y; }

    inline std::vector<T> get_z_coords() { return z; }

    inline T get_potential(const unsigned &particle) {

        T V = 0;
        /*
        std::cout << "num_particles: " << num_particles
                  << std::endl;
        std::cout << "get_potential: particle = " << particle
                  << std::endl;
        */
        for (int i = 0; i < particle; i++) {
            // std::cout << "V: " << V << std::endl;
            V += *(potentials + num_particles * i -
                   (i * (i + 1)) / 2 + particle - i - 1);
        }

        // std::cout << std::endl;

        for (int i = particle + 1; i < num_particles; i++) {
            // std::cout << "V: " << V << std::endl;
            V += *(potentials + num_particles * particle -
                   (particle * (particle + 1)) / 2 + i - particle -
                   1);
        }
        return V;
    }

    /**
        @brief The function move particle using Monte Carlo
       algorithm
        @details

        \param
    */
    void move_particle() {

        int particle = random_int(rng);
        // print_potential(particle);
        T distance = step_factor * random_normal_number(rng);
        // T distance = random_normal_number(rng);
        T theta = M_PI * random_real(rng);
        T fi = 2.0 * M_PI * random_real(rng);

        T potential_energy_before = get_potential(particle);
        /*std::cout << "particle: " << particle
                  << "  potential energy: "
                  << potential_energy_before << std::endl;
        */
        T dz = distance * std::cos(theta);
        T sintheta = std::sin(theta);
        T dx = distance * std::cos(fi) * sintheta;
        T dy = distance * std::sin(fi) * sintheta;

        T x_new = x[particle] + dx;
        x_new = x_new - std::round(x_new / (box_size)) * box_size;
        // std::cout <<"particle: " << particle << "  x new: " <<
        // x_new << std::endl;

        T y_new = y[particle] + dy;
        y_new = y_new - std::round(y_new / (box_size)) * box_size;

        T z_new = z[particle] + dz;
        z_new = z_new - std::round(z_new / (box_size)) * box_size;

        T potential_energy_after =
            new_potential_energy(particle, x_new, y_new, z_new);

        T potential_energy_change =
            potential_energy_after - potential_energy_before;

        if (potential_energy_change < (T)0.0) {

            x[particle] = x_new;
            y[particle] = y_new;
            z[particle] = z_new;
            // std::cout<<"move accepted x[particle]: " <<
            // x[particle] << std::endl;
            update_arrays(particle);
            energy += potential_energy_change;
            total_accepted++;

        } else if (std::exp(-potential_energy_change /
                            temperature) > random_real(rng)) {
            /*std::cout<<"energy change:
            "<<potential_energy_change<<"energy:
            "<<this->get_energy()<<std::endl; std::cout<<"boxsize:
            "<<box_size<<std::endl; std::cout<<"new coords:
            ("<<x_new<<", "<<y_new<<", "<<z_new<<")\n"<<std::endl;
            */
            x[particle] = x_new;
            y[particle] = y_new;
            z[particle] = z_new;
            // std::cout<<"move accepted x[particle]: " <<
            // x[particle] << std::endl;
            update_arrays(particle);
            energy += potential_energy_change;
            total_accepted++;

        } else {
            total_rejected++;
        }
    }

    inline void print_particle_position(
        const unsigned &particle,
        const std::string &text = std::string()) {
        std::cout << text << x[particle] << " " << y[particle]
                  << " " << z[particle] << std::endl;
    }

    void set_density(const T &density_) {
        density = density_;
        box_size = std::cbrt(num_particles / density_);
    }

    void set_temperature(const T &temperature_) {
        this->temperature = temperature_;
    }

    inline T get_energy() { return energy; }

    inline T get_acceptance_ratio() { return acceptance_ratio; }

    inline T get_step_factor() { return step_factor; }

    inline T get_total_acceptance_ratio() {
        return (double)total_accepted /
               (double)(total_accepted + total_rejected);
    }

    inline T get_virial_pressure() {
        T virial_pressure = 0;
        for (int k = 0;
             k < (num_particles * (num_particles - 1)) / 2; k++) {
            virial_pressure +=
                std::sqrt(distances2[k]) * potential_grad[k];
        }
        return virial_pressure;
    }

    inline int get_num_accepted() { return accepted; }

    inline void print_potential(const unsigned &particle) {
        std::cout << "potential: " << std::endl;
        for (int i = 0; i < particle; i++) {
            std::cout << *(potentials + num_particles * i -
                           (i * (i + 1)) / 2 + particle - i - 1)
                      << " ";
        }
        for (int i = particle + 1; i < num_particles; i++) {
            std::cout << *(potentials + num_particles * particle -
                           (particle * (particle + 1)) / 2 + i -
                           particle - 1)
                      << " " << std::endl;
        }
        std::cout << "\n\n" << std::endl;
    };

    void test_period() {

        if (accepted + rejected == 1000) {

            // std::cout <<"step factor: "<<step_factor<<"  "<<
            // (double)accepted/(double)(accepted+rejected)  <<
            // std::endl;

            accepted += 1;
            rejected += 1;

            acceptance_ratio =
                (double)accepted / (double)(rejected);

            step_factor = 1.1 * step_factor * (double)accepted /
                          (double)rejected;
            // std::cout<<"step factor: "<<step_factor<<std::endl;
            if (step_factor > box_size)
                step_factor = box_size;

            accepted = 0;
            rejected = 0;
        }

        int particle = random_int(rng);
        // print_potential(particle);
        T distance = step_factor * random_normal_number(rng);
        T theta = M_PI * random_real(rng);
        T fi = 2.0 * M_PI * random_real(rng);

        T potential_energy_before = get_potential(particle);

        T dz = distance * std::cos(theta);
        T sintheta = std::sin(theta);
        T dx = distance * std::cos(fi) * sintheta;
        T dy = distance * std::sin(fi) * sintheta;

        T x_new = x[particle] + dx;
        x_new = x_new - std::round(x_new / (box_size)) * box_size;

        T y_new = y[particle] + dy;
        y_new = y_new - std::round(y_new / (box_size)) * box_size;

        T z_new = z[particle] + dz;
        z_new = z_new - std::round(z_new / (box_size)) * box_size;

        T potential_energy_after =
            new_potential_energy(particle, x_new, y_new, z_new);

        T potential_energy_change =
            potential_energy_after - potential_energy_before;

        if (potential_energy_change < (T)0.0) {

            x[particle] = x_new;
            y[particle] = y_new;
            z[particle] = z_new;
            update_arrays(particle);
            energy += potential_energy_change;
            accepted++;
        } else if (std::exp(-potential_energy_change /
                            temperature) > random_real(rng)) {
            x[particle] = x_new;
            y[particle] = y_new;
            z[particle] = z_new;
            update_arrays(particle);
            energy += potential_energy_change;
            accepted++;
        } else {
            rejected++;
        }
    }
};
