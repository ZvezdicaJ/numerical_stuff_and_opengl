#ifndef __CIRCLE__

template <typename T = float>
class Circle : public Shape2D<T> {
  private:
    void generate_vertexes(int = -1);

  public:
    Circle();
    Circle(Circle &&) = default;
    Circle &operator=(Circle &&) = default;
    Circle(const Circle &) = default;
    Circle &operator=(const Circle &) = default;
    T perimeter();
};

template <typename T>
Circle<T>::Circle() {
    this->draw_type = 'V';
    this->vertex_size = 2;
    this->min_vertexes = 50;
    this->generate_vertexes();
    this->initialize_buffers();
};

template <>
inline void Circle<float>::generate_vertexes(int num_vert) {
    // this function always generates 4n-1 different vertexes;
    // -1 becase the last point is the same as the first one
    int reminder;
    int number_of_vertexes; // actual total number of vertexes generated
    if ((reminder = this->min_vertexes % 4) != 0)
        number_of_vertexes = this->min_vertexes + reminder;
    else
        number_of_vertexes = this->min_vertexes;
    //   std::cout << "number of vertexes:  " << number_of_vertexes <<
    //   std::endl;

    if (num_vert != -1 && num_vert > 0) {
        number_of_vertexes = num_vert;
    }
    this->vertexes.reserve(2 * number_of_vertexes);
    this->vertexes.resize(2 * number_of_vertexes);
    float korak = 2.0 * M_PI / (float)(number_of_vertexes - 1);
    int vertexes_size = 0;
    for (int i = 0; i < number_of_vertexes / 4; i++) {
        __m128 fi_vec = _mm_set_ps(
            -M_PI + 4 * i * korak, -M_PI + (4 * i + 1) * korak,
            -M_PI + (4 * i + 2) * korak, -M_PI + (4 * i + 3) * korak);

        __m128 cos_vec = cos(fi_vec);
        __m128 sin_vec = sin(fi_vec);
        __m128 tocki12 = _mm_unpackhi_ps(cos_vec, sin_vec);
        tocki12 = _mm_shuffle_ps(tocki12, tocki12, _MM_SHUFFLE(1, 0, 3, 2));
        __m128 tocki34 = _mm_unpacklo_ps(cos_vec, sin_vec);
        tocki34 = _mm_shuffle_ps(tocki34, tocki34, _MM_SHUFFLE(1, 0, 3, 2));
        // print_sse_float(tocki12);
        // print_sse_float(tocki34);

        _mm_storeu_ps(&(this->vertexes[0]) + vertexes_size, tocki12);
        vertexes_size += 4;
        _mm_storeu_ps(&(this->vertexes[0]) + vertexes_size, tocki34);
        vertexes_size += 4;
    }
}

template <>
inline void Circle<double>::generate_vertexes(int num_vert) {
    // this function always generates 4n-1 different vertexes;
    // -1 becase the last point is the same as the first one
    int reminder;
    int number_of_vertexes; // actual total number of vertexes generated
    if ((reminder = this->min_vertexes % 4) != 0)
        number_of_vertexes = this->min_vertexes + reminder;
    else
        number_of_vertexes = this->min_vertexes;
    //   std::cout << "number of vertexes:  " << number_of_vertexes <<
    //   std::endl;

    if (num_vert != -1 && num_vert > 0) {
        number_of_vertexes = num_vert;
    }
    this->vertexes.reserve(2 * number_of_vertexes);
    this->vertexes.resize(2 * number_of_vertexes);
    float korak = 2.0 * M_PI / (float)(number_of_vertexes - 1);
    int vertexes_size = 0;
    for (int i = 0; i < number_of_vertexes / 4; i++) {
        __m256d fi_vec = _mm256_set_pd(
            -M_PI + 4 * i * korak, -M_PI + (4 * i + 1) * korak,
            -M_PI + (4 * i + 2) * korak, -M_PI + (4 * i + 3) * korak);

        __m256d cos_vec = cos(fi_vec);
        __m256d sin_vec = sin(fi_vec);
        __m256d tocki12 = _mm256_unpackhi_pd(cos_vec, sin_vec);
        tocki12 = _mm256_shuffle_pd(tocki12, tocki12, _MM_SHUFFLE(1, 0, 3, 2));
        __m256d tocki34 = _mm256_unpacklo_pd(cos_vec, sin_vec);
        tocki34 = _mm256_shuffle_pd(tocki34, tocki34, _MM_SHUFFLE(1, 0, 3, 2));
        // print_sse_float(tocki12);
        // print_sse_float(tocki34);

        _mm256_storeu_pd(&(this->vertexes[0]) + vertexes_size, tocki12);
        vertexes_size += 4;
        _mm256_storeu_pd(&(this->vertexes[0]) + vertexes_size, tocki34);
        vertexes_size += 4;
    }
}

template <>
inline float Circle<float>::perimeter() {
    float perim = 0;
    for (unsigned int i = 0; i < this->vertexes.size() / 4; i += 8) {
        __m128 vert12 = _mm_loadu_ps(&(this->vertexes[i]));
        __m128 vert34 = _mm_loadu_ps(&(this->vertexes[i + 4]));
        __m128 vert13 = _mm_shuffle_ps(vert12, vert34, _MM_SHUFFLE(3, 2, 3, 2));
        __m128 vert24 = _mm_shuffle_ps(vert12, vert34, _MM_SHUFFLE(1, 0, 1, 0));
        __m128 dif_vec = _mm_sub_ps(vert13, vert24);
        float dist2 = CalcDotProduct(dif_vec, dif_vec);
        perim += std::sqrt(dist2);
    }

    return perim;
}

#define __CIRCLE__
#endif
