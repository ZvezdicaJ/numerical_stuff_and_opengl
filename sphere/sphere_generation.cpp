#include "opengl_test.hpp"

sphere::sphere() {
    generate_sphere_mesh({0, 0, 0}, 1.0);
    initialize_buffers();
    compile_shaders();
}

sphere::sphere(std::array<float, 3> center, float radius) {
    generate_sphere_mesh(center, radius);
    initialize_buffers();
    compile_shaders();
}

// static inline
float CalcDotProductSse(__m128 x, __m128 y) {
    __m128 mulRes, shufReg, sumsReg;
    mulRes = _mm_mul_ps(x, y);

    // Calculates the sum of SSE Register -
    // https://stackoverflow.com/a/35270026/195787
    shufReg = _mm_movehdup_ps(mulRes); // Broadcast elements 3,1 to 2,0
    sumsReg = _mm_add_ps(mulRes, shufReg);
    shufReg = _mm_movehl_ps(shufReg, sumsReg); // High Half -> Low Half
    sumsReg = _mm_add_ss(sumsReg, shufReg);
    return _mm_cvtss_f32(
        sumsReg); // Result in the lower part of the SSE Register
}

void sphere::generate_sphere_mesh(std::array<float, 3> center, float radius) {
    vertexes.reserve(15);
    vertexes.resize(15);
    std::array<float, 3> dir1({radius, 0, 0});
    std::array<float, 3> dir2 = {radius * std::cos(2.0f * M_PI / 3.0f),
                                 radius * std::sin(2.0f * M_PI / 3.0f), 0};
    std::array<float, 3> dir3 = {radius * std::cos(4.0f * M_PI / 3.0f),
                                 radius * std::sin(4.0f * M_PI / 3.0f), 0};
    std::array<float, 3> z_dir = {0, 0, radius};
    // vertex 1
    vertexes[0] = center[0] + dir1[0];
    vertexes[1] = center[1] + dir1[1];
    vertexes[2] = center[2] + dir1[2];
    // vertex 2
    vertexes[3] = center[0] + dir2[0];
    vertexes[4] = center[1] + dir2[1];
    vertexes[5] = center[2] + dir2[2];
    // vertex 3
    vertexes[6] = center[0] + dir3[0];
    vertexes[7] = center[1] + dir3[1];
    vertexes[8] = center[2] + dir3[2];
    // vertex 4
    vertexes[9] = center[0] + z_dir[0];
    vertexes[10] = center[1] + z_dir[1];
    vertexes[11] = center[2] + z_dir[2];
    // vertex 5
    vertexes[12] = center[0] - z_dir[0];
    vertexes[13] = center[1] - z_dir[1];
    vertexes[14] = center[2] - z_dir[2];

    // definition of triangles we will later divide to create very fine mesh for
    // out sphere print_vertexes<float, 15>(vertexes);
    element_array = std::vector<int>(
        {0, 1, 3, 0, 1, 4, 0, 2, 3, 0, 2, 4, 1, 2, 3, 1, 2, 4});
    float triangles[6][3][3];
    for (int i = 0; i < 6; i++) {     // index i teče po trikotnikih
        for (int j = 0; j < 3; j++) { // index j teče po ogljiščih
            triangles[i][j][0] = vertexes[(element_array[3 * i + j] - 1) * 3];
            triangles[i][j][1] = vertexes[element_array[i * 3 + j] * 3 + 1];
            triangles[i][j][2] = vertexes[element_array[i * 3 + j] * 3 + 2];
        }
    }
    //    std::cout << "radius" << radius << std::endl;
    generate_sphere_mesh_improved(radius);
}

void sphere::generate_sphere_mesh(float radius) {
    int vertex_number = vertexes.size() / 3;
    vertexes.reserve(vertexes.size() * 5 + 1);
    int num_tri = element_array.size() / 3;
    std::vector<int> new_element_array; // = element_array;
    new_element_array.reserve(4 * element_array.size() + 1);

    for (int tri = 0; tri < num_tri; tri++) {
        /* disabled because below is sse version
        T vertex1[3] = {vertexes[element_array[3 * tri] * 3],
                          vertexes[element_array[3 * tri] * 3 + 1],
                          vertexes[element_array[3 * tri] * 3 + 2]};

          T vertex2[3] = {vertexes[element_array[3 * tri + 1] * 3],
                          vertexes[element_array[3 * tri + 1] * 3 + 1],
                          vertexes[element_array[3 * tri + 1] * 3 + 2]};

          T vertex3[3] = {vertexes[element_array[3 * tri + 2] * 3],
                          vertexes[element_array[3 * tri + 2] * 3 + 1],
                          vertexes[element_array[3 * tri + 2] * 3 + 2]};
        */
        // sse load                              vertex number
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

        float norm12 = std::sqrt(CalcDotProductSse(p12, p12)) / radius;
        float norm23 = std::sqrt(CalcDotProductSse(p23, p23)) / radius;
        float norm13 = std::sqrt(CalcDotProductSse(p13, p13)) / radius;
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
        generate_sphere_mesh(radius);
}

void sphere::compile_shaders() {
    // create empty shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // load  vertex  shader and compile it
    glShaderSource(vertexShader, 1, &(shaders::test_rotate_shader), NULL);
    glCompileShader(vertexShader);
    // check if successfully compiled
    check_vertex_shader(vertexShader);

    // create empty fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // load fragment shader and compile it
    glShaderSource(fragmentShader, 1, &(shaders::test_fragment_shader), NULL);
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

void sphere::initialize_buffers() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexes.size(), &vertexes[0],
                 GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * element_array.size(),
                 &(element_array[0]), GL_STATIC_DRAW);
}

void sphere::draw(float radius, std::array<float, 3> translate,
                  std::array<float, 3> rotation_axis, float angle) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glUseProgram(shaderProgram);

    glm::mat4 trans = glm::mat4(1.0);
    // make rotation by appropriate angle

    trans = glm::translate(trans,
                           glm::vec3(translate[0], translate[1], translate[2]));
    trans = glm::rotate(
        trans, (float)angle,
        glm::vec3(rotation_axis[0], rotation_axis[1], rotation_axis[2]));
    trans = glm::scale(trans, glm::vec3(radius, radius, radius));

    // std::cout << glm::to_string(trans) << std::endl;

    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "rotate");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    unsigned int triangle_color = glGetUniformLocation(shaderProgram, "color");
    glm::vec4 col(1.0f, 0.5f, 0.2f, 0.3f);
    glUniform4fv(triangle_color, 1, glm::value_ptr(col));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glDrawElements(GL_TRIANGLES, element_array.size(), GL_UNSIGNED_INT, 0);

    col = glm::vec4(1.0f, 0.0f, 0.0f, 0.5f);
    glUniform4fv(triangle_color, 1, glm::value_ptr(col));
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // render as wireframe
    glDrawElements(GL_TRIANGLES, element_array.size(), GL_UNSIGNED_INT, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // render as wireframe
    // col = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
    // glUniform4fv(triangle_color, 1, glm::value_ptr(col));
    // glDrawArrays(GL_LINE_STRIP, 0, 18);
}

void sphere::generate_sphere_mesh_improved(float radius) {
    int vertex_number = vertexes.size() / 3;
    vertexes.reserve(vertexes.size() * 5 + 1);
    int num_tri = element_array.size() / 3;
    std::vector<int> new_element_array; // = element_array;
    new_element_array.reserve(4 * element_array.size() + 1);
    std::unordered_map<std::pair<int, int>, int> new_vertex_indexing;
    new_vertex_indexing.reserve(vertex_number + 1);
    //print_vertexes(&vertexes[0], vertex_number);
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

        float norm12 = std::sqrt(CalcDotProductSse(p12, p12)) / radius;
        float norm23 = std::sqrt(CalcDotProductSse(p23, p23)) / radius;
        float norm13 = std::sqrt(CalcDotProductSse(p13, p13)) / radius;

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
        //std::cout << "new element array:  \n" << new_element_array << std::endl;
        //print_vertexes(&vertexes[0], vertex_number);
    }
    element_array = new_element_array;
    if (vertexes.size() <  min_vertexes)
        generate_sphere_mesh_improved(radius);
}
