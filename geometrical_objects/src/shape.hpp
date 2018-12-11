#ifndef __SHAPE__

enum class RENDER_TYPE { UNIFORM_COLOR = 0, CUSTOM_COLOR = 1 };

template <RENDER_TYPE T> class Shape {
  protected:
    RENDER_TYPE Rtype = T;
    std::vector<float> vertexes;
    std::vector<int> element_array;
    unsigned shaderProgram;
    bool shaders_compiled = false;
    bool colors_loaded = false;
    unsigned vertex_size = 3;
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

    template <RENDER_TYPE Q = T>
    typename std::enable_if<Q == RENDER_TYPE::UNIFORM_COLOR>::type
    draw(std::array<float, 3> scale = {0.5, 0.5, 0.5},
         std::array<float, 3> position = {0, 0, 0},
         std::array<float, 3> rotation_axis = {0, 0, 1}, float angle = 0,
         glm::vec4 color = {0.5, 0.5, 0.5, 0.5}) {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glUseProgram(shaderProgram);

        glm::mat4 trans = glm::mat4(1.0);
        trans = glm::translate(
            trans, glm::vec3(position[0], position[1], position[2]));
        trans = glm::rotate(
            trans, (float)angle,
            glm::vec3(rotation_axis[0], rotation_axis[1], rotation_axis[2]));
        trans = glm::scale(trans, glm::vec3(scale[0], scale[1], scale[2]));

        // std::cout << glm::to_string(trans) << std::endl;

        unsigned int transformLoc =
            glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        unsigned int triangle_color =
            glGetUniformLocation(shaderProgram, "color");
        color = glm::vec4(1.0f, 0.5f, 0.2f, 0.3f);
        glUniform4fv(triangle_color, 1, glm::value_ptr(color));

        glVertexAttribPointer(0, vertex_size, GL_FLOAT, GL_TRUE,
                              vertex_size * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glDrawElements(GL_TRIANGLES, element_array.size(), GL_UNSIGNED_INT, 0);

        color = glm::vec4(0.5f, 0.5f, 0.5f, 0.5f);
        glUniform4fv(triangle_color, 1, glm::value_ptr(color));

        // std::cout<<vertexes<<std::endl;
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // render as wireframe
        glDrawElements(GL_TRIANGLES, element_array.size(), GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // render as filled triangles

        // col = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
        // glUniform4fv(triangle_color, 1, glm::value_ptr(col));
        // glDrawArrays(GL_LINE_STRIP, 0, 18);
    }

    template <RENDER_TYPE Q = T>
    typename std::enable_if<Q == RENDER_TYPE::CUSTOM_COLOR>::type
    draw(std::array<float, 3> scale, std::array<float, 3> position = {0, 0, 0},
         std::array<float, 3> rotation_axis = {0, 0, 1}, float angle = 0) {

        glUseProgram(shaderProgram);
        glm::mat4 trans = glm::mat4(1.0);
        // make rotation by appropriate angle

        trans = glm::translate(
            trans, glm::vec3(position[0], position[1], position[2]));
        trans = glm::rotate(
            trans, (float)angle,
            glm::vec3(rotation_axis[0], rotation_axis[1], rotation_axis[2]));
        trans = glm::scale(trans, glm::vec3(scale[0], scale[1], scale[2]));

        // std::cout << glm::to_string(trans) << std::endl;

        unsigned int transformLoc =
            glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        unsigned int triangle_color =
            glGetUniformLocation(shaderProgram, "color");
        glm::vec4 color = glm::vec4(1.0f, 0.5f, 0.2f, 0.3f);
        glUniform4fv(triangle_color, 1, glm::value_ptr(color));

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, CBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (vertex_colors.size()),
                     &(vertex_colors[0]), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 4 * sizeof(float),
                              (void *)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, vertex_size, GL_FLOAT, GL_TRUE,
                              vertex_size * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, element_array.size(), GL_UNSIGNED_INT, 0);

        color = glm::vec4(0.5f, 0.5f, 0.5f, 0.5f);
        glUniform4fv(triangle_color, 1, glm::value_ptr(color));

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // render as wireframe
        glDrawElements(GL_TRIANGLES, element_array.size(), GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // render as filled triangles

        // col = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
        // glUniform4fv(triangle_color, 1, glm::value_ptr(col));
        // glDrawArrays(GL_LINE_STRIP, 0, 18);
    }

    template <RENDER_TYPE Q = T>
    typename std::enable_if<Q == RENDER_TYPE::UNIFORM_COLOR, void>::type
    compile_shaders() {
        // create empty shader
        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);

        // load  vertex  shader and compile it
        glShaderSource(vertexShader, 1,
                       (shaders::uniform_vertex_shaders[vertex_size].c_str()), NULL);
        glCompileShader(vertexShader);
        // check if successfully compiled
        check_vertex_shader(vertexShader);

        // create empty fragment shader
        unsigned int fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        // load fragment shader and compile it
        glShaderSource(fragmentShader, 1, &(shaders::uniform_fragment_shader),
                       NULL);
        glCompileShader(fragmentShader);
        check_fragment_shader(fragmentShader);

        this->shaderProgram = glCreateProgram();
        glAttachShader(this->shaderProgram, vertexShader);
        glAttachShader(this->shaderProgram, fragmentShader);
        glLinkProgram(this->shaderProgram);
        check_shader_program(this->shaderProgram);
        glUseProgram(this->shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    template <RENDER_TYPE Q = T>
    typename std::enable_if<Q == RENDER_TYPE::CUSTOM_COLOR, void>::type
    compile_shaders() {
        // create empty shader
        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);

        // load  vertex  shader and compile it
        glShaderSource(vertexShader, 1,
                       (shaders::custom_vertex_shaders[vertex_size].c_str()), NULL);
        glCompileShader(vertexShader);
        // check if successfully compiled
        check_vertex_shader(vertexShader);

        // create empty fragment shader
        unsigned int fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        // load fragment shader and compile it
        glShaderSource(fragmentShader, 1, &(shaders::custom_fragment_shader),
                       NULL);
        glCompileShader(fragmentShader);
        check_fragment_shader(fragmentShader);

        this->shaderProgram = glCreateProgram();
        glAttachShader(this->shaderProgram, vertexShader);
        glAttachShader(this->shaderProgram, fragmentShader);
        glLinkProgram(this->shaderProgram);
        check_shader_program(this->shaderProgram);
        glUseProgram(this->shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
};

#define __SHAPE__
#endif
