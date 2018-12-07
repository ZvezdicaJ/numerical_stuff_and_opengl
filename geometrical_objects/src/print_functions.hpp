inline void print_vertex(float *ver, const std::string vertex_name = "",
                         unsigned vert_size = 3) {
    std::cout << "\n " << vertex_name << "  ";
    for (int i = 0; i < vert_size; i++)
        std::cout << *(ver + i) << " ";
    std::cout << std::endl;
}

template <class T>
inline std::ostream &operator<<(std::ostream &out, std::vector<T> vec) {
    for (int i = 0; i < vec.size(); i++) {
        out << vec[i] << " ";
    }
    return out;
}

template <class T, int size>
inline void print_vertexes(const std::array<T, size> &vertexes,
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

inline void print(std::string str) { std::cout << str << std::endl; }

inline std::ostream &operator<<(std::ostream &out, std::pair<int, int> pair) {
    std::cout << "<" << pair.first << ", " << pair.second<<">";
    return out;
}

inline std::ostream &operator<<(std::ostream &out,
                         std::unordered_map<std::pair<int, int>, int> umap) {
    for (const auto &element : umap) {
        std::cout << element.first << "  " << element.second << std::endl;
    }
    return out;
}
