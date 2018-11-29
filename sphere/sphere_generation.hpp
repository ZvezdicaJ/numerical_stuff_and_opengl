class sphere {
   private:
    std::vector<float> vertexes;
    std::vector<int> element_array;
    bool vertexes_generated = false;

    unsigned shader_program;
    bool shaders_compiled = false;
    unsigned VBO;
    unsigned VAO;
    unsigned EBO;
    int min_vertexes = 300;

    // this one just helps the one found in public section
    std::vector<int> generate_sphere_mesh(float radius = 1);

   public:
    void draw(float radius, std::array<float>& translate,
              std::array<float>& rotation_axis, float angle);
    void draw_wireframe(float radius, std::array<float>& translate,
                        std::array<float>& rotation_axis, float angle);
    unsigned compile_shaders();
    void set_number_of_vertexes();

    void generate_sphere_mesh(std::array<float, 3> center = {0, 0, 0},
                              float radius = 1);
    void initialize_buffers();
}
