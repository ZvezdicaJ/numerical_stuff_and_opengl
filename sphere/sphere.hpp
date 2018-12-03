enum class RENDER_TYPE { UNIFORM_COLOR, CUSTOM_COLOR };

template <RENDER_TYPE T>
class Sphere {
   private:
    std::vector<float> vertexes;
    std::vector<int> element_array;
    bool vertexes_generated = false;

    unsigned shaderProgram;
    bool shaders_compiled = false;
    unsigned VBO;
    unsigned VAO;
    unsigned EBO;

    unsigned CBO;  // color buffer object
    int min_vertexes = 400;
    RENDER_TYPE Rtype = T;

   public:
    // sphere() = default;
    Sphere(Sphere &&) = default;
    Sphere &operator=(Sphere &&) = default;
    Sphere(const Sphere &) = default;
    Sphere &operator=(const Sphere &) = default;
    Sphere();
    void draw_wireframe(float radius, std::array<float, 3> translate,
                        std::array<float, 3> rotation_axis, float angle);
    void compile_shaders();
    void set_min_number_of_vertexes(unsigned);

    void generate_sphere_mesh();
    // this one just helps the one found in public section
    void generate_sphere_mesh_helper();
    void generate_sphere_mesh_helper_improved();
    void initialize_buffers();

    template <RENDER_TYPE Q = T>
    typename std::enable_if<Q == RENDER_TYPE::UNIFORM_COLOR, void>::type draw(
        float radius = 0.5, std::array<float, 3> translate = {0, 0, 0},
        std::array<float, 3> rotation_axis = {0, 0, 1}, float angle = 0,
        glm::vec4 color = {0.5, 0.5, 0.5, 0.5});

  template <RENDER_TYPE Q = T>
    typename std::enable_if<Q == RENDER_TYPE::CUSTOM_COLOR, void>::type draw(
        float radius = 0.5, std::array<float, 3> translate = {0, 0, 0},
        std::array<float, 3> rotation_axis = {0, 0, 1}, float angle = 0);
};

template <class T>
inline void hash_combine(std::size_t &seed, const T &v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std {
template <typename S, typename T>
struct hash<pair<S, T>> {
    inline size_t operator()(const pair<S, T> &v) const {
        size_t seed = 0;
        ::hash_combine(seed, v.first);
        ::hash_combine(seed, v.second);
        return seed;
    }
};
}  // namespace std
