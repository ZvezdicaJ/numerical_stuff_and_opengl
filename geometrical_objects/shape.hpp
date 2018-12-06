class Shape {
  protected:
    std::vector<float> vertexes;
    std::vector<int> element_array;
    unsigned shaderProgram;
    bool shaders_compiled = false;
    unsigned VBO;
    unsigned VAO;
    unsigned EBO;
    unsigned CBO; // color buffer object
    int min_vertexes = 100;
    virtual void initialize_buffers() {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexes.size(),
                     &vertexes[0], GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(float) * element_array.size(), &(element_array[0]),
                     GL_STATIC_DRAW);
    }

  public:
    virtual void set_min_number_of_vertexes(unsigned num) { min_vertexes = num; };
};
