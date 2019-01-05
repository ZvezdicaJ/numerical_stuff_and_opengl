#ifndef __SHAPE__

/**
@class Shape
@brief virtual base class
@
 */
template <typename T>
class Shape {
    static_assert(std::is_same<float, T>::value ||
                      std::is_same<double, T>::value,
                  "Shapes can only be instantiated with floating point types: "
                  "float, double, long double!");

  protected:
    unsigned vertex_size = 0;
    std::vector<T> vertexes; /**< Vector holding all vertexes; 2,3 or 4
                                    consequtive numbers form a  vertex */
    char draw_type; /**< this can either be 'E' or 'V', depending on wheather to
                     draw elements (element buffer) or vertexes (vertex buffer
                     object) */
    std::vector<int>
        element_array; /**< vector holding all elements in correct order */
    bool colors_loaded =
        false; /**< indicator whether color have been loaded or not  */
    /**< vertex size can consist of 2, 3 or 4 points; this is important
             for correct interpretation of vertexes vector*/
    unsigned VBO; /**< vertex buffer object */
    unsigned VAO; /**< vertex array object*/
    unsigned EBO; /**< element buffer address */
    unsigned CBO; /**< color buffer address */
    unsigned min_vertexes = 5;
    std::vector<float> vertex_colors;

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
    virtual void set_min_number_of_vertexes(unsigned num) {
        min_vertexes = num;
    };

    virtual void set_vertex_colors(std::vector<float> &colors_) {
        assert(colors_.size() / 4 == vertexes.size() / vertex_size &&
               "Each vertex should have a color value in the form of vec4: "
               "incorrect size of color vector!");
        vertex_colors = colors_;
    }

    std::vector<T> get_vertexes() { return vertexes; }
    std::vector<T> get_colors() { return vertex_colors; }
    unsigned num_vertexes() { return vertexes.size() / vertex_size; }
    virtual unsigned get_vertex_size() { return vertex_size; }

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

template <typename T>
class Shape2D : public Shape<T> {
  protected:
  public:
};

template <typename T>
class Shape3D : public Shape<T> {
  protected:
  public:
    T area();
};

template <>
inline float Shape3D<float>::area() {
    float ar = 0;
    for (unsigned tri = 0; tri < this->element_array.size() / 3; tri += 1) {
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
        ar += std::sqrt(s * (s - distance12) * (s - distance13) *
                        (s - distance23));
    }
    return ar;
}

#define __SHAPE__
#endif
