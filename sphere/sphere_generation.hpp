class sphere {
  private:
    std::vector<float> vertexes;
    std::vector<int> element_array;
    bool vertexes_generated = false;

    unsigned shaderProgram;
    bool shaders_compiled = false;
    unsigned VBO;
    unsigned VAO;
    unsigned EBO;
    int min_vertexes = 300;

  public:
    // sphere() = default;
    sphere(sphere &&) = default;
    sphere &operator=(sphere &&) = default;
    sphere(const sphere &) = default;
    sphere &operator=(const sphere &) = default;
    sphere();
    sphere(std::array<float, 3> center, float radius);

    void draw(float radius = 0.5, std::array<float, 3> translate = {0,0,0},
              std::array<float, 3> rotation_axis = {0,0,1}, float angle = 0);
    void draw_wireframe(float radius, std::array<float,3> translate,
                        std::array<float,3> rotation_axis, float angle);
    void compile_shaders();
    void set_number_of_vertexes();

    void generate_sphere_mesh(std::array<float, 3> center = {0, 0, 0},
                              float radius = 1);
    // this one just helps the one found in public section
    void generate_sphere_mesh(float radius = 1);

    void initialize_buffers();
};