
template <RENDER_TYPE T = RENDER_TYPE::UNIFORM_COLOR>
class Circle : public Shape {
  private:
    // different shaders are compiled for different color schemes
    template <RENDER_TYPE Q = T>
    typename std::enable_if<Q == RENDER_TYPE::CUSTOM_COLOR, void>::type
    compile_shaders();

    template <RENDER_TYPE Q = T>
    typename std::enable_if<Q == RENDER_TYPE::UNIFORM_COLOR, void>::type
    compile_shaders();

    void generate_vertexes(int num_vert = -1);

  public:
    Circle();
    Circle(Circle &&) = default;
    Circle &operator=(Circle &&) = default;
    Circle(const Circle &) = default;
    Circle &operator=(const Circle &) = default;
    float perimeter();

    template <RENDER_TYPE Q = T>
    typename std::enable_if<Q == RENDER_TYPE::UNIFORM_COLOR, void>::type
    draw(float radius = 0.5, std::array<float, 3> translate = {0, 0, 0},
         std::array<float, 3> rotation_axis = {0, 0, 1}, float angle = 0,
         glm::vec4 color = {0.5, 0.5, 0.5, 0.5});

    template <RENDER_TYPE Q = T>
    typename std::enable_if<Q == RENDER_TYPE::CUSTOM_COLOR, void>::type
    draw(float radius = 0.5, std::array<float, 3> translate = {0, 0, 0},
         std::array<float, 3> rotation_axis = {0, 0, 1}, float angle = 0);
};

template <RENDER_TYPE T> Circle<T>::Circle() {
    min_vertexes = 20;
    generate_vertexes();
    compile_shaders();
    initialize_buffers();
};

template <RENDER_TYPE T> void Circle<T>::generate_vertexes(int num_vert) {
    // this function always generates 4n-1 different vertexes;
    // -1 becase the last point is the same as the first one
    int reminder;
    int number_of_vertexes; // actual total number of vertexes generated
    if ((reminder = min_vertexes % 4) != 0)
        number_of_vertexes = min_vertexes + reminder;
    else
        number_of_vertexes = min_vertexes;
    //   std::cout << "number of vertexes:  " << number_of_vertexes <<
    //   std::endl;

    if (num_vert != -1 && num_vert > 0) {
        number_of_vertexes = num_vert;
    }
    vertexes.reserve(2 * number_of_vertexes);
    vertexes.resize(2 * number_of_vertexes);
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

        _mm_storeu_ps(&vertexes[0] + vertexes_size, tocki12);
        vertexes_size += 4;
        _mm_storeu_ps(&vertexes[0] + vertexes_size, tocki34);
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

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    check_shader_program(shaderProgram);
    glUseProgram(shaderProgram);
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

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    check_shader_program(shaderProgram);
    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

template <RENDER_TYPE T>
template <RENDER_TYPE Q>
typename std::enable_if<Q == RENDER_TYPE::UNIFORM_COLOR>::type
Circle<T>::draw(float radius, std::array<float, 3> position,
                std::array<float, 3> rotation_axis, float angle,
                glm::vec4 color) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glUseProgram(shaderProgram);

    glm::mat4 trans = glm::mat4(1.0);
    // make rotation by appropriate angle

    trans =
        glm::translate(trans, glm::vec3(position[0], position[1], position[2]));
    trans = glm::rotate(
        trans, (float)angle,
        glm::vec3(rotation_axis[0], rotation_axis[1], rotation_axis[2]));
    trans = glm::scale(trans, glm::vec3(radius, radius, radius));

    // std::cout << glm::to_string(trans) << std::endl;

    unsigned int transformLoc =
        glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    unsigned int triangle_color = glGetUniformLocation(shaderProgram, "color");
    color = glm::vec4(1.0f, 0.5f, 0.2f, 0.3f);
    glUniform4fv(triangle_color, 1, glm::value_ptr(color));

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, 2 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glDrawElements(GL_TRIANGLES, element_array.size(), GL_UNSIGNED_INT, 0);

    color = glm::vec4(0.5f, 0.5f, 0.5f, 0.5f);
    glUniform4fv(triangle_color, 1, glm::value_ptr(color));

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // render as wireframe
    glDrawArrays(GL_LINE_STRIP, 0, vertexes.size() / 2);
}

template <RENDER_TYPE T>
template <RENDER_TYPE Q>
typename std::enable_if<Q == RENDER_TYPE::CUSTOM_COLOR>::type
Circle<T>::draw(float radius, std::array<float, 3> position,
                std::array<float, 3> rotation_axis, float angle) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glUseProgram(shaderProgram);

    glm::mat4 trans = glm::mat4(1.0);
    // make rotation by appropriate angle

    trans =
        glm::translate(trans, glm::vec3(position[0], position[1], position[2]));
    trans = glm::rotate(
        trans, (float)angle,
        glm::vec3(rotation_axis[0], rotation_axis[1], rotation_axis[2]));
    trans = glm::scale(trans, glm::vec3(radius, radius, radius));

    // std::cout << glm::to_string(trans) << std::endl;

    unsigned int transformLoc =
        glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    unsigned int triangle_color = glGetUniformLocation(shaderProgram, "color");
    glm::vec4 color = glm::vec4(1.0f, 0.5f, 0.2f, 0.3f);
    glUniform4fv(triangle_color, 1, glm::value_ptr(color));

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, 2 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glDrawElements(GL_TRIANGLES, element_array.size(), GL_UNSIGNED_INT, 0);

    color = glm::vec4(0.5f, 0.5f, 0.5f, 0.5f);
    glUniform4fv(triangle_color, 1, glm::value_ptr(color));

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // render as wireframe
    glDrawArrays(GL_LINE_STRIP, 0, vertexes.size() / 2);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // render as filled triangles
}
