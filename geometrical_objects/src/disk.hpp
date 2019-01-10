#ifndef __DISK__

template <typename T = float> class Disk : public Shape3D<T> {
  private:
    void generate_vertexes(int = 10, T = 0.5);

  public:
    Disk();
    Disk(int, T = 0.5);
    Disk(Disk &&) = default;
    Disk &operator=(Disk &&) = default;
    Disk(const Disk &) = default;
    Disk &operator=(const Disk &) = default;
    T perimeter();
};

template <typename T> Disk<T>::Disk() {
    this->draw_type = 'V';
    this->vertex_size = 2;
    this->min_vertexes = 50;
    this->generate_vertexes();
    this->initialize_buffers();
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
    this->vertexes.reserve(3 * num_vertexes + 1); // 3 krat ker je vsak vertex iz treh tock
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
            __m128 cos_vec = cos(fi_vec);
            __m128 sin_vec = sin(fi_vec);

            __m128 xy_tocki12 = _mm_unpackhi_ps(cos_vec, sin_vec);
            xy_tocki12 =
                _mm_shuffle_ps(xy_tocki12, xy_tocki12, _MM_SHUFFLE(1, 0, 3, 2));

            __m128 xy_tocki34 = _mm_unpacklo_ps(cos_vec, sin_vec);
            xy_tocki34 =
                _mm_shuffle_ps(xy_tocki34, xy_tocki34, _MM_SHUFFLE(1, 0, 3, 2));

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
            print_sse(tocka1, "tocka1: ");
            print_sse(tocka2, "tocka2: ");
            print_sse(tocka3, "tocka3: ");
            print_sse(tocka4, "tocka4: ");
        }
        ind = num_upper_vertexes * 3;
    }
}

#define __DISK__
#endif
