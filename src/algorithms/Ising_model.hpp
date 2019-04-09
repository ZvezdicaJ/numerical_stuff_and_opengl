#pragma once
#include <random>
#include <iostream>
#include <functional>
#include <ctime>

/**
 * @class spin_dir
 * @brief This enum contains two possible spin directions in the Ising model
 */
enum spin_dir : int { UP = 1, DOWN = -1 };

/**
 * @class IsingModel
 * @brief Ising model containing two possible spin flipping methods - metropolis
 * and wolff.
 */
template <typename T>
class IsingModel {
    static_assert(std::is_same<T, float>::value ||
                      std::is_same<T, double>::value,
                  "wolff_algorithm can either be float of double type");

  private:
    unsigned size =
        302; /**<size of spin array, spin array is always square size*size */
    T J = 1; /**< coupling constant between spins*/
    T H = 0; /**< external field strength */
    T tc = 2.269185; /**< critical temperature of Ising model */
    T temperature =
        2; /**< real temperature of spin array, default value is 2. */
    spin_dir *spin_array;      /**< pointer to spin array */
    unsigned cluster_size = 0; /**< variable monitoring size of wolff cluster*/
    int magnetization = 0;     /**< magnetization of spin array */
    T energy = 0;              /**< energy of spin array  */
    unsigned samples = 0;

    // std::default_random_engine generator;

    std::random_device rd;
    std::mt19937 rng;
    std::uniform_int_distribution<int> random_int;
    std::uniform_real_distribution<T> random_real;

    /**
     * @brief enforce periodic boundary conditions
     */
    void enforce_boundary_conditions() {
        for (unsigned j = 0; j < size; j++) {
            *(spin_array + j) = *(spin_array + size * (size - 2) + j);
            *(spin_array + (size - 1) * size + j) = *(spin_array + size + j);

            *(spin_array + j * size) = *(spin_array + j * size + size - 2);
            *(spin_array + j * size + size - 1) = *(spin_array + j * size + 1);
        }
    }

    /**
     * @brief set random spin directions
     */
    void set_random_spin_directions() {
        int spin_up = 0;
        int spin_down = 0;
        for (unsigned i = 0; i < size; i++) {
            for (unsigned j = 0; j < size; j++) {
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
        energy = calc_energy();
        magnetization = calc_magnetization();
    }

    /**
     *@brief Make a Wolff algorithm step. This is a helper for flip_cluster()
     *function.
     *@details Choose a random spin and flip it. Then check it's neighbours. If
     *they have the same spin turn them with the probability of exp^(-2J/T).
     *@param spin Direction of the chosen spin
     *@param k index indicating position in spin array
     *@param l index indicating position in spin array
     */
    void wolff_cluster_step(spin_dir spin, unsigned k, unsigned l) {
        spin_dir *p = spin_array;
        T dE =
            2 * J * (T)(*(p + k * size + l)) *
                (T)((*(p + (k - 1) * size + l)) + (*(p + (k + 1) * size + l)) +
                    (*(p + k * size + l + 1)) + (*(p + k * size + l - 1))) +
            2 * H * (T)(*(p + k * size + l));

        *(spin_array + k * size + l) =
            (spin_dir)(-*(spin_array + k * size + l));
        // energy += dE;
        cluster_size += 1;

        if ((*(p + k * size + l)) == UP) {
            magnetization += 2;

        } else {
            magnetization -= 2;
        };

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
#ifdef __gl_h_
    unsigned VBO;
#endif

    /** @brief Basic constructor for IsingModel class.
     *  @details By default it sets the temperature to 2 and size of spin array
     * to 50. It also initializes random generator, sets random spin directions
     * and enforces periodic boundary conditions for the start of calculation
     */
    IsingModel(unsigned size_ = 50, T temperature_ = 2)
        : size(size_), rng(rd()), temperature(temperature_),
          random_int(std::uniform_int_distribution<int>(1, size_ - 2)),
          random_real(std::uniform_real_distribution<T>(0, 1)) {
        spin_array = (spin_dir *)malloc(sizeof(spin_dir) * size_ * size_);
        rng.seed(::time(NULL));
        set_random_spin_directions();
        enforce_boundary_conditions();
#ifdef __gl_h_
        initialize_VBO();
#endif
    };

    /** @brief
     *
     **/
    spin_dir *get_spin_array() { return spin_array; };

    /**
     * @brief calculate magnetization - difference between spins up and spins
     * down
     */
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

    /**
     * @brief sets the temperature
     */
    void set_temperature(T temperature_) { temperature = temperature_; }

    /** @brief Get the size of the cluster in the last Wolff step.
     */
    unsigned get_cluster_size() { return cluster_size; }

    /** @brief Get current energy.
     */
    T get_energy() { return energy; }

    /** @brief get current magnetization.
     */
    int get_magnetization() { return magnetization; }

    /** @brief The function flips the cluster using Wolff algorithm.
     * @details It calls the wolff_cluster_step() function.
     */
    void flip_cluster() {
        // randomly choose k and l
        unsigned k = random_int(rng);
        unsigned l = random_int(rng);
        cluster_size = 0;
        // std::cout << "(k, l) = " << k << " " << l << std::endl;
        spin_dir spin = *(spin_array + k + size + l);
        wolff_cluster_step(spin, k, l);
        enforce_boundary_conditions();

        energy = calc_energy();
        magnetization = calc_magnetization();
    }

    /**
     * @brief set all spins up.
     */
    void set_all_spins_up() {
        int spin_up = 0;

        for (unsigned i = 0; i < size; i++) {
            for (unsigned j = 0; j < size; j++) {
                *(spin_array + i * size + j) = UP;
            }
        }
        enforce_boundary_conditions();
        energy = calc_energy();
        magnetization = calc_magnetization();
    }

    /**@brief The function calculates energy of the spin array.
     */
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

    /**@brief Makes a metropolis step.
     * @details a random spin is chosen and flipped with the probability
     * exp(-dE/T)
     * @param spin_flips number of metropolis spin flips to make
     */
    void metropolis_steps(unsigned spin_flips) {
        spin_dir *p = spin_array;
        T dE;
        for (unsigned i = 0; i < spin_flips; i++) {
            //  std::cout << "metropolis: magnetization: "
            //          << this->calc_magnetization() << std::endl;
            unsigned k = random_int(rng);
            unsigned l = random_int(rng);

            dE =
                2 * J * (T)(*(p + k * size + l)) *
                    (T)((*(p + (k - 1) * size + l)) +
                        (*(p + (k + 1) * size + l)) +
                        (*(p + k * size + l + 1)) + (*(p + k * size + l - 1))) +
                2 * H * (T)(*(p + k * size + l));

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
            }

            if (dE > 0 && pow(2.71828182845904523536, -dE / temperature) >
                              random_real(rng)) {
                if ((*(p + k * size + l)) == DOWN) {
                    (*(p + k * size + l)) = UP;
                    magnetization += 2;
                } else {
                    (*(p + k * size + l)) = DOWN;
                    magnetization -= 2;
                }
                energy += dE;
            }
            // enforcing boundary conditions
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

#ifdef __gl_h_
    /**
     * @brief Allocates and initializes vertex buffer object, element buffer
     * object and vertex array object. It also allocates color buffer - where
     * color for each vertex is stored.
     */
    void initialize_VBO() {
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(spin_dir) * size * size,
                     (void *)spin_array, GL_DYNAMIC_DRAW);
    }
#endif
};
