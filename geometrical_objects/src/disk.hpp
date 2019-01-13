#ifndef __DISK__

template <typename T = float>
class Disk : public Shape3D<T> {
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

template <typename T>
Disk<T>::Disk() {
    this->draw_type = 'V';
    this->vertex_size = 3;
    this->min_vertexes = 20;
    this->generate_vertexes();
    this->initialize_buffers();
};

template <>
inline void Disk<float>::generate_vertexes(int bulges, float ratio) {
    // this function always generates 4n-1 different vertexes;
    // -1 becase the last point is the same as the first one
    int num_vertexes;
    int num_upper_vertexes = (min_vertexes - 2) / 2;
    int reminder4 = num_upper_vertexes % 4;

    if (reminder4 != 0)
        num_upper_vertexes += (4 - reminder4);
    num_vertexes = (num_upper_vertexes + 2) * 2;

    this->vertexes.reserve(3 * num_vertexes + 1);
    // 3 krat ker je vsak vertex iz treh točk
    this->vertexes.resize(3 * num_vertexes);
    this->element_array.reserve(4 * 3 * num_upper_vertexes + 1);
    this->element_array.resize(4 * 3 * num_upper_vertexes);
    std::fill(this->element_array.begin(), this->element_array.end(), 0);

    float korak = 2.0 * M_PI / (float)(num_upper_vertexes);
    __m128 radius = _mm_set_ps1(1.0);
    __m128 heightup = _mm_set_ps1(0.5);
    __m128 heightdown = _mm_set_ps1(-0.5);
    __m128 cons = _mm_set_ps(0, 1, 2, 3);
    __m128 korak_vec = _mm_set_ps1(korak);

    std::cout << "reminder4: " << reminder4 << std::endl;
    std::cout << "num_upper_vertexes: " << num_upper_vertexes << std::endl;
    std::cout << "num_vertexes: " << num_vertexes << std::endl;

    __m128i next_element = _mm_setr_epi32(0, 1, 1, 1);

    // first element forming upper circle
    __m128i elements_high = _mm_setr_epi32(0, 2, 3, 4);
    // first element forming lower circle
    __m128i elements_low = _mm_setr_epi32(1, 6, 7, 8);
    __m128i jump = _mm_setr_epi32(0, 6, 6, 6);

    int second_half_ind = 3 * num_vertexes / 2;
    int ind = 0;

    _mm_storeu_ps(&(this->vertexes[0]), _mm_setr_ps(0, 0, 0.5, 0.5));
    _mm_storeu_ps(&(this->vertexes[second_half_ind - 3]),
                  _mm_setr_ps(0, 0, -0.5, -0.5));
    for (int i = 0; i < num_upper_vertexes; i += 4) {
        __m128 i_vec = _mm_set_ps1(i / 2);
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
            _mm_shuffle_ps(xy_tocki12, heightup, _MM_SHUFFLE(0, 0, 1, 0));
        __m128 tocka2 =
            _mm_shuffle_ps(xy_tocki12, heightup, _MM_SHUFFLE(0, 0, 3, 2));

        __m128 tocka3 =
            _mm_shuffle_ps(xy_tocki34, heightup, _MM_SHUFFLE(0, 0, 1, 0));
        __m128 tocka4 =
            _mm_shuffle_ps(xy_tocki34, heightup, _MM_SHUFFLE(0, 0, 3, 2));

        _mm_storeu_ps(&(this->vertexes[3]) + 3 * i, tocka1);
        _mm_storeu_ps(&(this->vertexes[3]) + 3 * i + 3, tocka2);
        _mm_storeu_ps(&(this->vertexes[3]) + 3 * i + 6, tocka3);
        _mm_storeu_ps(&(this->vertexes[3]) + 3 * i + 9, tocka4);

        __m128 tocka5 =
            _mm_shuffle_ps(xy_tocki12, heightdown, _MM_SHUFFLE(0, 0, 1, 0));
        __m128 tocka6 =
            _mm_shuffle_ps(xy_tocki12, heightdown, _MM_SHUFFLE(0, 0, 3, 2));

        __m128 tocka7 =
            _mm_shuffle_ps(xy_tocki34, heightdown, _MM_SHUFFLE(0, 0, 1, 0));
        __m128 tocka8 =
            _mm_shuffle_ps(xy_tocki34, heightdown, _MM_SHUFFLE(0, 0, 3, 2));

        _mm_storeu_ps(&(this->vertexes[second_half_ind]) + 3 * i, tocka5);
        _mm_storeu_ps(&(this->vertexes[second_half_ind]) + 3 * i + 3, tocka6);
        _mm_storeu_ps(&(this->vertexes[second_half_ind]) + 3 * i + 6, tocka7);
        _mm_storeu_ps(&(this->vertexes[second_half_ind]) + 3 * i + 9, tocka8);
    };
    _mm_store_ss(&(this->vertexes[second_half_ind - 3]),
                 _mm_setr_ps(0, 0, -0.5, -0.5));

    /*
    _mm_storeu_si128((__m128i *)(&(this->element_array[0]) + ind),
                     elements_high);
    _mm_storeu_si128((__m128i *)(&(this->element_array[0]) + ind + 3),
                     elements_low);

    __m128i stranski1 = _mm_unpackhi_epi32(elements_high, elements_low);
    __m128i stranski2 = _mm_unpackhi_epi32(elements_low, elements_high);

     _mm_storeu_si128((__m128i *)(&(this->element_array[0]) + ind + 6),
                     stranski1);
    _mm_storeu_si128((__m128i *)(&(this->element_array[0]) + ind + 9),
                     stranski2);

    elements_high = _mm_add_epi32(elements_high, next_element);
    elements_low = _mm_add_epi32(elements_low, next_element);

    //

    _mm_storeu_si128((__m128i *)(&(this->element_array[0]) + ind + 12),
                     elements_high);
    _mm_storeu_si128((__m128i *)(&(this->element_array[0]) + ind + 15),
                     elements_low);

   stranski1 = _mm_unpackhi_epi32(elements_high, elements_low);
   stranski2 = _mm_unpackhi_epi32(elements_low, elements_high);

   _mm_storeu_si128((__m128i *)(&(this->element_array[0]) + ind + 18),
                    stranski1);
   _mm_storeu_si128((__m128i *)(&(this->element_array[0]) + ind + 21),
                    stranski2);

    elements_low = _mm_add_epi32(elements_low, next_element);
    elements_high = _mm_add_epi32(elements_high, next_element);

    //

    _mm_storeu_si128((__m128i *)(&(this->element_array[0]) + ind + 24),
                     elements_high);
    _mm_storeu_si128((__m128i *)(&(this->element_array[0]) + ind + 27),
                     elements_low);

    stranski1 = _mm_unpackhi_epi32(elements_high, elements_low);
    stranski2 = _mm_unpackhi_epi32(elements_low, elements_high);

    _mm_storeu_si128((__m128i *)(&(this->element_array[0]) + ind + 30),
                     stranski1);
    _mm_storeu_si128((__m128i *)(&(this->element_array[0]) + ind + 33),
                     stranski2);

    elements_low = _mm_add_epi32(elements_low, jump);
    elements_high = _mm_add_epi32(elements_high, jump);

    // koncni elementi
    ind += 36;
}*/

    print_vertexes(&(this->vertexes[0]), 2 * num_upper_vertexes + 2, 3);
    std::cout << "\n\n" << std::endl;
    //    print_vertexes(&(this->element_array[0]), num_vertexes, 3);
}

#define __DISK__
#endif
