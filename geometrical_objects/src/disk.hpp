#ifndef __DISK__

template <typename T = float> class Disk : public Shape2D<T> {
  private:
    void generate_vertexes(int = 10, T = 0.5);

  public:
    Disk();
    Disk(int, T = 0.5);
    Disk(Star &&) = default;
    Disk &operator=(Star &&) = default;
    Disk(const Star &) = default;
    Disk &operator=(const Star &) = default;
    T perimeter();
};

template <typename T> Disk<T>::Disk() {
    this->draw_type = 'V';
    this->vertex_size = 2;
    this->min_vertexes = 50;
    this->generate_vertexes();
    this->initialize_buffers();
    this->generate_filling_ebo();
    // std::cout << "circle buffers:\nVBO: " << this->VBO << "\nVAO: " <<
    // this->VAO << std::endl;
};

template <typename T> Disk<T>::Disk(int bulges, T ratio) {
    this->draw_type = 'V';
    this->vertex_size = 2;
    this->min_vertexes = 50;
    this->generate_vertexes(bulges, ratio);
    this->initialize_buffers();

    // std::cout << "circle buffers:\nVBO: " << this->VBO << "\nVAO: " <<
    // this->VAO << std::endl;
};

template <>
inline void Disk<float>::generate_vertexes(int bulges, float ratio) {
    // this function always generates 4n-1 different vertexes;
    // -1 becase the last point is the same as the first one
    int num_vertexes;
    if ((min_vertexes % 2) == 0)
        num_vertexes = min_vertexes;
    else {
        num_vertexes = min_vertexes;
    }

    this->vertexes.reserve(3 * num_vertexes + 1);
    this->vertexes.resize(3 * num_vertexes);

    float korak = M_PI / (float)((num_vertexes - 2) / 2);
    __m128 radius = _mm_set_ps1(1.0);
    __m128 height = _mm_set_ps1(0.5);
    __m128 cons = _mm_set_ps(0, 1, 2, 3);
    __m128 korak_vec = _mm_set_ps1(korak);

    int num_upper_vertexes = (num_vertexes - 2) / 2;
    int ind = 0;
    for (int j = 0; j < num_upper_vertexes; j++) {
        for (int i = 0; i < num_upper_vertexes; i += 4) {
            __m128 i_vec = _mm_set_ps1(i);
            __m128 fi_vec = _mm_mul_ps(_mm_add_ps(i_vec, cons), korak_vec);
            __m128 cos_vec = _mm_mul_ps(cos(fi_vec), ansatz);
            __m128 sin_vec = _mm_mul_ps(sin(fi_vec), ansatz);

            __m128 xy_tocki12 = _mm_unpackhi_ps(cos_vec, sin_vec);
            tocki12 = _mm_shuffle_ps(tocki12, tocki12, _MM_SHUFFLE(1, 0, 3, 2));

            __m128 xy_tocki34 = _mm_unpacklo_ps(cos_vec, sin_vec);
            tocki34 = _mm_shuffle_ps(tocki34, tocki34, _MM_SHUFFLE(1, 0, 3, 2));

            __m128 tocka1 =
                _mm_shuffle_ps(xy_tocki12, height, _MM_SHUFFLE(0, 0, 1, 0));
            __m128 tocka2 =
                _mm_shuffle_ps(xy_tocki12, height, _MM_SHUFFLE(0, 0, 3, 2));

            __m128 tocka3 =
                _mm_shuffle_ps(xy_tocki34, height, _MM_SHUFFLE(0, 0, 1, 0));
            __m128 tocka4 =
                _mm_shuffle_ps(xy_tocki34, height, _MM_SHUFFLE(0, 0, 3, 2));

            _mm_storeu_ps(&(this->vertexes[ind]) + 3 * i, tocka1);
            _mm_storeu_ps(&(this->vertexes[ind]) + 3 * i + 3, tocka2);
            _mm_storeu_ps(&(this->vertexes[ind]) + 3 * i + 6, tocka3);
            _mm_storeu_ps(&(this->vertexes[ind]) + 3 * i + 9, tocka4);
        }
        ind = num_upper_vertexes * 3;
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
    }
}


#define __DISK__
#endif
