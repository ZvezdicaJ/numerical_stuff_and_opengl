#pragma once
// this header file has to be included before  shape.hpp and after
// shader_class.hpp

template <typename T>
class Shape;
template <typename T>
class Shape2D;

/**
 * @brief draw 2d and 3d shapes of uniform color.
 * @param shape An object of type class Shape to be
 * drawn
 * @param shader_object Object of type class Shader<RENDER_TYPE::UNIFORM_COLOR>,
 * which contains correct shader.
 * @param scale sscales object in x,y and z directions.
 * @param position translate center of the object to coordinates specified by
 * this vector.
 * @param rotation_axis axis around which object should be rotated.
 * @param angle Angle specifying how much to rotate the object.
 * @param color color of the shape - the same for all vertexes
 */
template <typename T>
void draw(Shape<T> &shape, Shader<RENDER_TYPE::UNIFORM_COLOR> &shader_object,
          std::array<float, 3> scale = {0.5, 0.5, 0.5},
          std::array<float, 3> position = {0, 0, 1},
          std::array<float, 3> rotation_axis = {0, 0, 1}, float angle = 0,
          glm::vec4 color = {0.5, 0.5, 0.5, 0.5}) {

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
    // color = glm::vec4(0.8f, 0.6f, 0.2f, 0.5f);
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
        if (shape.draw_type == 'V')
            glDrawArrays(GL_LINE_LOOP, 0,
                         shape.vertexes.size() / shape.vertex_size);

        else if (shape.draw_type == 'E') {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.EBO);
            glDrawElements(GL_LINES, shape.element_array.size(),
                           GL_UNSIGNED_INT, 0);
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

/**
 * @brief draw 2d and 3d shapes of custom color - each vertex has it's own
 * color.
 * @param shape An object of type class Shape to be
 * drawn
 * @param shader_object Object of type class Shader<RENDER_TYPE::UNIFORM_COLOR>,
 * which contains correct shader.
 * @param scale sscales object in x,y and z directions.
 * @param position translate center of the object to coordinates specified by
 * this vector.
 * @param rotation_axis axis around which object should be rotated.
 * @param angle Angle specifying how much to rotate the object.
 */
template <typename T>
void draw(Shape<T> &shape, Shader<RENDER_TYPE::CUSTOM_COLOR> &shader_object,
          std::array<float, 3> scale = {0.5, 0.5, 0.5},
          std::array<float, 3> position = {0, 0, 1},
          std::array<float, 3> rotation_axis = {0, 0, 1}, float angle = 0) {

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
        if (shape.draw_type == 'V')
            glDrawArrays(GL_LINE_LOOP, 0,
                         shape.vertexes.size() / shape.vertex_size);

        else if (shape.draw_type == 'E') {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.EBO);
            glDrawElements(GL_LINES, shape.element_array.size(),
                           GL_UNSIGNED_INT, 0);
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

/**
 * @brief draw 2d and 3d shapes frame (not full triangles), of uniform color.
 * @param shape An object of type class Shape to be
 * drawn
 * @param shader_object Object of type class Shader<RENDER_TYPE::UNIFORM_COLOR>,
 * which contains correct shader.
 * @param scale sscales object in x,y and z directions.
 * @param position translate center of the object to coordinates specified by
 * this vector.
 * @param rotation_axis axis around which object should be rotated.
 * @param angle Angle specifying how much to rotate the object.
 * @param color color of the shape - the same for all vertexes
 */
template <typename T>
void draw_wireframe(Shape<T> &shape,
                    Shader<RENDER_TYPE::UNIFORM_COLOR> &shader_object,
                    std::array<float, 3> scale = {0.5, 0.5, 0.5},
                    std::array<float, 3> position = {0, 0, 1},
                    std::array<float, 3> rotation_axis = {0, 0, 1},
                    float angle = 0, glm::vec4 color = {0.5, 0.5, 0.5, 0.5}) {

    if (shape.vertex_size != 3)
        return;
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

    unsigned int transformLoc =
        glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    unsigned int triangle_color = glGetUniformLocation(shaderProgram, "color");
    // color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.EBO);

    // std::cout << "number of elements drawn: "
    //          << (shape.element_array.size() - 15) / 3 << std::endl;
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // render as wireframe
    glDrawElements(GL_TRIANGLES, shape.element_array.size(), GL_UNSIGNED_INT,
                   0);
    glPolygonMode(
        GL_FRONT_AND_BACK,
        GL_FILL); // render as filled triangles
                  // color = glm::vec4(0.5f, 0.5f, 0.5f, 0.5f);
                  // glUniform4fv(triangle_color, 1, glm::value_ptr(color));

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // render as wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // render as filled triangles

    // col = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
    // glUniform4fv(triangle_color, 1, glm::value_ptr(col));
    // glDrawArrays(GL_LINE_STRIP, 0, 18);
}

/**
 * @brief draw 2d shape filling of uniform color.
 * @param shape An object of type class Shape to be
 * drawn
 * @param shader_object Object of type class Shader<RENDER_TYPE::UNIFORM_COLOR>,
 * which contains correct shader.
 * @param scale sscales object in x,y and z directions.
 * @param position translate center of the object to coordinates specified by
 * this vector.
 * @param rotation_axis axis around which object should be rotated.
 * @param angle Angle specifying how much to rotate the object.
 * @param color color of the shape - the same for all vertexes
 */
template <typename T>
void draw_2d_object(Shape2D<T> &shape,
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
    glUniform4fv(triangle_color, 1, glm::value_ptr(color));

    GLenum type;
    if (std::is_same<double, T>::value)
        type = GL_DOUBLE;
    else
        type = GL_FLOAT;

    static int ind = 0;
    if (ind == 0) {
        std::cout << "number of vertexes: "
                  << shape.vertexes.size() / shape.vertex_size << std::endl;

        for (int i = 0; i < shape.filling_elements.size(); i += 3) {
            std::cout << shape.filling_elements[i] << "  "
                      << shape.filling_elements[i + 1] << "  "
                      << shape.filling_elements[i + 2] << "     " << std::endl;
        }
        std::cout << std::endl;
        for (int j = 0; j < shape.vertexes.size(); j += 2) {
            std::cout << "(" << shape.vertexes[j] << " "
                      << shape.vertexes[j + 1] << ")   " << std::endl;
        }
        std::cout << "number of elements to draw: "
                  << shape.filling_elements.size() << std::endl;
        ind = 1;
    }

    if (shape.filling_draw_type == 'V') {

        glBindBuffer(GL_ARRAY_BUFFER, shape.FILLING_VBO);
        glBindVertexArray(shape.VAO);
        glVertexAttribPointer(0, shape.vertex_size, type, GL_TRUE,
                              shape.vertex_size * sizeof(T), (void *)0);
        glEnableVertexAttribArray(0);

        glDrawArrays(GL_TRIANGLES, 0,
                     shape.filling_vertexes.size() / shape.vertex_size);

    } else if (shape.filling_draw_type == 'E') {

        glBindBuffer(GL_ARRAY_BUFFER, shape.VBO);
        glBindVertexArray(shape.VAO);
        glVertexAttribPointer(0, shape.vertex_size, type, GL_TRUE,
                              shape.vertex_size * sizeof(T), (void *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.FILLING_EBO);

        glDrawElements(GL_TRIANGLES, shape.filling_elements.size(),
                       GL_UNSIGNED_INT, 0);
    }
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // render as filled triangles
}

/**
 * @brief draw 2d shape filling  of custom color.
 * @param shape An object of type class Shape to be
 * drawn
 * @param shader_object Object of type class Shader<RENDER_TYPE::UNIFORM_COLOR>,
 * which contains correct shader.
 * @param scale sscales object in x,y and z directions.
 * @param position translate center of the object to coordinates specified by
 * this vector.
 * @param rotation_axis axis around which object should be rotated.
 * @param angle Angle specifying how much to rotate the object.
 * @param color color of the shape - the same for all vertexes
 */
template <typename T>
void draw_2d_object(Shape2D<T> &shape,
                    Shader<RENDER_TYPE::CUSTOM_COLOR> &shader_object,
                    std::array<float, 3> scale = {0.5, 0.5, 0.5},
                    std::array<float, 3> position = {0, 0, 1},
                    std::array<float, 3> rotation_axis = {0, 0, 1},
                    float angle = 0) {

    assert(shape.colors_loaded == true &&
           "to draw 2d shape with custom colors, you have to load colors");

    // if (shape_.vertex_size == 2) {
    // std::cout << "draw_2d_object can display only 2d shapes";
    //    return;
    // }
    // Shape2D<T> shape = std::static_cast<Shape2D<T>>(shape_);
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

    unsigned int transformLoc =
        glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    GLenum type;
    if (std::is_same<double, T>::value)
        type = GL_DOUBLE;
    else
        type = GL_FLOAT;

    if (shape.filling_draw_type == 'V') {
        // if draw_type=='V' bind filling vertex buffer object
        glBindBuffer(GL_ARRAY_BUFFER, shape.FILLING_VBO);
        glBindVertexArray(shape.VAO);
        glVertexAttribPointer(0, shape.vertex_size, type, GL_TRUE,
                              shape.vertex_size * sizeof(T), (void *)0);
        glEnableVertexAttribArray(0);

        // then enable colors
        glBindBuffer(GL_ARRAY_BUFFER, shape.CBO);
        glBindVertexArray(shape.VAO);
        glVertexAttribPointer(1, 4, type, GL_TRUE, 4 * sizeof(float),
                              (void *)0);
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLES, 0,
                     shape.filling_vertexes.size() / shape.vertex_size);

    } else if (shape.filling_draw_type == 'E') {

        glBindBuffer(GL_ARRAY_BUFFER, shape.VBO);
        glBindVertexArray(shape.VAO);
        glVertexAttribPointer(0, shape.vertex_size, type, GL_TRUE,
                              shape.vertex_size * sizeof(T), (void *)0);
        glEnableVertexAttribArray(0);

        // enable colors
        glBindBuffer(GL_ARRAY_BUFFER, shape.CBO);
        glBindVertexArray(shape.VAO);
        glVertexAttribPointer(1, 4, type, GL_TRUE, 4 * sizeof(float),
                              (void *)0);
        glEnableVertexAttribArray(1);

        // if draw_type=='V' bind filling element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.FILLING_EBO);
        glDrawElements(GL_TRIANGLES, shape.filling_elements.size(),
                       GL_UNSIGNED_INT, 0);
    }
}
