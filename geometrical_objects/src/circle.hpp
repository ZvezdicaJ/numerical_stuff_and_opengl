#ifndef __CIRCLE__
template <RENDER_TYPE T = RENDER_TYPE::UNIFORM_COLOR>
class Circle : public Shape<T> {
  private:
    // different shaders are compiled for different color schemes
    template <RENDER_TYPE Q = T>
    typename std::enable_if<Q == RENDER_TYPE::CUSTOM_COLOR, void>::type
    compile_shaders();

    template <RENDER_TYPE Q = T>
    typename std::enable_if<Q == RENDER_TYPE::UNIFORM_COLOR, void>::type
    compile_shaders();

    void generate_vertexes(int = -1);

  public:
    Circle();
    Circle(Circle &&) = default;
    Circle &operator=(Circle &&) = default;
    Circle(const Circle &) = default;
    Circle &operator=(const Circle &) = default;

    float perimeter();
};

template <RENDER_TYPE T> Circle<T>::Circle() {
    this->min_vertexes = 100;
    this->generate_vertexes();
    this->compile_shaders();
    this->initialize_buffers();
};

template <RENDER_TYPE T> void Circle<T>::generate_vertexes(int num_vert) {
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

        __m128 cos_vec = sse_cos(fi_vec);
        __m128 sin_vec = sse_sin(fi_vec);
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
    //    std::cout << "\n\n" << std::endl;
    // print_vertexes(&vertexes[0], vertexes.size() / 2, 2);
}

template <RENDER_TYPE T>
template <RENDER_TYPE Q>
typename std::enable_if<Q == RENDER_TYPE::UNIFORM_COLOR, void>::type
Circle<T>::compile_shaders() {
    // create empty shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // load  vertex  shader and compile it
    glShaderSource(vertexShader, 1, &(circle_shaders::uniform_vertex_shader),
                   NULL);
    glCompileShader(vertexShader);
    // check if successfully compiled
    check_vertex_shader(vertexShader);

    // create empty fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // load fragment shader and compile it
    glShaderSource(fragmentShader, 1,
                   &(circle_shaders::uniform_fragment_shader), NULL);
    glCompileShader(fragmentShader);
    check_fragment_shader(fragmentShader);

    this->shaderProgram = glCreateProgram();
    glAttachShader(this->shaderProgram, vertexShader);
    glAttachShader(this->shaderProgram, fragmentShader);
    glLinkProgram(this->shaderProgram);
    check_shader_program(this->shaderProgram);
    glUseProgram(this->shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

template <RENDER_TYPE T>
template <RENDER_TYPE Q>
typename std::enable_if<Q == RENDER_TYPE::CUSTOM_COLOR, void>::type
Circle<T>::compile_shaders() {
    // create empty shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // load  vertex  shader and compile it
    glShaderSource(vertexShader, 1, &(circle_shaders::custom_vertex_shader),
                   NULL);
    glCompileShader(vertexShader);
    // check if successfully compiled
    check_vertex_shader(vertexShader);

    // create empty fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // load fragment shader and compile it
    glShaderSource(fragmentShader, 1, &(circle_shaders::custom_fragment_shader),
                   NULL);
    glCompileShader(fragmentShader);
    check_fragment_shader(fragmentShader);

    this->shaderProgram = glCreateProgram();
    glAttachShader(this->shaderProgram, vertexShader);
    glAttachShader(this->shaderProgram, fragmentShader);
    glLinkProgram(this->shaderProgram);
    check_shader_program(this->shaderProgram);
    glUseProgram(this->shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

template <RENDER_TYPE T> float Circle<T>::perimeter() {
    float perimeter = 0;
    for (int i = 0; i < this->vertexes.size() / 4; i += 8) {
        __m128 vert12 = _mm_loadu_ps(&(this->vertexes[i]));
        __m128 vert34 = _mm_loadu_ps(&(this->vertexes[i + 4]));
        __m128 vert13 = _mm_shuffle_ps(vert12, vert34, _MM_SHUFFLE(3, 2, 3, 2));
        __m128 vert24 = _mm_shuffle_ps(vert12, vert34, _MM_SHUFFLE(1, 0, 1, 0));
        __m128 dif_vec = _mm_sub_ps(vert13, vert24);
        float dist2 = CalcDotProductSse(dif_vec, dif_vec);
        perimeter += std::sqrt(dist2);
    }

    return perimeter;
}

#define __CIRCLE__
#endif
