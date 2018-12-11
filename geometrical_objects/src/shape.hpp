
enum class RENDER_TYPE { UNIFORM_COLOR = 0, CUSTOM_COLOR = 1 };

template<RENDER_TYPE T>
class Shape {
  protected:
    std::vector<float> vertexes;
    std::vector<int> element_array;
    unsigned shaderProgram;
    bool shaders_compiled = false;
    bool colors_loaded = false;
    unsigned vertex_size;
    unsigned VBO;
    unsigned VAO;
    unsigned EBO;
    unsigned CBO; // color buffer object
    int min_vertexes = 5;
    std::vector<float> vertex_colors;
    void initialize_buffers() {
        glGenVertexArrays(1, &VAO);
        // glBindVertexArray(VAO);

        // generate and bind and fill vertex data
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexes.size(),
                     &vertexes[0], GL_STATIC_DRAW);

        // generate and bind and fill element data
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(float) * element_array.size(), &(element_array[0]),
                     GL_STATIC_DRAW);

        // generate color buffer but do not load data;
        // load data
        glGenBuffers(1, &CBO);
    }

  public:
    virtual void set_min_number_of_vertexes(unsigned num) {
        min_vertexes = num;
    };

    virtual void set_vertex_colors(std::vector<float> &colors_) {
        assert(colors_.size() / 4 == vertexes.size() / vertex_size &&
               "Each vertex should have a color value in the form of vec4: "
               "incorrect size of color vector!");
        vertex_colors = colors_;
    }

    std::vector<float> get_vertexes() { return vertexes; }
    unsigned num_vertexes() { return vertexes.size() / vertex_size; }
};
