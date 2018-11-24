#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shaders.hpp"

bool check_shader_program(unsigned);
bool check_vertex_shader(unsigned);
bool check_fragment_shader(unsigned);
void pause();
void print(std::string);
void OnPlusPressed(GLFWwindow *);
void OnMinusPressed(GLFWwindow *);
void OnClosePressed(GLFWwindow *);

