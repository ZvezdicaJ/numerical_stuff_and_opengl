// this header file has to be included before  shape.hpp and after
// shader_class.hpp

template <typename T>
class Shape;

template <typename T>
void draw(Shape<T> &shape, Shader<RENDER_TYPE::UNIFORM_COLOR> &shader_object,
          std::array<float, 3> scale = {0.5, 0.5, 0.5},
          std::array<float, 3> position = {0, 0, 1},
          std::array<float, 3> rotation_axis = {0, 0, 1}, float angle = 0,
          glm::vec4 color = {0.5, 0.5, 0.5, 0.5}) {

    glBindVertexArray(shape.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, shape.VBO);
    unsigned shaderProgram =
        shader_object.get_shader_program(shape.vertex_size - 2);
    glUseProgram(shaderProgram);

    std::vector<T> vert = shape.get_vertexes();
    print_vertexes(vert, vert.size() / 2, 2);

    glm::mat4 trans = glm::mat4(1.0);
    trans =
        glm::translate(trans, glm::vec3(position[0], position[1], position[2]));
    trans = glm::rotate(
        trans, (float)angle,
        glm::vec3(rotation_axis[0], rotation_axis[1], rotation_axis[2]));
    trans = glm::scale(trans, glm::vec3(scale[0], scale[1], scale[2]));

    // std::cout << glm::to_string(trans) << std::endl;

    unsigned int transformLoc =
        glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    unsigned int triangle_color = glGetUniformLocation(shaderProgram, "color");
    color = glm::vec4(1.0f, 0.5f, 0.2f, 0.3f);
    glUniform4fv(triangle_color, 1, glm::value_ptr(color));

    GLenum type;
    if (std::is_same<double, T>::value)
        type = GL_DOUBLE;
    else
        type = GL_FLOAT;

    glVertexAttribPointer(0, shape.vertex_size, type, GL_TRUE,
                          shape.vertex_size * sizeof(T), (void *)0);
    glEnableVertexAttribArray(0);

    if (shape.vertex_size == 3) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.EBO);
        glDrawElements(GL_TRIANGLES, shape.element_array.size(),
                       GL_UNSIGNED_INT, 0);
    } else if (shape.vertex_size == 2) {
        if (shape.draw_type == 'E') {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.EBO);
            glDrawElements(GL_LINE_STRIP, shape.element_array.size(),
                           GL_UNSIGNED_INT, 0);
        } else if (shape.draw_type == 'V') {
            std::cout << "draw_type: V" << std::endl;
            glDrawArrays(GL_LINE_STRIP, 0,
                         shape.vertexes.size() / shape.vertex_size);
        }
    }
    // color = glm::vec4(0.5f, 0.5f, 0.5f, 0.5f);
    // glUniform4fv(triangle_color, 1, glm::value_ptr(color));

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // render as wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // render as filled triangles

    // col = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
    // glUniform4fv(triangle_color, 1, glm::value_ptr(col));
    // glDrawArrays(GL_LINE_STRIP, 0, 18);
}

template <typename T>
void draw(Shape<T> &shape, Shader<RENDER_TYPE::CUSTOM_COLOR> &shader_object,
          std::array<float, 3> scale = {0.5, 0.5, 0.5},
          std::array<float, 3> position = {0, 0, 0},
          std::array<float, 3> rotation_axis = {0, 0, 1}, float angle = 0) {

    unsigned shaderProgram =
        shader_object.shader_program[shape.vertex_size - 2];
    glUseProgram(shaderProgram);
    glUseProgram(shaderProgram);
    glm::mat4 trans = glm::mat4(1.0);
    // make rotation by appropriate angle

    trans =
        glm::translate(trans, glm::vec3(position[0], position[1], position[2]));
    trans = glm::rotate(
        trans, (float)angle,
        glm::vec3(rotation_axis[0], rotation_axis[1], rotation_axis[2]));
    trans = glm::scale(trans, glm::vec3(scale[0], scale[1], scale[2]));

    // std::cout << glm::to_string(trans) << std::endl;

    unsigned int transformLoc =
        glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    unsigned int triangle_color = glGetUniformLocation(shaderProgram, "color");
    glm::vec4 color = glm::vec4(1.0f, 0.5f, 0.2f, 0.3f);
    glUniform4fv(triangle_color, 1, glm::value_ptr(color));

    glBindVertexArray(shape.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, shape.CBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (shape.vertex_colors.size()),
                 &(shape.vertex_colors[0]), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 4 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, shape.VBO);
    glVertexAttribPointer(0, shape.vertex_size, GL_FLOAT, GL_TRUE,
                          shape.vertex_size * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.EBO);
    glDrawElements(GL_LINE_STRIP, shape.element_array.size(), GL_UNSIGNED_INT,
                   0);
};

template <typename T>
void draw_wireframe(Shape<T> &shape,
                    Shader<RENDER_TYPE::UNIFORM_COLOR> &shader_object,
                    std::array<float, 3> scale = {0.5, 0.5, 0.5},
                    std::array<float, 3> position = {0, 0, 0},
                    std::array<float, 3> rotation_axis = {0, 0, 1},
                    float angle = 0, glm::vec4 color = {0.5, 0.5, 0.5, 0.5}) {
    unsigned shaderProgram =
        shader_object.shader_program[shape.vertex_size - 1];
    glBindVertexArray(shape.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, shape.VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.EBO);
    glUseProgram(shaderProgram);
    glm::mat4 trans = glm::mat4(1.0);
    trans =
        glm::translate(trans, glm::vec3(position[0], position[1], position[2]));
    trans = glm::rotate(
        trans, (float)angle,
        glm::vec3(rotation_axis[0], rotation_axis[1], rotation_axis[2]));
    trans = glm::scale(trans, glm::vec3(scale[0], scale[1], scale[2]));

    unsigned int transformLoc =
        glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    unsigned int triangle_color = glGetUniformLocation(shaderProgram, "color");

    glUniform4fv(triangle_color, 1, glm::value_ptr(color));
    glVertexAttribPointer(0, shape.vertex_size, GL_FLOAT, GL_TRUE,
                          shape.vertex_size * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // render as wireframe
    glDrawElements(GL_TRIANGLES, shape.element_array.size(), GL_UNSIGNED_INT,
                   0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // render as filled triangles
}