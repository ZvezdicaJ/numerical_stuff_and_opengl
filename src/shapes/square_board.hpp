#pragma once
#ifndef __SQUARE_BOARD__

/**
 *@class A class which holds vertexes forming a circle.
 *@brief This class holds vertexes and other data for a circle in xy plane.
 @todo Finish float version of perimeter function and implement double version.
 */
template <typename T = float>
class SquareBoard : public Shape2D<T> {
  private:
    void generate_vertexes();
    unsigned size;
    void generate_filling_ebo();

  public:
    SquareBoard();
    SquareBoard(SquareBoard &&) = default;
    SquareBoard &operator=(SquareBoard &&) = default;
    SquareBoard(const SquareBoard &) = default;
    SquareBoard &operator=(const SquareBoard &) = default;
    T perimeter();
};

/** @brief Basic constructor for class SquareBoard.
 *  @details Constructor generates vertexes and initializes opengl buffers.
 */
template <typename T>
SquareBoard<T>::SquareBoard() {
    this->draw_type = 'V';
    this->vertex_size = 2;
    this->min_vertexes = 10;
    this->generate_vertexes();
    this->initialize_buffers();
    this->generate_filling_vbo();

    // std::cout << "circle buffers:\nVBO: " << this->VBO << "\nVAO: " <<
    // this->VAO << std::endl;
};

template <>
inline void SquareBoard<float>::generate_vertexes() {
    float step = 2.0 / (size); // 2 because 1-(-1)=2 - origin is at (-1,-1)
    unsigned num_points = size + 1;
    // number of vector in y direction - 4 points are processed at the same time
    unsigned num_vectors = num_points / 4;
    this->vertexes.reserve(num_points * 2);
    this->vertexes.resize(num_points * 2);

    __m128 step_vec = _mm_set_ps1(step);
    __m128 origin = _mm_set_ps1(-1.0);
    unsigned index = 0;
    for (unsigned i = 0; i < num_points; i++) {
#ifdef __FMA__
        __m128 xcoord = _mm_fmadd_ps(step_vec, _mm_set_ps1(i), origin);
#endif
#ifndef __FMA__
        __m128 xcoord = _mm_add_ps(_mm_mul_ps(step, _mm_set_ps1(i)), origin);
#endif
        __m128 ansatz = _mm_setr_ps(0.0, 1.0, 2.0, 3.0);
        for (unsigned j = 0; j < num_vectors; j++) {
            __m128 ycoord = _mm_mul_ps(ansatz, step_vec);
            ansatz = _mm_add_ps(ansatz, _mm_set_ps1(4.0));

            __m128 tocki12 = _mm_shuffle_ps(xcoord, ycoord, 0b01000100);
            tocki12 = _mm_shuffle_ps(tocki12, tocki12, 0b11011000);
            _mm_stream_ps(&(this->vertexes[index]), tocki12);
            index += 4;

            __m128 tocki34 = _mm_shuffle_ps(xcoord, ycoord, 0b11011101);
            tocki34 = _mm_shuffle_ps(tocki34, tocki34, 0b11011000);
            _mm_stream_ps(&(this->vertexes[index]), tocki34);
            index += 4;
        }
    }
    this->element_array.reserve(2 * size * size);
    this->element_array.resize(2 * size * size);
    __m128i element_ansatz1 = _mm_setr_epi32(0, 1, size, 0);
    __m128i element_ansatz2 = _mm_setr_epi32(1, size, size + 1, 0);
    __m128i increase = _mm_set1_epi32(1);
    __m128i jump = _mm_set1_epi32(size);
    for (unsigned i = 0; i < size; i++) {
        for (unsigned j = 0; j < size; j++) {
            _mm_stream_si128((__m128i *)&(this->element_array[index]),
                             element_ansatz1);
            index += 3;
            _mm_stream_si128((__m128i *)&(this->element_array[index]),
                             element_ansatz2);
            element_ansatz1 = _mm_add_epi32(element_ansatz1, increase);
            element_ansatz2 = _mm_add_epi32(element_ansatz2, increase);
        }
    }
}

template <>
inline void SquareBoard<float>::generate_filling_ebo() {
    unsigned index = 0;
    this->filling_elements.reserve(2 * size * size);
    this->filling_elements.resize(2 * size * size);
    __m128i element_ansatz1 = _mm_setr_epi32(0, 1, size, 0);
    __m128i element_ansatz2 = _mm_setr_epi32(1, size, size + 1, 0);
    __m128i increase = _mm_set1_epi32(1);
    __m128i jump = _mm_set1_epi32(size);
    for (unsigned i = 0; i < size; i++) {
        for (unsigned j = 0; j < size; j++) {
            _mm_stream_si128((__m128i *)&(this->filling_elements[index]),
                             element_ansatz1);
            index += 3;
            _mm_stream_si128((__m128i *)&(this->filling_elements[index]),
                             element_ansatz2);
            index += 3;
            element_ansatz1 = _mm_add_epi32(element_ansatz1, increase);
            element_ansatz2 = _mm_add_epi32(element_ansatz2, increase);
        }
    }
    glGenBuffers(1, &(this->FILLING_EBO));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->FILLING_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(float) * filling_elements.size(), &filling_elements[0],
                 GL_STATIC_DRAW);
}

#ifdef __AVX2__
template <>
inline void SquareBoard<double>::generate_vertexes() {
    double step = 2.0 / (size); // 2 because 1-(-1)=2 - origin is at (-1,-1)
    unsigned num_points = size + 1;
    // number of vector in y direction - 4 points are processed at the same time
    unsigned num_vectors = num_points / 4;
    this->vertexes.reserve(num_points * 2);
    this->vertexes.resize(num_points * 2);

    __m256d step_vec = _mm256_set1_pd(step);
    __m256d origin = _mm256_set1_pd(-1.0);
    unsigned index = 0;
    for (unsigned i = 0; i < num_points; i++) {
        __m256d xcoord = _mm256_fmadd_pd(step_vec, _mm256_set1_pd(i), origin);
        __m256d ansatz = _mm256_setr_pd(0.0, 1.0, 2.0, 3.0);
        for (unsigned j = 0; j < num_vectors; j++) {
            __m256d ycoord = _mm256_mul_pd(ansatz, step_vec);
            ansatz = _mm256_add_pd(ansatz, _mm256_set1_pd(4.0));

            __m256d tocki12 = _mm256_shuffle_pd(xcoord, ycoord, 0b01000100);
            tocki12 = _mm256_shuffle_pd(tocki12, tocki12, 0b11011000);
            _mm256_stream_pd(&vertexes[index], tocki12);
            index += 4;

            __m256d tocki34 = _mm256_shuffle_pd(xcoord, ycoord, 0b11011101);
            tocki34 = _mm256_shuffle_pd(tocki34, tocki34, 0b11011000);
            _mm256_stream_pd(&vertexes[index], tocki34);
            index += 4;
        }
    }
    this->element_array.reserve(2 * size * size);
    this->element_array.resize(2 * size * size);
    __m128i element_ansatz1 = _mm_setr_epi32(0, 1, size, 0);
    __m128i element_ansatz2 = _mm_setr_epi32(1, size, size + 1, 0);
    __m128i increase = _mm_set1_epi32(1);
    __m128i jump = _mm_set1_epi32(size);
    for (unsigned i = 0; i < size; i++) {
        for (unsigned j = 0; j < size; j++) {
            _mm_stream_si128((__m128i *)&(this->element_array[index]),
                             element_ansatz1);
            index += 3;
            _mm_stream_si128((__m128i *)&(this->element_array[index]),
                             element_ansatz2);
            index += 3;
            element_ansatz1 = _mm_add_epi32(element_ansatz1, increase);
            element_ansatz2 = _mm_add_epi32(element_ansatz2, increase);
        }
    }
}

template <>
inline void SquareBoard<double>::generate_filling_ebo() {
    unsigned index = 0;
    this->filling_elements.reserve(2 * size * size);
    this->filling_elements.resize(2 * size * size);
    __m128i element_ansatz1 = _mm_setr_epi32(0, 1, size, 0);
    __m128i element_ansatz2 = _mm_setr_epi32(1, size, size + 1, 0);
    __m128i increase = _mm_set1_epi32(1);
    __m128i jump = _mm_set1_epi32(size);
    for (unsigned i = 0; i < size; i++) {
        for (unsigned j = 0; j < size; j++) {
            _mm_stream_si128((__m128i *)&(this->filling_elements[index]),
                             element_ansatz1);
            index += 3;
            _mm_stream_si128((__m128i *)&(this->filling_elements[index]),
                             element_ansatz2);
            index += 3;
            element_ansatz1 = _mm_add_epi32(element_ansatz1, increase);
            element_ansatz2 = _mm_add_epi32(element_ansatz2, increase);
        }
    }
    glGenBuffers(1, &(this->FILLING_EBO));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->FILLING_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(float) * filling_elements.size(), &filling_elements[0],
                 GL_STATIC_DRAW);
}
#endif

/*
  static unsigned random_value() {
  static std::mt19937 engine{std::random_device{}()};
  static std::uniform_int_distribution<unsigned> distribution{0, 10};
  return distribution(engine);
  }
*/
#endif
