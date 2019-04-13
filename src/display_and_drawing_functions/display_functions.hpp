
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#ifndef __glad_h_
#include <glad/glad.h>
#endif
#pragma once

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
#include "star3d.hpp"
#include "display_functions.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

void display();
void pause_screen();
void OnPlusPressed(GLFWwindow *window);
void OnMinusPressed(GLFWwindow *window);
void OnEnterPressed(GLFWwindow *window, unsigned &enter_count);
void OnClosePressed(GLFWwindow *window);

/**
 * @brief  This function is for initialization of Shape vector. It's not a
 * general function and you have to check the implementation to see which shapes
 * are implemented.
 * @param shapes Vector of pointers to Shape objects.
 */
template <typename T>
void initialize_shapes(std::vector<Shape<T> *> &shapes) {
    static_assert(std::is_same<float, T>::value ||
                      std::is_same<double, T>::value,
                  "Shapes can only be instantiated with floating point types: "
                  "float, double, long double!");
    shapes.emplace_back(new (Circle<T>));
}

template <typename T>
void display(std::vector<Shape<T> *> &shapes) {
    static_assert(std::is_same<float, T>::value ||
                      std::is_same<double, T>::value,
                  "Shapes can only be instantiated with floating point types: "
                  "float, double, long double!");
    Shader<RENDER_TYPE::UNIFORM_COLOR> shader;
    display_all_shapes(shapes, shader);
}

template <RENDER_TYPE T, typename U>
void display_all_shapes(std::vector<Shape<U> *> &shapes, Shader<T> &shader) {
    static_assert(std::is_same<float, U>::value ||
                      std::is_same<double, U>::value,
                  "Shapes can only be instantiated with floating point types: "
                  "float, double, long double!");
    for (Shape<U> *shape : shapes) {
        draw(*shape, shader, {0.2, 0.2, 0.2}, {0.4, 0.5, 0}, {0.5, 0.5, 0.5}, 0,
             {0.2, 0.4, 0.5, 0.7});
    }
}

template <RENDER_TYPE T, typename U>
void rotate_all_shapes(std::vector<Shape<U> *> &shapes, Shader<T> &shader) {
    std::array<float, 3> axis({1, 1, 1});
    static float fi = 0.0;
    for (Shape<U> *shape : shapes) {
        fi += 0.05;
        draw(*shape, shader, {0.2, 0.2, 0.2}, {0.4, 0.5, 0}, axis, fi,
             {0.2, 0.4, 0.5, 0.7});
    }
}

template <typename T>
void move_square(GLFWwindow *window, std::array<T, 4> &square,
                 std::array<T, 2> &pos) {

    static_assert(
        std::is_same<T, float>::value ||
        std::is_same<T, double>::value &
            "move_square function is defined only for floats and doubles");

    T &min_x = square[0];
    T &max_x = square[1];
    T &min_y = square[2];
    T &max_y = square[3];

    std::array<T, 2> cursor_pos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        glfwGetCursorPos(window, cursor_pos.data(), cursor_pos.data() + 1);

        T pos_x = cursor_pos[0];
        T pos_y = cursor_pos[1];
        bool move = true;

        if (pos_x > min_x && pos_x < max_x && pos_y > min_y && pos_y < max_y) {

            glfwGetCursorPos(window, pos.data(), pos.data() + 1);
            &min_x += pos[0] - cursor_pos[0];
            &max_x += pos[0] - cursor_pos[0];
            &min_y += pos[1] - cursor_pos[1];
            &max_y += pos[1] - cursor_pos[1];
        }
    }
}
