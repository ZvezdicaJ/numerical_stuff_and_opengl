#ifndef __DISK__

template <typename T = float>
class Disk : public Shape3D<T> {
  private:
    void generate_vertexes();
    void generate_wheel_line_elements();
    std::vector<int> wheel_line_elements;

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
    this->min_vertexes = 50;
    this->generate_vertexes();
    this->initialize_buffers();
};

template <>
inline void Disk<float>::generate_vertexes() {
    // this function always generates 4n-1 different vertexes;
    // -1 becase the last point is the same as the first one
    int num_vertexes;
    int num_upper_vertexes = (min_vertexes - 2) / 2;
    int reminder4 = num_upper_vertexes % 4;

    if (reminder4 != 0)
        num_upper_vertexes += (4 - reminder4);
    num_vertexes = (num_upper_vertexes * 2) + 2;

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
    __m128 cons = _mm_set_ps(3, 2, 1, 0);
    __m128 korak_vec = _mm_set_ps1(korak);

    __m128i next_element = _mm_setr_epi32(0, 1, 1, 1);

    // first element forming upper circle
    __m128i elements_high = _mm_setr_epi32(0, 1, 2, 3);
    // first element forming lower circle
    __m128i elements_low =
        _mm_add_epi32(elements_high, _mm_set1_epi32(num_upper_vertexes + 1));

    int second_half_ind = 3 * num_vertexes / 2;
    int ind = 0;

    _mm_storeu_ps(&(this->vertexes[0]), _mm_setr_ps(0, 0, 0.5, 0.5));
    _mm_storeu_ps(&(this->vertexes[second_half_ind]),
                  _mm_setr_ps(0, 0, -0.5, -0.5));

    for (int i = 0; i < num_upper_vertexes; i += 4) {
        __m128 i_vec = _mm_set_ps1(i);
        __m128 fi_vec = _mm_mul_ps(_mm_add_ps(i_vec, cons), korak_vec);
        __m128 cos_vec = cos(fi_vec);
        __m128 sin_vec = sin(fi_vec);
        print_sse(_mm_set_ps(127, 95, 64, 32), "test: ");
        print_sse(fi_vec, "fi_vec: ");
        print_sse(cos_vec, "cos: ");
        print_sse(sin_vec, "sin: ");

        __m128 xy_tocki12 = _mm_unpacklo_ps(cos_vec, sin_vec);
        __m128 xy_tocki34 = _mm_unpackhi_ps(cos_vec, sin_vec);

        __m128 tocka1 = _mm_shuffle_ps(xy_tocki12, heightup, 0b0000100);
        __m128 tocka2 = _mm_shuffle_ps(xy_tocki12, heightup, 0b0001110);

        __m128 tocka3 = _mm_shuffle_ps(xy_tocki34, heightup, 0b0000100);
        __m128 tocka4 = _mm_shuffle_ps(xy_tocki34, heightup, 0b0001110);

        _mm_storeu_ps(&(this->vertexes[3]) + 3 * i, tocka1);
        _mm_storeu_ps(&(this->vertexes[3]) + 3 * i + 3, tocka2);
        _mm_storeu_ps(&(this->vertexes[3]) + 3 * i + 6, tocka3);
        _mm_storeu_ps(&(this->vertexes[3]) + 3 * i + 9, tocka4);

        __m128 tocka5 = _mm_shuffle_ps(xy_tocki12, heightdown, 0b0000100);
        __m128 tocka6 = _mm_shuffle_ps(xy_tocki12, heightdown, 0b0001110);

        __m128 tocka7 = _mm_shuffle_ps(xy_tocki34, heightdown, 0b0000100);
        __m128 tocka8 = _mm_shuffle_ps(xy_tocki34, heightdown, 0b0001110);

        _mm_storeu_ps(&(this->vertexes[second_half_ind + 3]) + 3 * i, tocka5);
        _mm_storeu_ps(&(this->vertexes[second_half_ind + 3]) + 3 * i + 3,
                      tocka6);
        _mm_storeu_ps(&(this->vertexes[second_half_ind + 3]) + 3 * i + 6,
                      tocka7);
        _mm_storeu_ps(&(this->vertexes[second_half_ind + 3]) + 3 * i + 9,
                      tocka8);

        for (int j = 0; j < 4; j++) {
            _mm_storeu_si128(
                (__m128i *)(&(this->element_array[0]) + ind + 12 * j),
                elements_high);
            _mm_storeu_si128(
                (__m128i *)(&(this->element_array[0]) + ind + 12 * j + 3),
                elements_low);

            __m128i stranski1 =
                _mm_unpacklo_epi32(_mm_shuffle_epi32(elements_high, 0b10011001),
                                   _mm_shuffle_epi32(elements_low, 0b10011001));
            print_sse(_mm_shuffle_epi32(elements_high, 0b10011001),
                      "shuffled:");

            __m128i stranski2 = _mm_unpackhi_epi32(
                _mm_shuffle_epi32(elements_low, 0b10011001),
                _mm_shuffle_epi32(elements_high, 0b01100110));

            _mm_storeu_si128(
                (__m128i *)(&(this->element_array[0]) + ind + 12 * j + 6),
                stranski1);
            _mm_storeu_si128(
                (__m128i *)(&(this->element_array[0]) + ind + 12 * j + 9),
                stranski2);

            elements_high = _mm_add_epi32(elements_high, next_element);
            elements_low = _mm_add_epi32(elements_low, next_element);
        }
        ind += 48;
    };

    _mm_storeu_si128(
        (__m128i *)(&(this->element_array[(num_upper_vertexes * 4 - 4) * 3])),
        _mm_setr_epi32(0, num_upper_vertexes, 1, 0));
    _mm_storeu_si128(
        (__m128i *)(&(this->element_array[(num_upper_vertexes * 4 - 3) * 3])),
        _mm_setr_epi32(num_upper_vertexes + 1, num_upper_vertexes + 2,
                       num_vertexes - 1, 0));
    _mm_storeu_si128(
        (__m128i *)(&(this->element_array[(num_upper_vertexes * 4 - 2) * 3])),
        _mm_setr_epi32(num_upper_vertexes + 2, num_vertexes - 1, 1, 0));

    _mm_storeu_si128(
        (__m128i *)(&(this->element_array[(num_upper_vertexes * 4 - 1) * 3])),
        _mm_setr_epi32(num_vertexes - 1, 1, num_upper_vertexes, 0));

    this->vertexes[second_half_ind] = 0;

    print_vertexes(&(this->vertexes[0]), 2 * num_upper_vertexes + 2, 3);
    std::cout << "\n\n" << std::endl;
    print_vertexes(&(this->element_array[0]), 4 * num_upper_vertexes, 3);
}

template <typename T>
void Disk<T>::generate_wheel_line_elements() {
    int number_of_points = this->vertexes.size() / 3;
    int nuber_of_lines = 3 * (number_of_points - 2) / 2;
    wheel_line_elements.reserve(number_of_lines + 1);
    wheel_line_elements.reserve(number_of_lines);
}

#define __DISK__
#endif