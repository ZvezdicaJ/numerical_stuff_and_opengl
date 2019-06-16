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
#include <thread> // std::this_thread::sleep_for
#include <chrono>

#include "apex_memmove.h"
#include "type_definitions.hpp"
#include "convex_hull.hpp"

#include "shaders.hpp"
#include "shader_class.hpp"
#include "auxiliary_functions.hpp"
#include "print_functions.hpp"
#include "drawing_functions.hpp"
#include "display_functions.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <mgl2/mgl.h>

#include "display_functions.hpp"
#include "ising_shader.hpp"
#include "spin_array.hpp"
#include "text_rendering.hpp"

#include "ising_windows.hpp"
#include "nanogui/nanogui.h"
#include "callback_functions.hpp"

nanogui::Screen *screen;

int main() {

    glfwInit();
    const char *glsl_version = "#version 450";

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window =
        glfwCreateWindow(1280, 800, "Ising model", NULL, NULL);
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

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSwapInterval(0);
    glfwSwapBuffers(window);

    Shader<RENDER_TYPE::CUSTOM> frame_shader(
        ising_frame_vertex_shaders, ising_frame_geometry_shader,
        ising_frame_fragment_shader);
    Shader<RENDER_TYPE::CUSTOM> triangle_shader(
        ising_triangle_vertex_shaders,
        ising_triangle_geometry_shader,
        ising_triangle_fragment_shader);
    unsigned size = 50;
    glm::vec3 starting_pos = {0.37, 0.1, 0.0};
    SpinArray<float> spin_array(size, starting_pos);
    spin_array.set_clickable_square(window);
    IsingModel<float> alg1(size);
    static char algorithm_choice = 'M';
    aligned_vector<float> vert = spin_array.get_vertexes();
    alg1.set_temperature(2.2);

    Text ising_text(
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-M.ttf");

    std::array<float, 2> ising_pos;
    std::vector<float> energy, magnetization;

    // initialize nanogui screen
    screen = new nanogui::Screen();
    screen->initialize(window, true);

    bool enabled = true;

    settings_window(screen, window, &alg1, &algorithm_choice,
                    energy, magnetization);

    screen->setVisible(true);
    screen->performLayout();
    // nanoguiWindow->center();

    glfwSetCursorPosCallback(
        window, [](GLFWwindow *, double x, double y) {
            screen->cursorPosCallbackEvent(x, y);
        });

    glfwSetMouseButtonCallback(window, [](GLFWwindow *, int button,
                                          int action,
                                          int modifiers) {
        screen->mouseButtonCallbackEvent(button, action, modifiers);
    });

    glfwSetKeyCallback(window, [](GLFWwindow *, int key,
                                  int scancode, int action,
                                  int mods) {
        screen->keyCallbackEvent(key, scancode, action, mods);
    });

    glfwSetCharCallback(window,
                        [](GLFWwindow *, unsigned int codepoint) {
                            screen->charCallbackEvent(codepoint);
                        });

    glfwSetDropCallback(window, [](GLFWwindow *, int count,
                                   const char **filenames) {
        screen->dropCallbackEvent(count, filenames);
    });

    glfwSetScrollCallback(window,
                          [](GLFWwindow *, double x, double y) {
                              screen->scrollCallbackEvent(x, y);
                          });

    glfwSetFramebufferSizeCallback(
        window, [](GLFWwindow *, int width, int height) {
            screen->resizeCallbackEvent(width, height);
        });

    while (!glfwWindowShouldClose(window)) {

        OnMinusPressed(window);
        OnPlusPressed(window);
        OnClosePressed(window);

        glfwPollEvents();

        {
            spin_array.movable(window);
            // spin_array.set_pos({0.4, 0.1, 0});
            draw_frame(frame_shader, spin_array);
            draw_black_white(triangle_shader, spin_array, alg1);

            ising_text.RenderText(
                "magnetization: " +
                    std::to_string(alg1.calc_magnetization()),
                width * 0.5, height * 0.85, 1.0,
                glm::vec3(0.0, 0.0, 0), window);

            ising_text.RenderText(
                "energy: " +
                    std::to_string((int)alg1.calc_energy()),
                width * 0.75, height * 0.85, 1.0,
                glm::vec3(0.0, 0.0, 0), window);

            ising_text.RenderText("Ising model", width * 0.45,
                                  height * 0.95, 1.0,
                                  glm::vec3(1.0, 0, 0), window);

            if (algorithm_choice == 'M')
                alg1.metropolis_steps(size * size);
            else if (algorithm_choice == 'W')
                alg1.flip_cluster();
            else if (algorithm_choice == 'N')
                alg1.flip_cluster_nonrecursive();
            else if (algorithm_choice == 'V')
                alg1.flip_cluster_nonrecursive_vectorized();

            magnetization.push_back(alg1.get_magnetization());
            energy.push_back(alg1.get_energy());
        }

        {}
        // Draw nanogui

        screen->drawContents();
        screen->drawWidgets();

        glfwSwapBuffers(window);
        glClearColor(
            1.0f, 1.0f, 1.0f,
            1.0f); // set which color to clear the screen with
        // GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT and
        // GL_STENCIL_BUFFER_BIT. set which buffer to use to clear
        // the screen
        glClear(GL_COLOR_BUFFER_BIT);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
