#pragma once
#include <random>
#include <iostream>
#include <functional>
#include <ctime>

enum spin_dir : int { UP = 1, DOWN = -1 };

template <typename T>
class IsingModel {
    static_assert(std::is_same<T, float>::value ||
                      std::is_same<T, double>::value,
                  "wolff_algorithm can either be float of double type");

  private:
    unsigned size = 302;
    T J = 1;
    T H = 0;
    T tc = 2.269185;
    T temperature;
    spin_dir *spin_array;

    int magnetization = 0;
    int magnetization2 = 0;
    T energy = 0;
    T energy2 = 0;
    int average_magnetization = 0;
    int average_magnetization2 = 0;
    T average_energy = 0;
    T average_energy2 = 0;
    unsigned samples = 0;

     static std::mt19937_64 generator;
    //std::default_random_engine generator;

    static std::uniform_real_distribution<T> real_dist;
    static decltype(std::bind(real_dist, generator)) real_generator;

    std::uniform_int_distribution<int> int_dist;
    decltype(std::bind(int_dist, generator)) int_generator;

    void enforce_boundary_conditions() {
        for (unsigned j = 0; j < size; j++) {
            *(spin_array + j) = *(spin_array + size * (size - 2) + j);
            *(spin_array + (size - 1) * size + j) = *(spin_array + size + j);

            *(spin_array + j * size) = *(spin_array + j * size + size - 2);
            *(spin_array + j * size + size - 1) = *(spin_array + j * size + 1);
        }
    }

    void set_random_spin_directions() {
        for (unsigned i = 0; i < size; i++) {
            for (unsigned j = 0; j < size; j++) {
                // h[i][j]=1;
                if (real_generator() >= 0.5) {
                    *(spin_array + i * size + j) = DOWN;
                } else {
                    *(spin_array + i * size + j) = UP;
                }
            }
        }
    }

    void wolff_cluster_step(int spin, int k, int l) {

        *(spin_array + k * size + l) = -*(spin_array + k * size + l);

        if (k != size - 2 && *(spin_array + (k + 1) * size + l) == spin &&
            std::exp(-2 * J / temperature) < real_generator()) {
            wolff_cluster_step(spin, k + 1, l);
        }
        if (k != 1 && *(spin_array + (k - 1) * size + l) == spin &&
            std ::exp(-2 * J / temperature) < real_generator()) {
            wolff_cluster_step(spin, k - 1, l);
        }

        if (l != size - 2 && *(spin_array + k * size + l + 1) == spin &&
            std::exp(-2 * J / temperature) < real_generator()) {
            wolff_cluster_step(spin, k, l + 1);
        }

        if (l != 1 && *(spin_array + k * size + l - 1) == spin &&
            std::exp(-2 * J / temperature) < real_generator()) {
            wolff_cluster_step(spin, k, l - 1);
        }
    }

    void flip_cluster() {
        // randomly choose k and l
        int k = int_generator();
        int l = int_generator();
        spin_dir spin = *(spin_array + k + size + l);
        wolff_cluster_step(spin, k, l);
    }

  public:
    int calc_magnetization() {
        int s = 0;
        for (unsigned i = 1; i < size - 1; i++) {
            for (unsigned j = 1; j < size - 1; j++) {
                s += *(spin_array + i * size + j);
            }
        }
        magnetization = s;
        return s;
    }

    IsingModel(unsigned size_ = 50) : size(size_) {
        spin_array = (spin_dir *)malloc(sizeof(spin_dir) * size_ * size_);

        static std::uniform_real_distribution<T> real_dist =
            std::uniform_real_distribution<T>(0.0, 1.0);
        static decltype(std::bind(real_dist, generator)) real_generator;

        static std::uniform_int_distribution<int> int_dist =
            std::uniform_int_distribution<int>(1, size - 2);
        static decltype(std::bind(int_dist, generator)) int_generator;

        set_random_spin_directions();
        enforce_boundary_conditions();
    };

    T calc_energy() {
        T w = 0;
        for (unsigned i = 1; i < size - 1; i++) {
            for (unsigned j = 1; j < size - 1; j++) {
                spin_dir *p1 = spin_array + i * size + j; // spin_array[i][j]
                spin_dir *p2 = spin_array + (i - 1) * size + j;
                spin_dir *p3 = spin_array + (i + 1) * size + j;
                w += -J * (T)((*p1) * (*p2 + *p3 + (*(p1 - 1)) + (*(p1 + 1))));
            }
        }
        energy = w / 2.0;
        return energy;
    }

    void calculate_properties() {
        magnetization = (T)calc_magnetization();
        magnetization2 = magnetization * magnetization;
        energy = calc_energy() - H * (T)magnetization;
        energy2 = energy * energy;
        average_magnetization += magnetization;
        average_magnetization2 += magnetization2;
        average_energy += energy;
        average_energy2 += energy2;
        samples++;
    }
};
