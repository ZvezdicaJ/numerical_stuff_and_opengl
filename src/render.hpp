
enum class RENDER_TYPE { UNIFORM_COLOR = 0, CUSTOM_COLOR = 1 };

template <RENDER_TYPE T> class Shape {
  protected:
   
    unsigned shaderProgram; /**< shader program for chosen render type */
    bool shaders_compiled =
        false; /**< true/false depending on whether the shaders were compiled*/
    bool colors_loaded =
        false; /**< indicator whether color have been loaded or not  */
    unsigned vertex_size =
        3; /**< vertex size can consist of 2, 3 or 4 points; this is important
              for correct interpretation of vertexes vector*/
    unsigned VBO; /**< vertex buffer object */
    unsigned VAO; /**< vertex array object*/
    unsigned EBO; /**< element buffer address */
    unsigned CBO; /**< color buffer address */
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
