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
int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    print("Window created!");

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned size = 50;
    float step_size = 1.0 / (float)size;
    float ising_array[size * size][2];
    float initial_pos = -0.5 + step_size / 2.0;

    // i goes over x coordinate
    for (int i = 0; i < size; i++) {
        // j goes over x coordinate
        for (int j = 0; j < size; j++) {
            ising_array[i * size + j][0] = initial_pos + j * step_size;
            ising_array[i * size + j][1] = initial_pos + i * step_size;
            // std::cout << "array index:  " << i * size + j << std::endl;
        }
    }

    Shader<RENDER_TYPE::CUSTOM> shader(ising_frame_vertex_shaders,
                                       ising_frame_geometry_shader,
                                       ising_frame_fragment_shader);

    IsingModel<float> alg1(size);
    alg1.set_temperature(2.7);
    /*
    for (int i = 0; i < 1000; i++) {
        std::cout << "magnetization: " << alg1.calc_magnetization()
                  << "\ncluster size: " << alg1.get_cluster_size()
                  << "\nenergy: " << alg1.calc_energy() << std::endl;
        alg1.flip_cluster();
        std::cout << "\n\n" << std::endl;
    }
    alg1.set_temperature(5);
    std::cout << "\n*****temperature increased*****\n" << std::endl;
    for (int i = 0; i < 2000; i++) {
        alg1.flip_cluster();
        std::cout << "magnetization: " << alg1.calc_magnetization()
                  << "  cluster size: " << alg1.get_cluster_size() << "\n"
                  << std::endl;
    }
    alg1.metropolis_steps(1000);
    */
}
