// this header file has to be included before  shape.hpp and after
// shader_class.hpp

template <typename T> class Shape;

template <typename T>
void draw(Shape<T> &shape, Shader<RENDER_TYPE::UNIFORM_COLOR> &shader_object,
          std::array<float, 3> scale = {0.5, 0.5, 0.5},
          std::array<float, 3> position = {0, 0, 1},
          std::array<float, 3> rotation_axis = {0, 0, 1}, float angle = 0,
          glm::vec4 color = {0.5, 0.5, 0.5, 0.5}) {

    unsigned shaderProgram =
        shader_object.get_shader_program(shape.vertex_size - 2);
    glUseProgram(shaderProgram);

    // std::vector<T> vert = shape.get_vertexes();
    // print_vertexes(vert, vert.size() / 2, 2);
    // std::cout << "vertex_size: " << shape.vertex_size << std::endl;
    // std::cout << "shader version: " << shape.vertex_size - 2 << std::endl;
    // std::cout << "draw buffers:\nVBO: " << shape.VBO << "\nVAO: " <<
    // shape.VAO
    //          << std::endl;

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
    color = glm::vec4(0.8f, 0.6f, 0.2f, 0.5f);
    glUniform4fv(triangle_color, 1, glm::value_ptr(color));

    GLenum type;
    if (std::is_same<double, T>::value)
        type = GL_DOUBLE;
    else
        type = GL_FLOAT;

    glBindBuffer(GL_ARRAY_BUFFER, shape.VBO);
    glBindVertexArray(shape.VAO);
    glVertexAttribPointer(0, shape.vertex_size, type, GL_TRUE,
                          shape.vertex_size * sizeof(T), (void *)0);
    glEnableVertexAttribArray(0);

    if (shape.vertex_size == 3) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.EBO);
        glDrawElements(GL_TRIANGLES, shape.element_array.size(),
                       GL_UNSIGNED_INT, 0);
    } else if (shape.vertex_size == 2) {
        glDrawArrays(GL_LINE_LOOP, 0,
                     shape.vertexes.size() / shape.vertex_size);
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
          std::array<float, 3> position = {0, 0, 1},
          std::array<float, 3> rotation_axis = {0, 0, 1}, float angle = 0) {

    unsigned shaderProgram =
        shader_object.get_shader_program(shape.vertex_size - 2);
    glUseProgram(shaderProgram);

    // std::vector<T> vert = shape.get_vertexes();
    // print_vertexes(vert, vert.size() / 2, 2);
    // std::cout << "vertex_size: " << shape.vertex_size << std::endl;
    // std::cout << "shader version: " << shape.vertex_size - 2 << std::endl;
    // std::cout << "draw buffers:\nVBO: " << shape.VBO << "\nVAO: " <<
    // shape.VAO
    //          << std::endl;

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

    GLenum type;
    if (std::is_same<double, T>::value)
        type = GL_DOUBLE;
    else
        type = GL_FLOAT;

    glBindBuffer(GL_ARRAY_BUFFER, shape.VBO);
    glBindVertexArray(shape.VAO);
    glVertexAttribPointer(0, shape.vertex_size, type, GL_TRUE,
                          shape.vertex_size * sizeof(T), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, shape.CBO);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 4 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(1);

    if (shape.vertex_size == 3) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.EBO);
        glDrawElements(GL_TRIANGLES, shape.element_array.size(),
                       GL_UNSIGNED_INT, 0);
    } else if (shape.vertex_size == 2) {
        glDrawArrays(GL_LINE_LOOP, 0,
                     shape.vertexes.size() / shape.vertex_size);
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
void draw_wireframe(Shape<T> &shape,
                    Shader<RENDER_TYPE::UNIFORM_COLOR> &shader_object,
                    std::array<float, 3> scale = {0.5, 0.5, 0.5},
                    std::array<float, 3> position = {0, 0, 1},
                    std::array<float, 3> rotation_axis = {0, 0, 1},
                    float angle = 0, glm::vec4 color = {0.5, 0.5, 0.5, 0.5}) {

    unsigned shaderProgram =
        shader_object.get_shader_program(shape.vertex_size - 2);
    glUseProgram(shaderProgram);

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
    color = glm::vec4(0.8f, 0.6f, 0.2f, 0.5f);
    glUniform4fv(triangle_color, 1, glm::value_ptr(color));

    GLenum type;
    if (std::is_same<double, T>::value)
        type = GL_DOUBLE;
    else
        type = GL_FLOAT;

    glBindBuffer(GL_ARRAY_BUFFER, shape.VBO);
    glBindVertexArray(shape.VAO);
    glVertexAttribPointer(0, shape.vertex_size, type, GL_TRUE,
                          shape.vertex_size * sizeof(T), (void *)0);
    glEnableVertexAttribArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // render as wireframe
    if (shape.vertex_size == 3) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.EBO);
        glDrawElements(GL_TRIANGLES, shape.element_array.size(),
                       GL_UNSIGNED_INT, 0);
    } else if (shape.vertex_size == 2) {
        glDrawArrays(GL_LINE_LOOP, 0,
                     shape.vertexes.size() / shape.vertex_size);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // render as filled triangles
}

template <typename T>
void draw_2d_object(Shape<T> &shape,
                    Shader<RENDER_TYPE::UNIFORM_COLOR> &shader_object,
                    std::array<float, 3> scale = {0.5, 0.5, 0.5},
                    std::array<float, 3> position = {0, 0, 1},
                    std::array<float, 3> rotation_axis = {0, 0, 1},
                    float angle = 0, glm::vec4 color = {0.5, 0.5, 0.5, 0.5}) {
    if (shape.vertex_size == 2) {
        // std::cout << "draw_2d_object can display only 2d shapes";
        return;
    }
    //    assert(shape.vertex_size == 2 && "draw_polygon is made only for 2d
    //    shapes");
    unsigned shaderProgram =
        shader_object.get_shader_program(shape.vertex_size - 2);
    glUseProgram(shaderProgram);

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
    color = glm::vec4(0.8f, 0.6f, 0.2f, 0.5f);
    glUniform4fv(triangle_color, 1, glm::value_ptr(color));

    GLenum type;
    if (std::is_same<double, T>::value)
        type = GL_DOUBLE;
    else
        type = GL_FLOAT;

    glBindBuffer(GL_ARRAY_BUFFER, shape.FILLING_VBO);
    glBindVertexArray(shape.VAO);
    glVertexAttribPointer(0, shape.vertex_size, type, GL_TRUE,
                          shape.vertex_size * sizeof(T), (void *)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_POLYGON, 0,
                 shape.filling_vertexes.size() / shape.vertex_size);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // render as filled triangles
}

// TEST THIS
template <typename T, RENDER_TYPE COLOR_TYPE>
void draw(Shape<T> &shape, Shader<COLOR_TYPE> &shader_object,
          std::array<float, 3> scale = {0.5, 0.5, 0.5},
          std::array<float, 3> position = {0, 0, 1},
          std::array<float, 3> rotation_axis = {0, 0, 1}, float angle = 0,
          glm::vec4 color = {0.5, 0.5, 0.5, 0.5}) {

    unsigned shaderProgram =
        shader_object.get_shader_program(shape.vertex_size - 2);
    glUseProgram(shaderProgram);

    // std::vector<T> vert = shape.get_vertexes();
    // print_vertexes(vert, vert.size() / 2, 2);
    // std::cout << "vertex_size: " << shape.vertex_size << std::endl;
    // std::cout << "shader version: " << shape.vertex_size - 2 << std::endl;
    // std::cout << "draw buffers:\nVBO: " << shape.VBO << "\nVAO: " <<
    // shape.VAO
    //          << std::endl;

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

    GLenum type;
    if (std::is_same<double, T>::value)
        type = GL_DOUBLE;
    else
        type = GL_FLOAT;
    if (RENDER_TYPE::UNIFORM_COLOR == COLOR_TYPE) {
        unsigned int triangle_color =
            glGetUniformLocation(shaderProgram, "color");
        color = glm::vec4(0.8f, 0.6f, 0.2f, 0.5f);
        glUniform4fv(triangle_color, 1, glm::value_ptr(color));
    }

    glBindBuffer(GL_ARRAY_BUFFER, shape.VBO);
    glBindVertexArray(shape.VAO);
    glVertexAttribPointer(0, shape.vertex_size, type, GL_TRUE,
                          shape.vertex_size * sizeof(T), (void *)0);
    glEnableVertexAttribArray(0);

    if (RENDER_TYPE::CUSTOM_COLOR == COLOR_TYPE) {
        glBindBuffer(GL_ARRAY_BUFFER, shape.CBO);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 4 * sizeof(float),
                              (void *)0);
        glEnableVertexAttribArray(1);
    }

    if (shape.vertex_size == 3) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.EBO);
        glDrawElements(GL_TRIANGLES, shape.element_array.size(),
                       GL_UNSIGNED_INT, 0);
    } else if (shape.vertex_size == 2) {
        glDrawArrays(GL_LINE_LOOP, 0,
                     shape.vertexes.size() / shape.vertex_size);
    }
    // color = glm::vec4(0.5f, 0.5f, 0.5f, 0.5f);
    // glUniform4fv(triangle_color, 1, glm::value_ptr(color));

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // render as wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // render as filled triangles

    // col = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
    // glUniform4fv(triangle_color, 1, glm::value_ptr(col));
    // glDrawArrays(GL_LINE_STRIP, 0, 18);
}
