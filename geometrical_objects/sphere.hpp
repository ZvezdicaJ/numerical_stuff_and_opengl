#ifndef __SPHERE__

enum class RENDER_TYPE { UNIFORM_COLOR = 0, CUSTOM_COLOR = 1 };

template <RENDER_TYPE T = RENDER_TYPE::UNIFORM_COLOR>
class Sphere : public Shape {
  private:
    RENDER_TYPE Rtype = T;
    void generate_vertexes_helper();
    void generate_vertexes_helper_improved();
    void generate_vertexes();
    void compile_shaders();

  public:
    // sphere() = default;
    Sphere();
    Sphere(Sphere &&) = default;
    Sphere &operator=(Sphere &&) = default;
    Sphere(const Sphere &) = default;
    Sphere &operator=(const Sphere &) = default;

    void draw_wireframe(float radius, std::array<float, 3> translate,
                        std::array<float, 3> rotation_axis, float angle);
    template <RENDER_TYPE Q = T>
    typename std::enable_if<Q == RENDER_TYPE::UNIFORM_COLOR, void>::type
    draw(float radius = 0.5, std::array<float, 3> translate = {0, 0, 0},
         std::array<float, 3> rotation_axis = {0, 0, 1}, float angle = 0,
         glm::vec4 color = {0.5, 0.5, 0.5, 0.5});
    template <RENDER_TYPE Q = T>
    typename std::enable_if<Q == RENDER_TYPE::CUSTOM_COLOR, void>::type
    draw(float radius = 0.5, std::array<float, 3> translate = {0, 0, 0},
         std::array<float, 3> rotation_axis = {0, 0, 1}, float angle = 0);

    void refine();
    float area();
    float quality();

    template <RENDER_TYPE Q = T>
    typename std::enable_if<Q == RENDER_TYPE::CUSTOM_COLOR, void>::type
    compile_shaders();
};

template <RENDER_TYPE T> Sphere<T>::Sphere() {
    generate_vertexes();
    compile_shaders();
    initialize_buffers();
}

template <RENDER_TYPE T> void Sphere<T>::generate_vertexes() {
    vertexes.reserve(15);
    vertexes.resize(15);
    std::array<float, 3> dir1({1.0f, 0, 0});
    std::array<float, 3> dir2 = {std::cos(2.0f * M_PI / 3.0f),
                                 std::sin(2.0f * M_PI / 3.0f), 0};
    std::array<float, 3> dir3 = {std::cos(4.0f * M_PI / 3.0f),
                                 std::sin(4.0f * M_PI / 3.0f), 0};
    std::array<float, 3> z_dir = {0, 0, 1.0f};
    // vertex 1
    vertexes[0] = dir1[0];
    vertexes[1] = dir1[1];
    vertexes[2] = dir1[2];
    // vertex 2
    vertexes[3] = dir2[0];
    vertexes[4] = dir2[1];
    vertexes[5] = dir2[2];
    // vertex 3
    vertexes[6] = dir3[0];
    vertexes[7] = dir3[1];
    vertexes[8] = dir3[2];
    // vertex 4
    vertexes[9] = z_dir[0];
    vertexes[10] = z_dir[1];
    vertexes[11] = z_dir[2];
    // vertex 5
    vertexes[12] = -z_dir[0];
    vertexes[13] = -z_dir[1];
    vertexes[14] = -z_dir[2];

    // definition of triangles we will later divide to create very fine mesh for
    // out sphere print_vertexes<float, 15>(vertexes);
    element_array = std::vector<int>(
        {0, 1, 3, 0, 1, 4, 0, 2, 3, 0, 2, 4, 1, 2, 3, 1, 2, 4});
    generate_vertexes_helper_improved();
}

template <RENDER_TYPE T> void Sphere<T>::generate_vertexes_helper_improved() {
    int vertex_number = vertexes.size() / 3;
    vertexes.reserve(vertexes.size() * 5 + 1);
    int num_tri = element_array.size() / 3;
    std::vector<int> new_element_array; // = element_array;
    new_element_array.reserve(4 * element_array.size() + 1);
    std::unordered_map<std::pair<int, int>, int> new_vertex_indexing;
    new_vertex_indexing.reserve(vertex_number + 1);
    // print_vertexes(&vertexes[0], vertex_number);
    for (int tri = 0; tri < num_tri; tri++) {
        int vert1_ind = element_array[3 * tri];
        int vert2_ind = element_array[3 * tri + 1];
        int vert3_ind = element_array[3 * tri + 2];
        int p12_ind = -1;
        int p23_ind = -1;
        int p13_ind = -1;

        std::pair<int, int> ind_pair1;
        std::pair<int, int> ind_pair2;
        std::pair<int, int> ind_pair3;

        // setting up index pairs to be checked
        if (vert1_ind < vert2_ind)
            ind_pair1 = std::pair<int, int>(vert1_ind, vert2_ind);
        else
            ind_pair1 = std::pair<int, int>(vert2_ind, vert1_ind);

        if (vert2_ind < vert3_ind)
            ind_pair2 = std::pair<int, int>(vert2_ind, vert3_ind);
        else
            ind_pair2 = std::pair<int, int>(vert2_ind, vert3_ind);

        if (vert1_ind < vert3_ind)
            ind_pair3 = std::pair<int, int>(vert1_ind, vert3_ind);
        else
            ind_pair3 = std::pair<int, int>(vert3_ind, vert1_ind);

        // checking index pairs
        auto iter = new_vertex_indexing.find(ind_pair1);
        if (iter != new_vertex_indexing.end())
            p12_ind = iter->second;

        iter = new_vertex_indexing.find(ind_pair2);
        if (iter != new_vertex_indexing.end())
            p23_ind = iter->second;

        iter = new_vertex_indexing.find(ind_pair3);
        if (iter != new_vertex_indexing.end())
            p13_ind = iter->second;

        __m128 vert1 = _mm_loadu_ps(&vertexes[vert1_ind * 3]);
        __m128 vert2 = _mm_loadu_ps(&vertexes[vert2_ind * 3]);
        __m128 vert3 = _mm_loadu_ps(&vertexes[vert3_ind * 3]);
        __m128 p12 = _mm_div_ps(
            _mm_add_ps(vert1, vert2),
            _mm_set_ps(std::numeric_limits<float>::infinity(), 2.0, 2.0, 2.0));
        __m128 p23 = _mm_div_ps(
            _mm_add_ps(vert2, vert3),
            _mm_set_ps(std::numeric_limits<float>::infinity(), 2.0, 2.0, 2.0));
        __m128 p13 = _mm_div_ps(
            _mm_add_ps(vert1, vert3),
            _mm_set_ps(std::numeric_limits<float>::infinity(), 2.0, 2.0, 2.0));

        float norm12 = std::sqrt(CalcDotProductSse(p12, p12));
        float norm23 = std::sqrt(CalcDotProductSse(p23, p23));
        float norm13 = std::sqrt(CalcDotProductSse(p13, p13));

        p12 = _mm_div_ps(p12, _mm_set_ps(1.0, norm12, norm12, norm12));
        p13 = _mm_div_ps(p13, _mm_set_ps(1.0, norm13, norm13, norm13));
        p23 = _mm_div_ps(p23, _mm_set_ps(1.0, norm23, norm23, norm23));

        int vertexes_size = vertexes.size();
        if (p12_ind == -1) {
            vertexes.resize(vertexes.size() + 3);
            _mm_storeu_ps(&vertexes[vertexes_size], p12);
            p12_ind = vertex_number;
            new_vertex_indexing[ind_pair1] = p12_ind;
            vertex_number += 1;
            vertexes_size += 3;
        }
        if (p23_ind == -1) {
            vertexes.resize(vertexes.size() + 3);
            _mm_storeu_ps(&vertexes[vertexes_size], p23);
            p23_ind = vertex_number;
            new_vertex_indexing[ind_pair2] = p23_ind;
            vertex_number += 1;
            vertexes_size += 3;
        }
        if (p13_ind == -1) {
            vertexes.resize(vertexes.size() + 3);
            _mm_storeu_ps(&vertexes[vertexes_size], p13);
            p13_ind = vertex_number;
            new_vertex_indexing[ind_pair3] = p13_ind;
            vertex_number += 1;
            vertexes_size += 3;
        }
        // set the correct vertex element numbers
        __m128i new_tri1 =
            _mm_set_epi32(-1, p12_ind, p23_ind, element_array[3 * tri + 1]);
        __m128i new_tri2 =
            _mm_set_epi32(-1, p23_ind, p13_ind, element_array[3 * tri + 2]);
        __m128i new_tri3 =
            _mm_set_epi32(-1, p12_ind, p13_ind, element_array[3 * tri]);
        __m128i new_tri4 = _mm_set_epi32(-1, p12_ind, p23_ind, p13_ind);

        int el_array_size = new_element_array.size();
        new_element_array.resize(el_array_size + 12);
        _mm_storeu_si128((__m128i_u *)&new_element_array[el_array_size],
                         new_tri1);
        _mm_storeu_si128((__m128i_u *)&new_element_array[el_array_size + 3],
                         new_tri2);
        _mm_storeu_si128((__m128i_u *)&new_element_array[el_array_size + 6],
                         new_tri3);
        _mm_storeu_si128((__m128i_u *)&new_element_array[el_array_size + 9],
                         new_tri4);

        // vertex_number = *std::max_element(std::begin(new_element_array),
        //                                  std::end(new_element_array)) +
        //                1;
        // std::cout << "new_vertex_indexing:\n"
        //          << new_vertex_indexing << std::endl;
        // std::cout << "new element array:  \n" << new_element_array <<
        // std::endl; print_vertexes(&vertexes[0], vertex_number);
    }
    element_array = new_element_array;
    if (vertexes.size() < min_vertexes)
        generate_vertexes_helper_improved();
    // print_vertexes(&vertexes[0], vertexes.size() / 3);
    // std::cout << "\n\n" << element_array << std::endl;
}

template <RENDER_TYPE T> void Sphere<T>::generate_vertexes_helper() {
    int vertex_number = vertexes.size() / 3;
    vertexes.reserve(vertexes.size() * 5 + 1);
    int num_tri = element_array.size() / 3;
    std::vector<int> new_element_array; // = element_array;
    new_element_array.reserve(4 * element_array.size() + 1);

    for (int tri = 0; tri < num_tri; tri++) {
        __m128 vert1 = _mm_loadu_ps(&vertexes[element_array[3 * tri] * 3]);
        __m128 vert2 = _mm_loadu_ps(&vertexes[element_array[3 * tri + 1] * 3]);
        __m128 vert3 = _mm_loadu_ps(&vertexes[element_array[3 * tri + 2] * 3]);
        __m128 p12 = _mm_div_ps(
            _mm_add_ps(vert1, vert2),
            _mm_set_ps(std::numeric_limits<float>::infinity(), 2.0, 2.0, 2.0));
        __m128 p23 = _mm_div_ps(
            _mm_add_ps(vert2, vert3),
            _mm_set_ps(std::numeric_limits<float>::infinity(), 2.0, 2.0, 2.0));
        __m128 p13 = _mm_div_ps(
            _mm_add_ps(vert1, vert3),
            _mm_set_ps(std::numeric_limits<float>::infinity(), 2.0, 2.0, 2.0));

        float norm12 = std::sqrt(CalcDotProductSse(p12, p12));
        float norm23 = std::sqrt(CalcDotProductSse(p23, p23));
        float norm13 = std::sqrt(CalcDotProductSse(p13, p13));
        // std::cout << "norm12: " << norm12 << "  norm13:  " << norm13
        //         << "  norm23  :" << norm23 << std::endl;
        p12 = _mm_div_ps(p12, _mm_set_ps(1.0, norm12, norm12, norm12));
        p13 = _mm_div_ps(p13, _mm_set_ps(1.0, norm13, norm13, norm13));
        p23 = _mm_div_ps(p23, _mm_set_ps(1.0, norm23, norm23, norm23));

        float norm = CalcDotProductSse(p12, p12);
        int vertexes_size = vertexes.size();
        vertexes.resize(vertexes.size() + 9);

        _mm_storeu_ps(&vertexes[vertexes_size], p12);
        _mm_storeu_ps(&vertexes[vertexes_size + 3], p23);
        _mm_storeu_ps(&vertexes[vertexes_size + 6], p13);
        // set the correct vertex element numbers
        __m128i new_tri1 = _mm_set_epi32(-1, vertex_number, vertex_number + 1,
                                         element_array[3 * tri + 1]);

        __m128i new_tri2 =
            _mm_set_epi32(-1, vertex_number + 1, vertex_number + 2,
                          element_array[3 * tri + 2]);
        __m128i new_tri3 = _mm_set_epi32(-1, vertex_number, vertex_number + 2,
                                         element_array[3 * tri]);
        __m128i new_tri4 = _mm_set_epi32(-1, vertex_number, vertex_number + 1,
                                         vertex_number + 2);

        int el_array_size = new_element_array.size();
        new_element_array.resize(el_array_size + 12);
        _mm_storeu_si128((__m128i_u *)&new_element_array[el_array_size],
                         new_tri1);
        _mm_storeu_si128((__m128i_u *)&new_element_array[el_array_size + 3],
                         new_tri2);
        _mm_storeu_si128((__m128i_u *)&new_element_array[el_array_size + 6],
                         new_tri3);
        _mm_storeu_si128((__m128i_u *)&new_element_array[el_array_size + 9],
                         new_tri4);

        vertex_number = *std::max_element(std::begin(new_element_array),
                                          std::end(new_element_array)) +
                        1;
    }
    element_array = new_element_array;
    if (vertexes.size() < min_vertexes)
        generate_vertexes_helper();
}

template <RENDER_TYPE T> void Sphere<T>::compile_shaders(){};

template <> inline void Sphere<RENDER_TYPE::UNIFORM_COLOR>::compile_shaders() {
    // create empty shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // load  vertex  shader and compile it
    glShaderSource(vertexShader, 1, &(shaders::uniform_vertex_shader), NULL);
    glCompileShader(vertexShader);
    // check if successfully compiled
    check_vertex_shader(vertexShader);

    // create empty fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // load fragment shader and compile it
    glShaderSource(fragmentShader, 1, &(shaders::uniform_fragment_shader),
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

template <> inline void Sphere<RENDER_TYPE::CUSTOM_COLOR>::compile_shaders() {
    // create empty shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // load  vertex  shader and compile it
    glShaderSource(vertexShader, 1, &(shaders::custom_vertex_shader), NULL);
    glCompileShader(vertexShader);
    // check if successfully compiled
    check_vertex_shader(vertexShader);

    // create empty fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // load fragment shader and compile it
    glShaderSource(fragmentShader, 1, &(shaders::custom_fragment_shader), NULL);
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
Sphere<T>::draw(float radius, std::array<float, 3> position,
                std::array<float, 3> rotation_axis, float angle,
                glm::vec4 color) {
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
    color = glm::vec4(1.0f, 0.5f, 0.2f, 0.3f);
    glUniform4fv(triangle_color, 1, glm::value_ptr(color));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glDrawElements(GL_TRIANGLES, element_array.size(), GL_UNSIGNED_INT, 0);

    color = glm::vec4(0.5f, 0.5f, 0.5f, 0.5f);
    glUniform4fv(triangle_color, 1, glm::value_ptr(color));

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // render as wireframe
    glDrawElements(GL_TRIANGLES, element_array.size(), GL_UNSIGNED_INT, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // render as filled triangles

    // col = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
    // glUniform4fv(triangle_color, 1, glm::value_ptr(col));
    // glDrawArrays(GL_LINE_STRIP, 0, 18);
}

template <RENDER_TYPE T>
template <RENDER_TYPE Q>
typename std::enable_if<Q == RENDER_TYPE::CUSTOM_COLOR>::type
Sphere<T>::draw(float radius, std::array<float, 3> position,
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glDrawElements(GL_TRIANGLES, element_array.size(), GL_UNSIGNED_INT, 0);

    color = glm::vec4(0.5f, 0.5f, 0.5f, 0.5f);
    glUniform4fv(triangle_color, 1, glm::value_ptr(color));

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // render as wireframe
    glDrawElements(GL_TRIANGLES, element_array.size(), GL_UNSIGNED_INT, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // render as filled triangles

    // col = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
    // glUniform4fv(triangle_color, 1, glm::value_ptr(col));
    // glDrawArrays(GL_LINE_STRIP, 0, 18);
}

template <RENDER_TYPE T> void Sphere<T>::refine() {
    generate_vertexes_helper_improved();
}

template <RENDER_TYPE T> float Sphere<T>::area() {
    float ar = 0;
    for (int tri = 0; tri < element_array.size() / 3; tri += 1) {
        int vert1_ind = element_array[3 * tri];
        int vert2_ind = element_array[3 * tri + 1];
        int vert3_ind = element_array[3 * tri + 2];

        __m128 vert1 = load_vertex(&vertexes[vert1_ind * 3]);
        __m128 vert2 = load_vertex(&vertexes[vert2_ind * 3]);
        __m128 vert3 = load_vertex(&vertexes[vert3_ind * 3]);

        float *v = (float *)&vert1;
        v = (float *)&vert2;
        v = (float *)&vert3;
        __m128 vec12 = _mm_sub_ps(vert1, vert2);
        __m128 vec23 = _mm_sub_ps(vert2, vert3);
        __m128 vec13 = _mm_sub_ps(vert1, vert3);

        float distance12 = std::sqrt(CalcDotProductSse(vec12, vec12));
        float distance23 = std::sqrt(CalcDotProductSse(vec23, vec23));
        float distance13 = std::sqrt(CalcDotProductSse(vec13, vec13));

        float s = (distance12 + distance23 + distance13) / 2.0;
        ar += std::sqrt(s * (s - distance12) * (s - distance13) *
                        (s - distance23));
    }

    return ar;
}

template <RENDER_TYPE T> float Sphere<T>::quality() {
    return std::fabs(area() - M_PI);
}

#define __SPHERE__
#endif
