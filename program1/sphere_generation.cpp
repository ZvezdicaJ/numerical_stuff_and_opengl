#include "opengl_test.hpp"

std::vector<array<float, 3>> generate_sphere_mesh() {
    std::vector<float> vertexes;
    vertexes.reserve(15);
    std::array<float, 3> dir1({1, 0, 0});
    std::array<float, 3> dir2 = {std::cos(2.0 * M_PI / 3),
                                 std::sin(2.0 * M_PI / 3), 0};
    std::array<float, 3> dir3 = {std::cos(4.0 * M_PI / 3),
                                 std::sin(4.0 * M_PI / 3), 0};
    std::array<float, 3> z_dir = {0, 0, 1};
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
    std::vector<int> element_array(
        {0, 1, 3, 0, 1, 4, 0, 2, 3, 0, 2, 4, 1, 2, 3, 1, 2, 4});
    float triangles[6][3][3];
    for (int i = 0; i < 6; i++) {      // index i teče po trikotnikih
        for (int j = 0; j < 3; j++) {  // index j teče po ogljiščih
            triangles[i][j][0] = vertexes[(element_array[3 * i + j] - 1) * 3];
            triangles[i][j][1] = vertexes[element_array[i * 3 + j] * 3 + 1];
            triangles[i][j][2] = vertexes[element_array[i * 3 + j] * 3 + 2];
        }
    }
}

std::vector<array<float, 3>> generate_sphere_mesh(
    std::vector<int>& element_array, std::vector<float>& vertexes) {
    vertexes.reserve(vertexes.size() * 5 + 1);
    int num_tri = element_array.size() / 3;
    std::vector<int> new_element_array;
    new_element_array.reserve(4 * element_array.size() + 1);
    for (tri = 0; tri < num_tri; tri++) {
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
        // sse load
        __m128 vert1 = _mm_loadu_ps(&vertexes[element_array[3 * tri] * 3]);
        __m128 vert2 = _mm_loadu_ps(&vertexes[element_array[3 * tri + 1] * 3]);
        __m128 vert3 = _mm_loadu_ps(&vertexes[element_array[3 * tri + 2] * 3]);
        __m128 p12 = _mm_div_ps(_mm_add_ps(vert1, vert2),
                                _mm_set_ps(2.0, 2.0, 2.0, 2.0));
        __m128 p23 = _mm_div_ps(_mm_add_ps(vert2, vert3),
                                _mm_set_ps(2.0, 2.0, 2.0, 2.0));
        __m128 p13 = _mm_div_ps(_mm_add_ps(vert1, vert3),
                                _mm_set_ps(2.0, 2.0, 2.0, 2.0));
        int vertex_number = vertexes.size();
        vertexes.resize(vertexes.size() + 9);
        _mm_storeu_ps(&vertexes[vertexes.size()], p12);
        _mm_storeu_ps(&vertexes[vertexes.size() + 3], p23);
        _mm_storeu_ps(&vertexes[vertexes.size() + 6], p13);

        // set the correct vertex element numbers
        __m128i new_tri1 = _mm_set_epi32(vertex_number, vertex_number + 1,
                                         element_array[3 * tri + 1], -1);

        __m128i new_tri1 = _mm_set_epi32(vertex_number +1, vertex_number + 2,
                                         element_array[3 * tri + 2], -1);

        __m128i new_tri1 = _mm_set_epi32(vertex_number, vertex_number + 2,
                                         element_array[3 * tri], -1);
    }
}
