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
    T temperature = 2;
    spin_dir *spin_array;
    unsigned cluster_size = 0;
    int magnetization = 0;
    int magnetization2 = 0;
    T energy = 0;
    T energy2 = 0;
    int average_magnetization = 0;
    int average_magnetization2 = 0;
    T average_energy = 0;
    T average_energy2 = 0;
    unsigned samples = 0;

    // std::default_random_engine generator;

    std::random_device rd;
    std::mt19937 rng;
    std::uniform_int_distribution<int> random_int;
    std::uniform_real_distribution<T> random_real;

    void enforce_boundary_conditions() {
        for (unsigned j = 0; j < size; j++) {
            *(spin_array + j) = *(spin_array + size * (size - 2) + j);
            *(spin_array + (size - 1) * size + j) = *(spin_array + size + j);

            *(spin_array + j * size) = *(spin_array + j * size + size - 2);
            *(spin_array + j * size + size - 1) = *(spin_array + j * size + 1);
        }
    }

    void set_random_spin_directions() {
        int spin_up = 0;
        int spin_down = 0;
        for (unsigned i = 1; i < size - 1; i++) {
            for (unsigned j = 1; j < size - 1; j++) {
                // h[i][j]=1;
                // std::cout << "random: " << random_real(rng) << std::endl;
                if (random_real(rng) >= 0.5) {
                    *(spin_array + i * size + j) = DOWN;
                    spin_down++;
                } else {
                    *(spin_array + i * size + j) = UP;
                    spin_up++;
                }
            }
        }
        // std::cout << "random spin up: " << spin_up << std::endl;
        // std::cout << "random spin down: " << spin_down << std::endl;
    }

    void wolff_cluster_step(spin_dir spin, unsigned k, unsigned l) {

        *(spin_array + k * size + l) =
            (spin_dir)(-*(spin_array + k * size + l));
        cluster_size += 1;

        if (k != size - 2 && *(spin_array + (k + 1) * size + l) == spin &&
            std::exp(-2 * J / temperature) < random_real(rng)) {
            wolff_cluster_step(spin, k + 1, l);
        }
        if (k != 1 && *(spin_array + (k - 1) * size + l) == spin &&
            std ::exp(-2 * J / temperature) < random_real(rng)) {
            wolff_cluster_step(spin, k - 1, l);
        }

        if (l != size - 2 && *(spin_array + k * size + l + 1) == spin &&
            std::exp(-2 * J / temperature) < random_real(rng)) {
            wolff_cluster_step(spin, k, l + 1);
        }

        if (l != 1 && *(spin_array + k * size + l - 1) == spin &&
            std::exp(-2 * J / temperature) < random_real(rng)) {
            wolff_cluster_step(spin, k, l - 1);
        }
    }

  public:
    IsingModel(unsigned size_ = 50, T temperature_ = 2)
        : size(size_), rng(rd()), temperature(temperature_),
          random_int(std::uniform_int_distribution<int>(1, size_ - 2)),
          random_real(std::uniform_real_distribution<T>(0, 1)) {
        spin_array = (spin_dir *)malloc(sizeof(spin_dir) * size_ * size_);
        rng.seed(::time(NULL));
        set_random_spin_directions();
        enforce_boundary_conditions();
    };

    int calc_magnetization() {
        int s = 0;
        for (unsigned i = 1; i < size - 1; i++) {
            for (unsigned j = 1; j < size - 1; j++) {
                s += (int)*(spin_array + i * size + j);
            }
        }
        magnetization = s;
        return s;
    }

    void set_temperature(T temperature_) { temperature = temperature_; }
    unsigned get_cluster_size() { return cluster_size; }
    void flip_cluster() {
        // randomly choose k and l
        unsigned k = random_int(rng);
        unsigned l = random_int(rng);
        cluster_size = 0;
        // std::cout << "(k, l) = " << k << " " << l << std::endl;
        spin_dir spin = *(spin_array + k + size + l);
        wolff_cluster_step(spin, k, l);
    }

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

    void metropolis_steps(unsigned spin_flips) {
        spin_dir *p = spin_array;
        T dE;
        for (unsigned i = 0; i < spin_flips; i++) {
            std::cout << "metropolis: magnetization: "
                      << this->calc_magnetization() << std::endl;
            unsigned k = random_int(rng);
            unsigned l = random_int(rng);

            dE =
                2 * J * (T)(*(p + k * size + l)) *
                    (T)((*(p + (k - 1) * size + l)) +
                        (*(p + (k + 1) * size + l)) +
                        (*(p + k * size + l + 1)) + (*(p + k * size + l - 1))) +
                2 * H * (T)(*(p + k * size + l));

            // printf("\n %d   dE = %.30f sosedi: %d %d %d %d   ",h[k][l],dE,
            // h[k-1][l],h[k+1][l],h[k][l-1],h[k][l+1]    );
            if (dE == 0.0) {
                (*(p + k * size + l)) = (spin_dir)(-(*(p + k * size + l)));
                if ((*(p + k * size + l)) == 1)
                    magnetization += 2;
                if ((*(p + k * size + l)) == -1)
                    magnetization -= 2;
            };

            if (dE < 0) {
                if ((*(p + k * size + l)) == DOWN) {
                    (*(p + k * size + l)) = UP;
                    magnetization += 2;
                    // printf("spin se poveca !!    ");
                } else {
                    (*(p + k * size + l)) = DOWN;
                    magnetization -= 2;
                    // printf("spin se zmanjsa!!    ");
                };
                energy += dE;
                // printf("   magnetizacija=%f    ", magnetiz);
            }
            // printf("\n%f   %f   %f",dE ,E0, energija(&h[0][0]));

            if (dE > 0 && pow(2.71828182845904523536, -dE / temperature) >
                              random_real(rng)) {
                // printf("   vzbuditev    ");
                if ((*(p + k * size + l)) == DOWN) {
                    (*(p + k * size + l)) = UP;
                    magnetization += 2;
                    // printf("\spin se poveca!!\n");
                } else {
                    (*(p + k * size + l)) = DOWN;
                    magnetization -= 2;
                    // printf("\nspin se zmanjsa!!\n");
                }
                energy += dE;
            }

            // printf("   magnetizacija=%f    ", magnetiz);

            if (k == 1 || k == size - 2 || l == 1 || l == size - 2) {

                for (unsigned j = 0; j < size; j++) {
                    (*(p + j)) = (spin_dir) * (p + size * (size - 2) + j);
                    *(p + size * (size - 1) + j) = (spin_dir) * (p + size + j);

                    *(p + size * j) = (spin_dir) * (p + size * j + size - 2);
                    *(p + size * j + (size - 1)) =
                        (spin_dir) * (p + size * j + 1);
                }
            }
        }
    }
};
