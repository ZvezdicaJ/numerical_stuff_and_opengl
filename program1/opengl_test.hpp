#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shaders.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ctime>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp> 

# define M_PI 3.14159265358979323846  /* pi */

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
void draw_sphere(std::vector<float>&, float, float);
