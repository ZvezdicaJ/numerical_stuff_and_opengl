#ifndef __SHAPE__

/**
 * @class Shape
 * @brief virtual base class for 2D and 3D shapes
 * @details All shapes inherit from this class. It contains all basic
 * structures for drawing including opengl buffers.
 */
template <typename T>
class Shape {
    static_assert(std::is_same<float, T>::value ||
                      std::is_same<double, T>::value,
                  "Shapes can only be instantiated with floating point types: "
                  "float, double, long double!");

  protected:
    unsigned vertex_size = 0;
    aligned_vector<T> vertexes; /**< Vector holding all vertexes; 2,3 or 4
            consequtive numbers form a  vertex */
    char draw_type; /**< this can either be 'E' or 'V', depending on wheather to
                     draw elements (element buffer) or vertexes (vertex buffer
                     object) */
    aligned_vector<int>
        element_array; /**< vector holding all elements in correct order */
    bool colors_loaded =
        false; /**< indicator whether color have been loaded or not  */
    /**< vertex size can consist of 2, 3 or 4 points; this is important
             for correct interpretation of vertexes vector*/
    unsigned VBO; /**< vertex buffer object */
    unsigned VAO; /**< vertex array object*/
    unsigned EBO; /**< element buffer address */
    unsigned CBO; /**< color buffer address */
    unsigned min_vertexes =
        5; /**< minimal number of vertexes to be generated for a given shape */
    aligned_vector<float>
        vertex_colors; /**< Vector holding a color for each vertex. Four
                          consequtive numbers form a rgb color value*/

    /**
     * @brief Allocates and initializes vertex buffer object, element buffer
     * object and vertex array object. It also allocates color buffer - where
     * color for each vertex is stored.
     */
    void initialize_buffers() {

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        // std::cout << "vertexes size: " << vertexes.size() << std::endl;
        // generate and bind and fill vertex data
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * vertexes.size(), &vertexes[0],
                     GL_STATIC_DRAW);

        // generate and bind and fill element data
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(int) * element_array.size(), &element_array[0],
                     GL_STATIC_DRAW);

        // generate color buffer but do not load data;
        // load data
        glGenBuffers(1, &CBO);
        // std::cout << "shape buffers:\nVBO: " << this->VBO
        //          << "\nVAO: " << this->VAO << std::endl;
    }

  public:
    /**
     * @brief As the name says, it sets minimal number of vertexes for a given
     * shape
     * @param num minimal number of vertexes
     */
    virtual void set_min_number_of_vertexes(unsigned num) {
        min_vertexes = num;
    };

    /**
     * @brief Set color for each vertex.
     * @param colors_ A vector containing colors. 4 consequtive numbers form an
     * rgb color value. Number of colors should be the same as the number of
     * vertexes.
     */
    virtual void set_vertex_colors(aligned_vector<float> &colors_) {
        assert(colors_.size() / 4 == vertexes.size() / vertex_size &&
               "Each vertex should have a color value in the form of vec4: "
               "incorrect size of color vector!");
        vertex_colors = colors_;
    }

    aligned_vector<T> get_vertexes() { return vertexes; }
    aligned_vector<T> get_colors() { return vertex_colors; }
    unsigned num_vertexes() { return vertexes.size() / vertex_size; }
    virtual unsigned get_vertex_size() { return vertex_size; }

    /**
     *@brief Generate random colors for each vertex.
     */
    void generate_random_colors() {
        int size = 4 * vertexes.size() / vertex_size;
        vertex_colors.reserve(size);

        std::default_random_engine generator;
        std::uniform_real_distribution<float> distribution(0, 1);
        auto random_real = std::bind(distribution, generator);

        for (int i = 0; i < size; i++) {
            vertex_colors.emplace_back(random_real());
        }
        glBindBuffer(GL_ARRAY_BUFFER, CBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (vertex_colors.size()),
                     &(vertex_colors[0]), GL_STATIC_DRAW);
    }

    // friends
    friend void draw<T>(Shape<T> &, Shader<RENDER_TYPE::UNIFORM_COLOR> &,
                        std::array<float, 3>, std::array<float, 3>,
                        std::array<float, 3>, float, glm::vec4);

    friend void draw<T>(Shape<T> &, Shader<RENDER_TYPE::CUSTOM_COLOR> &,
                        std::array<float, 3>, std::array<float, 3>,
                        std::array<float, 3>, float);

    friend void draw_wireframe<T>(
        Shape<T> &shape, Shader<RENDER_TYPE::UNIFORM_COLOR> &shader_object,
        std::array<float, 3> scale, std::array<float, 3> position,
        std::array<float, 3> rotation_axis, float angle, glm::vec4);
};

/**
 * @class Shape2D
 * @brief This is a base class for all 2D shapes.
 * @details It inherits from Shape class. Template parameter can either be float
 * of double. It is meant to contain functions common to all 2D shapes.
 */
template <typename T>
class Shape2D : public Shape<T> {
  protected:
    unsigned FILLING_VBO; /**<2D shapes consist of line, this buffer is meant to
                             fill 2D shapes.   */
    aligned_vector<T>
        filling_vertexes; /**<vertexes for interior of 2D shapes.  */
    void generate_filling_vbo();

  public:
    aligned_vector<T> get_filling_vertexes() { return filling_vertexes; }

    friend void draw_2d_object<T>(
        Shape2D<T> &shape, Shader<RENDER_TYPE::UNIFORM_COLOR> &shader_object,
        std::array<float, 3> scale, std::array<float, 3> position,
        std::array<float, 3> rotation_axis, float angle, glm::vec4);
};

/**
 * @brief Generates vertexes which fill the interior of 2D shapes.
 */
template <>
inline void Shape2D<float>::generate_filling_vbo() {

    int vertexes_size = (this->vertexes.size());
    int number_of_points = vertexes_size / (this->vertex_size);
    int filling_number_of_points;
    if (number_of_points % 2 == 0)
        filling_number_of_points = 3 * number_of_points / 2 + 1;
    else
        filling_number_of_points = 3 * (number_of_points) / 2;
    filling_vertexes.reserve(2 * filling_number_of_points);
    filling_vertexes.resize(2 * filling_number_of_points);

    std::cout << "number of points: " << number_of_points << std::endl;
    std::cout << "filling_vertexes size: " << filling_vertexes.size()
              << std::endl;

    for (int i = 0; i < number_of_points - 1; i += 2) {
        // std::cout << i << std::endl;
        const __m128i point12 =
            _mm_stream_load_si128((__m128i *)&(this->vertexes[2 * i]));
        _mm_storeu_si128((__m128i *)(&filling_vertexes[0] + 3 * i), point12);
        _mm_storeu_ps(&filling_vertexes[0] + 3 * i + 4, _mm_setzero_ps());
    }
    if (number_of_points % 2 == 0) {
        filling_vertexes[2 * filling_number_of_points - 2] = vertexes[0];
        filling_vertexes[2 * filling_number_of_points - 1] = vertexes[1];
    } else {
        filling_vertexes[2 * filling_number_of_points - 2] = vertexes[0];
        filling_vertexes[2 * filling_number_of_points - 1] = vertexes[1];
        filling_vertexes[2 * filling_number_of_points - 3] =
            vertexes[2 * number_of_points - 2];
        filling_vertexes[2 * filling_number_of_points - 4] =
            vertexes[2 * number_of_points - 1];
    }
    print_vertexes(&filling_vertexes[0], filling_vertexes.size() / 2, 2);
    /*std::cout
        << "address is 16 byte aligned: "
        << (((unsigned long)(&filling_vertexes[filling_number_of_points - 4]) &
             15))
        << std::endl;
    */
    //    _mm_storeu_ps(&filling_vertexes[filling_number_of_points - 4],
    //    point12);
    glGenBuffers(1, &FILLING_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, FILLING_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * filling_vertexes.size(),
                 &filling_vertexes[0], GL_STATIC_DRAW);
}

/**
 * @class Shape3D
 * @brief This is a base class for all 3D shapes.
 * @details It inherits from Shape class. Template parameter can either be float
 * of double. It is meant to contain functions common to all 3D shapes - like
 * area and volume.
 */
template <typename T>
class Shape3D : public Shape<T> {
  protected:
  public:
    T area();
};

/**
 * @brief This function calculates area of the given 3D shape.
 * @details It sums the areas of all triangles. For each triangle it calculates
 * the area using Heron formula.
 */
template <>
inline float Shape3D<float>::area() {
    float ar = 0;
    unsigned triangle_number = this->element_array.size() / 3;
    for (unsigned tri = 0; tri < triangle_number; tri += 1) {
        int vert1_ind = this->element_array[3 * tri];
        int vert2_ind = this->element_array[3 * tri + 1];
        int vert3_ind = this->element_array[3 * tri + 2];

        __m128 vert1 = load_vertex(&(this->vertexes[vert1_ind * 3]));
        __m128 vert2 = load_vertex(&(this->vertexes[vert2_ind * 3]));
        __m128 vert3 = load_vertex(&(this->vertexes[vert3_ind * 3]));

        __m128 vec12 = _mm_sub_ps(vert1, vert2);
        __m128 vec23 = _mm_sub_ps(vert2, vert3);
        __m128 vec13 = _mm_sub_ps(vert1, vert3);

        float distance12 = std::sqrt(CalcDotProduct(vec12, vec12));
        float distance23 = std::sqrt(CalcDotProduct(vec23, vec23));
        float distance13 = std::sqrt(CalcDotProduct(vec13, vec13));

        float s = (distance12 + distance23 + distance13) / 2.0;
        float increment = std::sqrt(s * (s - distance12) * (s - distance13) *
                                    (s - distance23));
        // std::cout << "triangle " << tri << "  " << increment << std::endl;
        ar += increment;
    }
    return ar;
}

/**
 * @brief This function calculates area of the given 3D shape.
 * @details It sums the areas of all triangles. For each triangle it calculates
 * the area using Heron formula.
 */
template <>
inline double Shape3D<double>::area() {
    double ar = 0;
    unsigned triangle_number = this->element_array.size() / 3;
    __m256d zeros = _mm256_setzero_pd();

    // print_avx(_mm256_setr_pd(0, 64, 128, 196));
    for (unsigned tri = 0; tri < triangle_number; tri += 1) {
        int vert1_ind = this->element_array[3 * tri];
        int vert2_ind = this->element_array[3 * tri + 1];
        int vert3_ind = this->element_array[3 * tri + 2];

        __m256d vert1 = _mm256_blend_pd(
            _mm256_loadu_pd(&(this->vertexes[vert1_ind * 3])), zeros, 0b1000);
        // print_avx(vert1, "vert1: ");
        __m256d vert2 = _mm256_blend_pd(
            _mm256_loadu_pd(&(this->vertexes[vert2_ind * 3])), zeros, 0b1000);
        __m256d vert3 = _mm256_blend_pd(
            _mm256_loadu_pd(&(this->vertexes[vert3_ind * 3])), zeros, 0b1000);

        __m256d vec12 = _mm256_sub_pd(vert1, vert2);
        __m256d vec23 = _mm256_sub_pd(vert2, vert3);
        __m256d vec13 = _mm256_sub_pd(vert1, vert3);

        double distance12 = std::sqrt(CalcDotProduct(vec12, vec12));
        double distance23 = std::sqrt(CalcDotProduct(vec23, vec23));
        double distance13 = std::sqrt(CalcDotProduct(vec13, vec13));

        double s = (distance12 + distance23 + distance13) / 2.0;
        ar += std::sqrt(s * (s - distance12) * (s - distance13) *
                        (s - distance23));
    }
    return ar;
}

#define __SHAPE__
#endif
