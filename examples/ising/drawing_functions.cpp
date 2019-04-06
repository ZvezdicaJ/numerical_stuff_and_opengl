#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#ifndef __glad_h_
#include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>
#include <xmmintrin.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <ctime>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <random>
#include <list>
#include <functional>
#include <boost/align/aligned_allocator.hpp> // this is for aligned std::vector

#include "apex_memmove.h"
#include "type_definitions.hpp"
#include "convex_hull.hpp"

#include "shaders.hpp"
#include "shader_class.hpp"
#include "auxiliary_functions.hpp"
#include "print_functions.hpp"
#include "drawing_functions.hpp"
#include "shape.hpp"
#include "sphere.hpp"
#include "circle.hpp"
#include "rectangle.hpp"
#include "star.hpp"
#include "box.hpp"
#include "disk.hpp"
#include "square_board.hpp"
#include "display_functions.hpp"
#include "square_board.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "Ising_model.hpp"
#include "ising_shader.hpp"

class spin_array {

  private:
    unsigned VBO, VAO, EBO;

  public:
}

void draw_frame(Shader<RENDER_TYPE::CUSTOM> shader, float *points,
                std::vector<spin_dir> spin_array, unsigned array_size) {

    unsigned shaderProgram = shader_object.get_shader_program;
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

    unsigned int square_size_loc =
        glGetUniformLocation(shaderProgram, "square_size");
    glUniform1fv(square_size_loc, 1, square_size;

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
