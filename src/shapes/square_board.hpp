#pragma once
#ifndef __SQUARE_BOARD__

/**
 *@class SquareBoard
 *@brief This class holds vertexes and other data for a circle in xy plane.
 *@todo Finish float version of perimeter function and implement double version.
 */
template <typename T = float>
class SquareBoard : public Shape2D<T> {
  private:
    unsigned size; /**< number of squares in the array */

    void generate_vertexes();
    void generate_filling_ebo();

  public:
    SquareBoard(unsigned = 50);
    SquareBoard(SquareBoard &&) = default;
    SquareBoard &operator=(SquareBoard &&) = default;
    SquareBoard(const SquareBoard &) = default;
    SquareBoard &operator=(const SquareBoard &) = default;
    T perimeter();
};

/** @brief Basic constructor for class SquareBoard.
 *  @details Constructor generates vertexes and initializes opengl buffers.
 *  @param size number of squares in single direction
 */
template <typename T>
SquareBoard<T>::SquareBoard(unsigned size_) : size(size_) {
    this->draw_type = 'E';
    this->filling_draw_type = 'E';
    this->vertex_size = 2;
    this->min_vertexes = 10;
    this->generate_vertexes();
    this->initialize_buffers();
    this->generate_filling_ebo();
};

/*@brief This function generates vertexes for SquareBoard.
 * @details The vertexes start in the upper left corner and continue in the
 * direction of lower y. This is done for each x value.
 */
template <>
inline void SquareBoard<float>::generate_vertexes() {
    std::cout << "generating board vertexes!" << std::endl;
    float step = 2.0 / (size); // 2 because 1-(-1)=2 - origin is at (-1,-1)
    unsigned num_points = size + 1;
    unsigned reminder = size % 4;
    unsigned integer_part = 4 * (size / 4);
    // number of vector in y direction - 4 points are processed at the same time
    this->vertexes.reserve(num_points * num_points * 2);
    this->vertexes.resize(num_points * num_points * 2);
    // std::cout << "vertexes size: " << num_points * num_points * 2 <<
    // std::endl;
    __m128 step_vec = _mm_set_ps1(step);
    __m128 origin = _mm_set_ps1(-1.0);
    unsigned index = 0;

    for (unsigned i = 0; i < num_points; i += 1) {
#ifdef __FMA__
        __m128 xcoord = _mm_fmadd_ps(step_vec, _mm_set_ps1(i), origin);
#endif
#ifndef __FMA__
        __m128 xcoord = _mm_add_ps(_mm_mul_ps(step, _mm_set_ps1(i)), origin);
#endif
        __m128 ansatz = _mm_sub_ps(_mm_set1_ps(num_points),
                                   _mm_setr_ps(1.0, 2.0, 3.0, 4.0));
        int stevec = 0;
        // j goes over y coord
        for (unsigned j = 0; j < num_points - 4; j += 4) {

            __m128 ycoord = _mm_add_ps(_mm_mul_ps(ansatz, step_vec), origin);
            ansatz = _mm_sub_ps(ansatz, _mm_set_ps1(4.0));
            std::cout << j << " " << std::endl;
            print_sse(ycoord, "ycoord: ");
            std::cout << "\n" << std::endl;

            __m128 tocki12 = _mm_shuffle_ps(xcoord, ycoord, 0b01000100);
            tocki12 = _mm_shuffle_ps(tocki12, tocki12, 0b11011000);
            _mm_storeu_ps(&(this->vertexes[index]), tocki12);
            index += 4;

            __m128 tocki34 = _mm_shuffle_ps(xcoord, ycoord, 0b11101101);
            tocki34 = _mm_shuffle_ps(tocki34, tocki34, 0b11011000);
            _mm_storeu_ps(&(this->vertexes[index]), tocki34);
            index += 4;
            stevec += 8;
        }

        //        for (unsigned j = 0; j <= reminder; j++) {
        for (int j = reminder; j >= 0; j -= 1) {
            vertexes[index] = (float)i * step - 1.0;
            index += 1;
            vertexes[index] = (float)(j)*step - 1.0;
            index += 1;
            stevec += 2;
        }
    }

    this->element_array.reserve(8 * size * size);
    this->element_array.resize(8 * size * size + 1);
    __m128i element_ansatz1 = _mm_setr_epi32(0, 1, size + 1, size + 2);
    __m128i element_ansatz2 = _mm_setr_epi32(0, size + 1, 1, size + 2);
    __m128i increase = _mm_set1_epi32(1);
    __m128i jump = _mm_set1_epi32(size + 1);
    index = 0;
    for (unsigned i = 0; i < size; i++) {
        for (unsigned j = 0; j < size; j++) {
            _mm_storeu_si128((__m128i *)&(this->element_array[index]),
                             element_ansatz1);
            index += 4;
            _mm_storeu_si128((__m128i *)&(this->element_array[index]),
                             element_ansatz2);
            index += 4;

            element_ansatz1 = _mm_add_epi32(element_ansatz1, increase);
            element_ansatz2 = _mm_add_epi32(element_ansatz2, increase);
        }
        element_ansatz1 = _mm_add_epi32(element_ansatz1, increase);
        element_ansatz2 = _mm_add_epi32(element_ansatz2, increase);
    }
}

/* @brief This function generates filling element buffer object.
 * each square consists of two triangles. Thus the number of triangles is
 * twice the number of squares and 6 times the number of squares
 *
 */
template <typename T>
inline void SquareBoard<T>::generate_filling_ebo() {
    unsigned index = 0;
    this->filling_elements.reserve(6 * size * size + 2);
    this->filling_elements.resize(6 * size * size);
    // below two ansatz represent the two triangles forming each square
    // triangle 1
    __m128i element_ansatz1 = _mm_setr_epi32(0, 1, size + 1, 0);
    // triangle 2
    __m128i element_ansatz2 = _mm_setr_epi32(1, size + 1, size + 2, 0);
    __m128i increase = _mm_set1_epi32(1);
    __m128i jump = _mm_set1_epi32(size);

    for (unsigned i = 0; i < size; i++) {
        for (unsigned j = 0; j < size; j++) {
            _mm_storeu_si128((__m128i *)&(this->filling_elements[index]),
                             element_ansatz1);
            index += 3;
            _mm_storeu_si128((__m128i *)&(this->filling_elements[index]),
                             element_ansatz2);
            index += 3;

            element_ansatz1 = _mm_add_epi32(element_ansatz1, increase);
            element_ansatz2 = _mm_add_epi32(element_ansatz2, increase);
        }
        element_ansatz1 = _mm_add_epi32(element_ansatz1, increase);
        element_ansatz2 = _mm_add_epi32(element_ansatz2, increase);
    }
    glGenBuffers(1, &(this->FILLING_EBO));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->FILLING_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(float) * this->filling_elements.size(),
                 &(this->filling_elements[0]), GL_STATIC_DRAW);
}

#ifdef __AVX2__
/* @brief This function generates filling element buffer object.
 * each square consists of two triangles. Thus the number of triangles is
 * twice the number of squares.
 */
template <>
inline void SquareBoard<double>::generate_vertexes() {
    double step = 2.0 / (size); // 2 because 1-(-1)=2 - origin is at (-1,-1)
    unsigned num_points = size + 1;
    // number of vector in y direction - 4 points are processed at the same
    // time
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

#endif

#endif
