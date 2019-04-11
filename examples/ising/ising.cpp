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
#include <thread>                            // std::this_thread::sleep_for
#include <chrono>

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

#include "display_functions.hpp"
#include "ising_shader.hpp"
#include "spin_array.hpp"
#include "text_rendering.hpp"

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(1280, 800, "Ising model", NULL, NULL);
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
    glDisable(GL_DITHER);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);

    Shader<RENDER_TYPE::CUSTOM> frame_shader(ising_frame_vertex_shaders,
                                             ising_frame_geometry_shader,
                                             ising_frame_fragment_shader);
    Shader<RENDER_TYPE::CUSTOM> triangle_shader(ising_triangle_vertex_shaders,
                                                ising_triangle_geometry_shader,
                                                ising_triangle_fragment_shader);
    unsigned size = 50;
    SpinArray<float> spin_array(size);
    IsingModel<float> alg1(size);

    aligned_vector<float> vert = spin_array.get_vertexes();
    alg1.set_temperature(2);

    Text ising_text("/usr/share/fonts/truetype/ubuntu/Ubuntu-M.ttf");

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    while (!glfwWindowShouldClose(window)) {
        OnMinusPressed(window);
        OnPlusPressed(window);
        OnClosePressed(window);
        glfwPollEvents();

        // draw_frame(frame_shader, spin_array, {0, 0, 0}, {3.0, 3.0, 3.0});
        draw_black_white(triangle_shader, spin_array, alg1, {0, 0, 0},
                         {2.5, 2.5, 2.5});
        ising_text.RenderText("Ising model", width * 0.45, height * 0.95, 1.0,
                              glm::vec3(1.0, 0, 0), window);
        ising_text.RenderText(
            "magnetization: " + std::to_string(alg1.calc_magnetization()),
            width * 0.3, height * 0.85, 1.0, glm::vec3(0.0, 0.0, 0), window);

        ising_text.RenderText("energy: " + std::to_string((int)alg1.calc_energy()),
                              width * 0.6, height * 0.85, 1.0,
                              glm::vec3(0.0, 0.0, 0), window);

        alg1.metropolis_steps(100);
        alg1.flip_cluster();
        glfwSwapBuffers(window);
        glClearColor(1.0f, 1.0f, 1.0f,
                     1.0f); // set which color to clear the screen with
        // GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT and
        // GL_STENCIL_BUFFER_BIT. set which buffer to use to clear the
        // screen
        glClear(GL_COLOR_BUFFER_BIT);
        //       std::cout << "magnetization: " << alg1.calc_magnetization()
        //          << "   energy: " << alg1.calc_energy() << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }

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
    */
}
