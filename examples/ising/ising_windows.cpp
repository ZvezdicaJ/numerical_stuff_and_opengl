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
#include "display_functions.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <mgl2/mgl.h>

#include "display_functions.hpp"
#include "ising_shader.hpp"
#include "spin_array.hpp"
#include "text_rendering.hpp"

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

#include "ising_windows.hpp"

void settings_window(GLFWwindow *window, IsingModel<float> &algorithm) {

    static bool first_call = true;
    if (first_call) {
        ImGui::SetWindowPos({100, 100});
        first_call = false;
    }

    ImGui::Begin("Settings");

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    ImVec2 window_size = {0.4 * width, 0.8 * height};
    ImGui::SetWindowSize(window_size, true);
    float temperature = algorithm.get_temperature();
    ImGui::SliderFloat("Temperature", &temperature, 0.0f, 10.0f);
    algorithm.set_temperature(temperature);

    /* std::array<double, 2> cursor_pos;
    glfwGetCursorPos(window, cursor_pos.data(), cursor_pos.data() + 1);
    ImGui::Text("Cursor_pos: x: %g  y: %g", cursor_pos[0], cursor_pos[1]);
    */

    // ImGui::EndChild();
    ImGui::End();
}
