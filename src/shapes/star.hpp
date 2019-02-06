#pragma once
#ifndef __STAR__

/**
 * @class Star
 * @brief 2d star with various number of bulges
 */
template <typename T = float>
class Star : public Shape2D<T> {
  private:
    void generate_vertexes(int = 10, T = 0.5);

  public:
    Star();
    Star(int, T = 0.5);
    Star(Star &&) = default;
    Star &operator=(Star &&) = default;
    Star(const Star &) = default;
    Star &operator=(const Star &) = default;
    T perimeter();
};

/*
 * @brief The constructor generates vertexes, initializes buffers and generates
 * filling for 2d shape
 * @param bulges number of star legs to draw
 * @param ration between star leg length and central part of the star
 */
template <typename T>
Star<T>::Star(int bulges, T ratio) {
    this->draw_type = 'V';
    this->filling_draw_type = 'V';
    this->vertex_size = 2;
    this->min_vertexes = 50;
    this->generate_vertexes(bulges, ratio);
    this->initialize_buffers();
    this->generate_filling_vbo();
};

/**
 * @brief This function generates vertexes for float version of the star. It's
 * using sse instructions - requires appropriate cpu
 *
 */
template <>
inline void Star<float>::generate_vertexes(int bulges, float ratio) {
    // this function always generates 4n-1 different vertexes;
    // -1 becase the last point is the same as the first one

    this->vertexes.reserve(4 * bulges);
    this->vertexes.resize(4 * bulges);
    int tocke = 2 * bulges;
    int r = 4 * ((tocke) / 4); // points should be a multiple of 4
    int reminder = tocke - r;  // stevilo preostalih tock

    float korak = M_PI / (float)bulges;
    __m128 ansatz = _mm_set_ps(ratio, 1.0, ratio, 1.0);
    __m128 cons = _mm_set_ps(0, 1, 2, 3);

    __m128 korak_vec = _mm_set_ps1(korak);
    for (int i = 0; i < r; i += 4) {
        __m128 i_vec = _mm_set_ps1(i);
        __m128 fi_vec = _mm_mul_ps(_mm_add_ps(i_vec, cons), korak_vec);
        __m128 cos_vec = _mm_mul_ps(cos(fi_vec), ansatz);
        __m128 sin_vec = _mm_mul_ps(sin(fi_vec), ansatz);

        __m128 tocki12 = _mm_unpackhi_ps(cos_vec, sin_vec);
        tocki12 = _mm_shuffle_ps(tocki12, tocki12, _MM_SHUFFLE(1, 0, 3, 2));
        __m128 tocki34 = _mm_unpacklo_ps(cos_vec, sin_vec);
        tocki34 = _mm_shuffle_ps(tocki34, tocki34, _MM_SHUFFLE(1, 0, 3, 2));

        _mm_storeu_ps(&(this->vertexes[0]) + 2 * i, tocki12);
        _mm_storeu_ps(&(this->vertexes[0]) + 2 * i + 4, tocki34);
    }

    int starting_indeks = 4 * bulges - 2 - reminder * 2;
    float factor;
    int stevec = 0;
    for (int i = reminder; i > 0; i--) {
        if (!stevec) {
            factor = 1.0;
            stevec = 1;
        } else if (stevec) {
            factor = ratio;
            stevec = 0;
        }
        float angle = 2.0 * M_PI - (reminder + 1 - i) * korak;
        int indeks = starting_indeks + 2 * i;
        (this->vertexes)[indeks] = factor * std::cos(angle);
        (this->vertexes)[indeks + 1] = factor * std::sin(angle);

        // std::cout << "indeks: " << indeks << std::endl;
        // std::cout << "kot: " << angle << " cos:  " << std::cos(angle)
        // << " sin:  " << std::sin(angle) << std::endl;
    }
}

/**
 * @brief This function generates vertexes for double version of the star. It's
 * using avx2 instructions - requires appropriate cpu.
 *
 */
template <>
inline void Star<double>::generate_vertexes(int bulges, double ratio) {
    // this function always generates 4n-1 different vertexes;
    // -1 becase the last point is the same as the first one

    this->vertexes.reserve(4 * bulges);
    this->vertexes.resize(4 * bulges);
    int tocke = 2 * bulges;
    int r = 4 * ((tocke) / 4);
    int reminder = tocke - r; // stevilo preostalih tock

    double korak = M_PI / (double)bulges;
    __m256d ansatz = _mm256_setr_pd(ratio, ratio, 1.0, 1.0);
    __m256d cons = _mm256_setr_pd(0, 2, 3, 1);

    __m256d korak_vec = _mm256_set1_pd(korak);
    for (int i = 0; i < r; i += 4) {
        __m256d i_vec = _mm256_set1_pd(i);
        __m256d fi_vec = _mm256_mul_pd(_mm256_add_pd(i_vec, cons), korak_vec);
        __m256d cos_vec = _mm256_mul_pd(cos(fi_vec), ansatz);
        __m256d sin_vec = _mm256_mul_pd(sin(fi_vec), ansatz);

        __m256d tocki12 = _mm256_shuffle_pd(cos_vec, sin_vec, 0b1100);
        __m256d tocki34 = _mm256_shuffle_pd(cos_vec, sin_vec, 0b0011);

        _mm256_storeu_pd(&(this->vertexes[0]) + 2 * i, tocki12);
        _mm256_storeu_pd(&(this->vertexes[0]) + 2 * i + 4, tocki34);
    }

    int starting_indeks = 4 * bulges - 2 - reminder * 2;
    double factor;
    int stevec = 0;
    for (int i = reminder; i > 0; i--) {
        if (!stevec) {
            factor = 1.0;
            stevec = 1;
        } else if (stevec) {
            factor = ratio;
            stevec = 0;
        }
        double angle = 2.0 * M_PI - (reminder + 1 - i) * korak;
        int indeks = starting_indeks + 2 * i;
        (this->vertexes)[indeks] = factor * std::cos(angle);
        (this->vertexes)[indeks + 1] = factor * std::sin(angle);

        // std::cout << "indeks: " << indeks << std::endl;
        // std::cout << "kot: " << angle << " cos:  " << std::cos(angle)
        // << " sin:  " << std::sin(angle) << std::endl;
    }
}

#define __STAR__
#endif
