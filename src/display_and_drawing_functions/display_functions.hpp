
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
