
template <class T, int size>
void print_vertexes(const std::array<T, size> &vertexes,
                    int number_to_print = 0, int vertex_size = 3) {
    if (number_to_print == 0)
        number_to_print = size / vertex_size;

    for (int i = 0; i < number_to_print; i++) {
        std::cout << "vertex " << i << ":   ";
        for (int j = 0; j < vertex_size; j++) {
            std::cout << vertexes[vertex_size * i + j] << "  ";
        }
        std::cout << std::endl;
    }
}

inline void print_vertexes(float *vertexes, int number_to_print,
                           int vertex_size = 3) {

    for (int i = 0; i < number_to_print; i++) {
        std::cout << "vertex " << i << ":   ";
        for (int j = 0; j < vertex_size; j++) {
            std::cout << vertexes[vertex_size * i + j] << "  ";
        }
        std::cout << std::endl;
    }
}

template <class T, int size>
void print_triangles(const std::array<std::array<T, 9>, size> &triangles,
                     int number_to_print = size, int vertex_size = 3) {
    if (number_to_print == 0)
        number_to_print = triangles.size();

    for (int i = 0; i < number_to_print; i++) {
        std::cout << "triangle " << i << std::endl;
        print_vertexes<float, 9>(triangles[i], 3, vertex_size);
    }
}

inline void print_triangles(float *triangles, int number_to_print,
                            int vertex_size = 3) {

    for (int tri = 0; tri < number_to_print; tri++) {
        std::cout << "triangle " << tri << ":   ";
        for (int ver = 0; ver < 3; ver++) {
            unsigned ofset = 3 * vertex_size * tri + 3 * ver;
            std::cout << triangles[ofset] << " " << triangles[ofset + 1] << " "
                      << triangles[ofset + 2] << "  ";
        }
        std::cout << std::endl;
    }
}
