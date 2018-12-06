class Shape {
   private:
    std::vector<float> vertexes;
    std::vector<int> element_array;

    unsigned shaderProgram;
    bool shaders_compiled = false;
    unsigned VBO;
    unsigned VAO;
    unsigned EBO;
    unsigned CBO;  // color buffer object
    int min_vertexes;

public:
  float area() = 0;
  virtual void initialize_buffers();
  virtual void set_min_number_of_vertexes(unsigned);
  virtual float area();
  virtual float quality();
}
