#pragma once
#include <random>
#include <iostream>
#include <functional>
#include <ctime>
#include <cstdlib>

/**
 * @class spin_dir
 * @brief This enum contains two possible spin directions in the
 * Ising model
 */
enum spin_dir : int { UP = 1, DOWN = -1 };

/**
 * @class IsingModel
 * @brief Ising model containing two possible spin flipping methods
 * - metropolis and wolff.
 */
template <typename T>
class IsingModel {
    static_assert(
        std::is_same<T, float>::value ||
            std::is_same<T, double>::value,
        "wolff_algorithm can either be float of double type");

  private:
    const unsigned size; /**<size of spin array, spin array is
                            always square size*size */
    T J = 1;             /**< coupling constant between spins*/
    T H = 0;             /**< external field strength */
    T tc = 2.269185;     /**< critical temperature of Ising model */
    T temperature = 2; /**< real temperature of spin array, default
                          value is 2. */
    spin_dir *spin_array; /**< pointer to spin array */
    unsigned cluster_size =
        0; /**< variable monitoring size of wolff cluster*/
    int magnetization = 0; /**< magnetization of spin array */
    T energy = 0;          /**< energy of spin array  */
    unsigned samples = 0;
    bool *visited_spins; /**< used in recursive wolff algorithm */

    std::random_device rd;
    std::mt19937 rng;
    std::uniform_int_distribution<int> random_int;
    std::uniform_real_distribution<T> random_real;

    /**
     * @brief enforce periodic boundary conditions
     */
    void enforce_boundary_conditions() {
        for (unsigned j = 0; j < size; j++) {
            *(spin_array + j) =
                *(spin_array + size * (size - 2) + j);
            *(spin_array + (size - 1) * size + j) =
                *(spin_array + size + j);

            *(spin_array + j * size) =
                *(spin_array + j * size + size - 2);
            *(spin_array + j * size + size - 1) =
                *(spin_array + j * size + 1);
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
                // std::cout << "random: " << random_real(rng) <<
                // std::endl;
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
     *@brief Make a Wolff algorithm step. This is a helper for
     *flip_cluster() function.
     *@details Choose a random spin and flip it. Then check it's
     *neighbours. If they have the same spin turn them with the
     *probability of exp^(-2J/T). The function the recursively
     *checks neighbours.
     *@param spin Direction of the chosen spin
     *@param k index indicating position in spin array
     *@param l index indicating position in spin array
     */
    void wolff_cluster_step(spin_dir spin, unsigned k, unsigned l) {
        spin_dir *p = spin_array;
        T dE = 2 * J * (T)(*(p + k * size + l)) *
                   (T)((*(p + (k - 1) * size + l)) +
                       (*(p + (k + 1) * size + l)) +
                       (*(p + k * size + l + 1)) +
                       (*(p + k * size + l - 1))) +
               2 * H * (T)(*(p + k * size + l));

        *(spin_array + k * size + l) =
            (spin_dir)(-*(spin_array + k * size + l));
        // energy += dE;
        cluster_size += 1;
        // set this spin as visited
        *(visited_spins + k * size + l) = true;

        int dm = 0;
        if ((*(p + k * size + l)) == UP) {
            magnetization += 2;
            dm = 2;

        } else {
            magnetization -= 2;
            dm = -2;
        };

        if (k != size - 2 &&
            *(spin_array + (k + 1) * size + l) == spin &&
            *(visited_spins + (k + 1) * size + l) == false &&
            std::exp(-2 * J / temperature) < random_real(rng)) {
            wolff_cluster_step(spin, k + 1, l);
            *(visited_spins + (k + 1) * size + l) = true;
            magnetization += dm;
        }

        if (k != 1 && *(spin_array + (k - 1) * size + l) == spin &&
            *(visited_spins + (k - 1) * size + l) == false &&
            std ::exp(-2 * J / temperature) < random_real(rng)) {
            wolff_cluster_step(spin, k - 1, l);
            *(visited_spins + (k - 1) * size + l) = true;
            magnetization += dm;
        }

        if (l != size - 2 &&
            *(spin_array + k * size + l + 1) == spin &&
            *(visited_spins + k * size + l + 1) == false &&
            std::exp(-2 * J / temperature) < random_real(rng)) {
            wolff_cluster_step(spin, k, l + 1);
            *(visited_spins + k * size + l + 1) = true;
            magnetization += dm;
        }

        if (l != 1 && *(spin_array + k * size + l - 1) == spin &&
            *(visited_spins + k * size + l - 1) == false &&
            std::exp(-2 * J / temperature) < random_real(rng)) {
            wolff_cluster_step(spin, k, l - 1);
            *(visited_spins + k * size + l - 1) = true;
            magnetization += dm;
        }
    }

    /** @brief The function calculates the energy change of spin
     *  flip.
     *  @param k row of spin
     *  @param l column of spin
     */
    inline T spin_flip_energy_change(const unsigned &k,
                                     const unsigned &l) {
        spin_dir *p = spin_array;
        T dE = 2 * J * (T)(*(p + k * size + l)) *
                   (T)((*(p + (k - 1) * size + l)) +
                       (*(p + (k + 1) * size + l)) +
                       (*(p + k * size + l + 1)) +
                       (*(p + k * size + l - 1))) +
               2 * H * (T)(*(p + k * size + l));
        return dE;
    }

    inline void visited_spins_false() {
#ifdef __AVX2__
        __m256i zeros = _mm256_set1_epi8(0);
        bool *p = visited_spins;
        unsigned reminder = (size * size) % 32;
        for (int i = 0; i <= size * size - reminder - 31; i += 32) {
            _mm256_stream_si256((__m256i *)(p + i), zeros);
        }
#endif

#ifndef __AVX2__
        __m128i zeros = _mm_set1_epi8(0);
        bool *p = &visited_spins;
        unsigned reminder = (size * size) % 16;
        for (int i = 0; i < size * size - reminder - 15; i += 16) {
            _mm_stream_si128((__m128i *)(p + i), zeros);
        }

#endif
        for (int i = size * size - reminder; i < size * size; i++) {
            *(p + i) = false;
        }
    }

    /**
     *@brief Make a Wolff algorithm step. This is a helper for
     *flip_cluster() function.
     *@details Choose a random spin and flip it. Then check it's
     *neighbours. If they have the same spin turn them with the
     *probability of exp^(-2J/T). This is nonrecursive version.
     *@param spin Direction of the chosen spin
     *@param k index indicating position in spin array
     *@param l index indicating position in spin array
     */
    void wolff_cluster_step_nonrecursive(const spin_dir &spin,
                                         const unsigned &k,
                                         const unsigned &l) {

        *(spin_array + (k)*size + l) =
            (spin_dir)(-*(spin_array + (k)*size + l));

        energy += spin_flip_energy_change(k, l);
        int dm = 0;
        if (spin == DOWN) {
            dm = 2;
            magnetization += 2;

        } else {
            dm = -2;
            magnetization -= 2;
        };

        cluster_size = 1;
        bool visited[size][size] = {false};
        std::vector<std::array<unsigned, 2>> flipped;
        flipped.push_back(std::array<unsigned, 2>({k, l}));
        visited[k][l] = true;

        for (int i = 0; i < flipped.size(); i++) {
            std::array<unsigned, 2> central_spin = flipped[i];
            unsigned kt = flipped[i][0];
            unsigned lt = flipped[i][1];

            if (kt != size - 2 &&
                *(spin_array + (kt + 1) * size + lt) == spin &&
                visited[kt + 1][lt] == false &&
                std::exp(-2 * J / temperature) < random_real(rng)) {
                // flip it
                *(spin_array + (kt + 1) * size + lt) = (spin_dir)(
                    -*(spin_array + (kt + 1) * size + lt));
                // set visited to true
                visited[kt + 1][lt] == true;
                // add spin to the list of flipped spins so that
                // it's neighbout will also be checked
                flipped.push_back(
                    std::array<unsigned, 2>({kt + 1, lt}));
                // the last step is to change energy and
                // magnetization
                energy += spin_flip_energy_change(kt + 1, lt);
                cluster_size++;
                magnetization += dm;
            }
            if (kt != 1 &&
                *(spin_array + (kt - 1) * size + lt) == spin &&
                visited[kt - 1][lt] == false &&
                std ::exp(-2 * J / temperature) <
                    random_real(rng)) {

                *(spin_array + (kt - 1) * size + lt) = (spin_dir)(
                    -*(spin_array + (kt - 1) * size + lt));
                visited[kt - 1][lt] == true;
                flipped.push_back(
                    std::array<unsigned, 2>({kt - 1, lt}));
                energy += spin_flip_energy_change(kt - 1, lt);
                // flip it
                cluster_size++;
                magnetization += dm;
            }

            if (lt != size - 2 &&
                *(spin_array + kt * size + lt + 1) == spin &&
                visited[kt][lt + 1] == false &&
                std::exp(-2 * J / temperature) < random_real(rng)) {
                // flip it
                *(spin_array + kt * size + lt + 1) =
                    (spin_dir)(-*(spin_array + kt * size + lt + 1));
                visited[kt][lt + 1] == true;
                flipped.push_back(
                    std::array<unsigned, 2>({kt, lt + 1}));
                energy += spin_flip_energy_change(kt, lt + 1);
                cluster_size++;
                magnetization += dm;
            }

            if (lt != 1 &&
                *(spin_array + kt * size + lt - 1) == spin &&
                visited[kt][lt - 1] == false &&
                std::exp(-2 * J / temperature) < random_real(rng)) {
                // flip it
                *(spin_array + kt * size + lt - 1) =
                    (spin_dir)(-*(spin_array + kt * size + lt - 1));
                visited[kt][lt - 1] == true;
                flipped.push_back(
                    std::array<unsigned, 2>({kt, lt - 1}));
                energy += spin_flip_energy_change(kt, lt - 1);
                cluster_size++;
                magnetization += dm;
            }
        }
    };

    /**
     *@brief Make a Wolff algorithm step. This is a helper for
     *flip_cluster() function.
     *@details Choose a random spin and flip it. Then check it's
     *neighbours. If they have the same spin turn them with the
     *probability of exp^(-2J/T). This is nonrecursive version.
     *@param spin Direction of the chosen spin
     *@param k index indicating position in spin array
     *@param l index indicating position in spin array
     */
    void
    wolff_cluster_step_nonrecursive_vectorized(const spin_dir &spin,
                                               const unsigned &k,
                                               const unsigned &l) {

        *(spin_array + (k)*size + l) =
            (spin_dir)(-*(spin_array + (k)*size + l));

        energy += spin_flip_energy_change(k, l);
        int dm = 0;
        if (spin == DOWN) {
            dm = 2;
            magnetization += 2;

        } else {
            dm = -2;
            magnetization -= 2;
        };

        cluster_size = 1;
        // at the start all spins except spin (k,l) have not yet
        // been checked
        int not_checked[size][size] = {1};
        not_checked[k][l] = 0;
        // flipped
        std::vector<std::array<unsigned, 2>> flipped;
        flipped.push_back(std::array<unsigned, 2>({k, l}));

        __m128i index_pattern = _mm_setr_epi32(1, -1, size, -size);
        __m128i detection_addition = _mm_setr_epi32(1, 0, 0, 0);
        __m128i modulo_numbers =
            _mm_setr_epi32(size, size, size * (size - 1), 1);
        __m128i lower_numbers = _mm_setr_epi32(0, 0, size, size);
        /*        __m128i higher_numbers =
            _mm_setr_epi32(size * size + 1, size * size + 1,
                           size * size + 1, size);
        */
        __m128i spin_direction = _mm_set1_epi32(spin);
        __m128i ones = _mm_set1_epi32(1);

        //        for (int i = 0; i < flipped.size(); i++) {
        for (int i = 0; i < 1; i++) {
            std::array<unsigned, 2> central_spin = flipped[i];

            int central_spin_index =
                central_spin[0] * size + central_spin[1];

            __m128i indices_to_load = _mm_add_epi32(
                _mm_set1_epi32(central_spin_index), index_pattern);
            print_sse(indices_to_load, "indices_to_load: ");
            ////////////////////////////////////////
            // check if any of the spins is on the edge - belogs to
            // boundray conditions
            __m128i modulo = _mm_positive_mod_epi32(
                _mm_add_epi32(indices_to_load, detection_addition),
                modulo_numbers);
            // we need to replace the last modulo with the index
            modulo =
                _mm_blend_epi32(modulo, indices_to_load, 0b1000);
            print_sse(modulo, "modulo: ");
            print_sse(lower_numbers, "lower_numbers: ");
            __m128i comparison1 =
                _mm_cmpgt_epi32(modulo, lower_numbers);

            print_sse(comparison1, "comparison1: ");

            //////////////////////////////////////////
            ///// comparison2 flip spin with probability of
            ///// e^(-2*J/Temperature)
            __m128 exp =
                _mm_set1_ps(std::exp(-2 * J / temperature));
            float random_numbers[4] = {
                random_real(rng), random_real(rng),
                random_real(rng), random_real(rng)};

            __m128 random_vector = _mm_loadu_ps(random_numbers);
            __m128i comparison2 = _mm_castps_si128(
                _mm_cmp_ps(exp, random_vector, _CMP_LT_OS));
            print_sse(comparison2, "comparison2: ");
            //////////////////////////////////////////////////
            /// flip only spins which are turned into the same
            /// direction as the original spin
            __m128i original_spins = _mm_i32gather_epi32(
                (int *)spin_array, indices_to_load, 4);
            // above 4 signifies that each integer is 4 byte long
            print_sse(original_spins, "original_spins: ");

            __m128i comparison3 =
                _mm_cmpeq_epi32(spin_direction, original_spins);
            print_sse(comparison3, "comparison3: ");

            //////////////////////////////////////////////////
            __m128i to_be_flipped = _mm_and_si128(
                _mm_and_si128(comparison1, comparison2),
                comparison3);
            print_sse(to_be_flipped, "to_be_flipped: ");
            //////////////////////////////////////
            ///// calculate the number of spins to be flipped

            __m128i count =
                _mm_shuffle_epi32(to_be_flipped, 0b11100100);
            count = _mm_add_epi32(to_be_flipped, count);
            count = _mm_add_epi32(
                count, _mm_shuffle_epi32(count, 0b01001111));
            int num_spins_to_flip =
                -_mm_extract_epi32(count, 0b00000000);
            std::cout << "number of flipped spins: "
                      << num_spins_to_flip << "\n " << std::endl;

            if (_mm_extract_epi32(to_be_flipped, 0) == 0) {

                flipped.push_back(std::array<unsigned, 2>(
                    {central_spin[0] + 1, central_spin[1]}));
                *(spin_array+central_spin[0]+1+central_spin[])
            }

            /*
            if (kt != size - 2 &&
                *(spin_array + (kt + 1) * size + lt) == spin &&
                visited[kt + 1][lt] == false &&
                std::exp(-2 * J / temperature) < random_real(rng)) {
                // flip it
                *(spin_array + (kt + 1) * size + lt) = (spin_dir)(
                    -*(spin_array + (kt + 1) * size + lt));
                // set visited to true
                visited[kt + 1][lt] == true;
                // add spin to the list of flipped spins so that
                // it's neighbout will also be checked
                flipped.push_back(
                    std::array<unsigned, 2>({kt + 1, lt}));
                // the last step is to change energy and
                // magnetization
                energy += spin_flip_energy_change(kt + 1, lt);
                cluster_size++;
                magnetization += dm;
                }*/
        }
    };

  public:
#ifdef __gl_h_
    unsigned VBO;
#endif

    /** @brief Basic constructor for IsingModel class.
     *  @details By default it sets the temperature to 2 and size of
     *  spin array to 50. It also initializes random generator, sets
     *  random spin directions and enforces periodic boundary
     *  conditions for the start of calculation
     */
    IsingModel(unsigned size_ = 50, T temperature_ = 2)
        : size(size_), rng(rd()), temperature(temperature_),
          random_int(
              std::uniform_int_distribution<int>(1, size_ - 2)),
          random_real(std::uniform_real_distribution<T>(0, 1)) {
        spin_array =
            (spin_dir *)malloc(sizeof(spin_dir) * size_ * size_);
        visited_spins = (bool *)std::aligned_alloc(32, size * size);

        rng.seed(::time(NULL));
        set_random_spin_directions();
        enforce_boundary_conditions();

#ifdef __gl_h_
        initialize_VBO();
#endif
        visited_spins_false();
    };

    IsingModel(IsingModel &&) = delete;
    IsingModel &operator=(IsingModel &&) = delete;
    IsingModel(const IsingModel &) = delete;
    IsingModel &operator=(const IsingModel &) = delete;

    /** @brief
     *
     **/
    spin_dir *get_spin_array() { return spin_array; };

    /**
     * @brief calculate magnetization - difference between spins up
     * and spins down
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
    void set_temperature(T temperature_) {
        temperature = temperature_;
    }

    /**
     * @brief get the temperature
     */
    T get_temperature() { return temperature; }

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
        // wolff_cluster_step_nonrecursive(spin, k, l);
        enforce_boundary_conditions();
        visited_spins_false();
        energy = calc_energy();
    }

    /** @brief The function flips the cluster using Wolff algorithm.
     * @details It calls the wolff_cluster_step() function.
     */
    void flip_cluster_nonrecursive() {
        // randomly choose k and l
        unsigned k = random_int(rng);
        unsigned l = random_int(rng);
        cluster_size = 0;
        // std::cout << "(k, l) = " << k << " " << l << std::endl;
        spin_dir spin = *(spin_array + k + size + l);
        wolff_cluster_step_nonrecursive(spin, k, l);
        enforce_boundary_conditions();
        energy = calc_energy();
    }

    /** @brief The function flips the cluster using Wolff algorithm.
     * @details It calls the wolff_cluster_step() function.
     */
    void flip_cluster_nonrecursive_vectorized() {
        // randomly choose k and l
        unsigned k = random_int(rng);
        unsigned l = random_int(rng);
        cluster_size = 0;
        // std::cout << "(k, l) = " << k << " " << l << std::endl;
        spin_dir spin = *(spin_array + k + size + l);
        wolff_cluster_step_nonrecursive_vectorized(spin, k, l);
        enforce_boundary_conditions();
        energy = calc_energy();
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
                spin_dir *p1 =
                    spin_array + i * size + j; // spin_array[i][j]
                spin_dir *p2 = spin_array + (i - 1) * size + j;
                spin_dir *p3 = spin_array + (i + 1) * size + j;
                w += -J * (T)((*p1) * (*p2 + *p3 + (*(p1 - 1)) +
                                       (*(p1 + 1))));
            }
        }
        energy = w / 2.0;
        return energy;
    }

    /**@brief Makes a metropolis step.
     * @details a random spin is chosen and flipped with the
     * probability exp(-dE/T)
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

            dE = 2 * J * (T)(*(p + k * size + l)) *
                     (T)((*(p + (k - 1) * size + l)) +
                         (*(p + (k + 1) * size + l)) +
                         (*(p + k * size + l + 1)) +
                         (*(p + k * size + l - 1))) +
                 2 * H * (T)(*(p + k * size + l));

            if (dE == 0.0) {
                (*(p + k * size + l)) =
                    (spin_dir)(-(*(p + k * size + l)));
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

            if (dE > 0 &&
                pow(2.71828182845904523536, -dE / temperature) >
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
            if (k == 1 || k == size - 2 || l == 1 ||
                l == size - 2) {

                for (unsigned j = 0; j < size; j++) {
                    (*(p + j)) =
                        (spin_dir) * (p + size * (size - 2) + j);
                    *(p + size * (size - 1) + j) =
                        (spin_dir) * (p + size + j);

                    *(p + size * j) =
                        (spin_dir) * (p + size * j + size - 2);
                    *(p + size * j + (size - 1)) =
                        (spin_dir) * (p + size * j + 1);
                }
            }
        }
    }

#ifdef __gl_h_
    /**
     * @brief Allocates and initializes vertex buffer object,
     * element buffer object and vertex array object. It also
     * allocates color buffer - where color for each vertex is
     * stored.
     */
    void initialize_VBO() {
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(spin_dir) * size * size,
                     (void *)spin_array, GL_DYNAMIC_DRAW);
    }
#endif
};
