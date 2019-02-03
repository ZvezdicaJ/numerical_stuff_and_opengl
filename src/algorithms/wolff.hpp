#pragma once
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <ulimit.h>
#include <random>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

enum spin_dir : int { UP = 1, DOWN = -1 };

template <typename T>
class wolff_algorithm {
    static_assert(std::is_same<T, float>::value ||
                      std::is_same<T, double>::value,
                  "wolff_algorithm can either be float of double type");

  private:
    const int size = 302;
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

    std::mt19937_64 generator;

    std::uniform_real_distribution<T> distribution(0, 1);
    auto real_generator = std::bind(distribution, generator);

    std::uniform_int_distribution<int> distribution_int;
    decltype(std::bind(distribution_int, generator)) int_generator;

    wolff_algorithm(unisgned size_ = 50) : size(size_) {
        spin_array = malloc(sizeof(spin_dir) * size_ * size_);
        distribution_int = std::uniform_int_distribution<int> dist(1, size - 2);
        set_random_spin_directions();
        enforce_boundary_conditions();
    };

    void enforce_boundary_conditions() {
        for (j = 0; j < size; j++) {
            *(spin_array + j) = *(spin_array + size * (size - 2) + j);
            *(spin_array + (size - 1) * size + j) = *(spin_array + size + j);

            *(spin_array + j * size) = *(spin_array + j * size + size - 2);
            *(spin_array + j * size + size - 1) = *(spin_array + j * size + 1);
        }
    }

    void set_random_spin_directions() {
        for (i = 0; i < size; i++) {
            for (j = 0; j < size; j++) {
                // h[i][j]=1;
                if (real_generator() >= 0.5) {
                    *(spin_array + i * size + j) = -1;
                } else {
                    *(spin_array + i * size + j) = 1;
                }
            }
        }
    }

    void spin_flip(int spin, int k, int l) {

        *(spin_array + k * size + l) = -*(spin_array + k * size + l);

        if (k != size - 2 && *(spin_array + (k + 1) * size + l) == spin &&
            std::exp(-2 * J / temperature) < real_generator()) {
            spin_flip(spin, k + 1, l);
        }
        if (k != 1 && *(spin_array + (k - 1) * size + l) == spin &&
            std ::exp(-2 * J / temperature) < real_generator()) {
            spin_flip(spin, k - 1, l);
        }

        if (l != size - 2 && *(spin_array + k * size + l + 1) == spin &&
            std::exp(-2 * J / temperature) < real_generator()) {
            spin_flip(spin, k, l + 1);
        }

        if (l != 1 && *(spin_array + k * size + l - 1) == spin &&
            std::exp(-2 * J / temperature) < real_generator()) {
            spin_flip(spin, k, l - 1);
        }
    }

    void flip_cluster() {
        // randomly choose k and l
        int k = int_generator();
        int l = int_generator();
        spin_dir spin = *(spin_array + k + size + l);
        spin_flip(spin, k, l);
    }

  public:
    int calc_magnetization() {
        int s = 0;
        int i = 0, j = 0;
        for (i = 1; i < size - 1; i++) {
            for (j = 1; j < size - 1; j++) {
                s += *(spin_array + i * size + j);
            }
        }
        magnetization = s;
        return s;
    }

    T calc_energy() {
        T w = 0;
        int i = 1, j = 1;
        for (i = 1; i < size - 1; i++) {
            for (j = 1; j < size - 1; j++) {
                w += -J * (T)(spin_array[i][j] *
                              (spin_array[i - 1][j] + spin_array[i + 1][j] +
                               spin_array[i][j - 1] + spin_array[i][j + 1]));
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
        stevec++;
    }
};
