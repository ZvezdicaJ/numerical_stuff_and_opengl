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
#include <mgl2/mgl.h>

#include "display_functions.hpp"
#include "ising_shader.hpp"
#include "spin_array.hpp"
#include "text_rendering.hpp"

// imgui includes
#include "imgui.h"

#include "imgui_internal.h"
#include "imconfig.h"
#include "imstb_rectpack.h"
#include "imstb_textedit.h"
#include "imstb_truetype.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

int main() {

    glfwInit();
    const char *glsl_version = "#version 450";

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
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

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard
    // Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable
    // Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can
    // also load multiple fonts and use ImGui::PushFont()/PopFont() to select
    // them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you
    // need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please
    // handle those errors in your application (e.g. use an assertion, or
    // display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and
    // stored into a texture when calling
    // ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame
    // below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string
    // literal you need to write a double backslash \\ !
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    // ImFont* font =
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
    // NULL, io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != NULL);

    Shader<RENDER_TYPE::CUSTOM> frame_shader(ising_frame_vertex_shaders,
                                             ising_frame_geometry_shader,
                                             ising_frame_fragment_shader);
    Shader<RENDER_TYPE::CUSTOM> triangle_shader(ising_triangle_vertex_shaders,
                                                ising_triangle_geometry_shader,
                                                ising_triangle_fragment_shader);
    unsigned size = 10;
    SpinArray<float> spin_array(size);
    spin_array.set_clickable_square(window);
    IsingModel<float> alg1(size);

    aligned_vector<float> vert = spin_array.get_vertexes();
    alg1.set_temperature(1);

    Text ising_text("/usr/share/fonts/truetype/ubuntu/Ubuntu-M.ttf");

    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    std::array<float, 2> ising_pos;
    while (!glfwWindowShouldClose(window)) {

        OnMinusPressed(window);
        OnPlusPressed(window);
        OnClosePressed(window);

        glfwPollEvents();

        // Start the Dear ImGui frame

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in
        // ImGui::ShowDemoWindow()! You can browse its code to learn more about
        // Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        { // draw Ising

            spin_array.movable(window);

            draw_frame(frame_shader, spin_array);
            draw_black_white(triangle_shader, spin_array, alg1);

            ising_text.RenderText("magnetization: " +
                                      std::to_string(alg1.calc_magnetization()),
                                  width * 0.3, height * 0.85, 1.0,
                                  glm::vec3(0.0, 0.0, 0), window);

            ising_text.RenderText("energy: " +
                                      std::to_string((int)alg1.calc_energy()),
                                  width * 0.6, height * 0.85, 1.0,
                                  glm::vec3(0.0, 0.0, 0), window);
            ising_text.RenderText("Ising model", width * 0.45, height * 0.95,
                                  1.0, glm::vec3(1.0, 0, 0), window);

            alg1.metropolis_steps(100);
            alg1.flip_cluster();
        }

        {
            ImVec2 pos = {50.0, 1.0};
            // ImGui::SetCursorPos(pos);
            ImGui::SetWindowPos(pos);
            std::array<double, 2> cursor_pos;

            std::array<float, 3> scale = {1, 1, 1};
            std::array<float, 4> square = {
                width / 2 - width / 4 * scale[0],
                width / 2 + width / 4 * scale[0],
                height / 2 - height / 4 * scale[1],
                height / 2 + height / 4 * scale[1],
            };
            glfwGetCursorPos(window, cursor_pos.data(), cursor_pos.data() + 1);
            ImGui::Text("Cursor_pos: x: %g  y: %g", cursor_pos[0],
                        cursor_pos[1]);
            ImGui::Text("Square: min_x: %g  max_x: %g", square[0], square[1]);
            ImGui::Text("Square: min_y: %g  max_y: %g", square[2], square[3]);
        }

        glfwSwapBuffers(window);
        glClearColor(1.0f, 1.0f, 1.0f,
                     1.0f); // set which color to clear the screen with
        // GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT and
        // GL_STENCIL_BUFFER_BIT. set which buffer to use to clear the
        // screen
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
