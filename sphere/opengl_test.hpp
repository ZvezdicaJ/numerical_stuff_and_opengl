
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <GLFW/glfw3.h>
#include <array>
#include <cmath>
#include <ctime>

#ifndef __glad_h_
#include <glad/glad.h>
#endif

#include "shaders.hpp"
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <xmmintrin.h>
#include "print_functions.hpp"
#include "sphere_generation.hpp"
#incldue <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#define M_PI 3.14159265358979323846 /* pi */

void draw_triangle();
bool check_shader_program(unsigned);
bool check_vertex_shader(unsigned);
bool check_fragment_shader(unsigned);
void pause();
void print(std::string);
void OnPlusPressed(GLFWwindow *);
void OnMinusPressed(GLFWwindow *);
void OnClosePressed(GLFWwindow *);
void draw_triangle_boundary();
void draw_double_pyramid(std::vector<float> &, float, float);
std::pair<std::vector<float>, std::vector<int>>
generate_sphere_mesh(std::array<float, 3>, float);
std::vector<int> generate_sphere_mesh(std::vector<int> &, std::vector<float> &,
                                      float);
void draw_sphere(std::array<float, 3> &, float, float);
