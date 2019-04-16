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

#include "nanogui/nanogui.h"
#include "ising_windows.hpp"

enum test_enum { Item1 = 0, Item2, Item3 };

void settings_window(nanogui::FormHelper *gui, GLFWwindow *window,
                     IsingModel<float> &algorithm, char &algorithm_choice,
                     std::vector<float> &energy,
                     std::vector<float> &magnetization) {

    bool bvar = true;
    int ivar = 12345678;
    double dvar = 3.1415926;
    float fvar = (float)dvar;
    std::string strval = "A string";
    test_enum enumval = Item2;
    nanogui::Color colval(0.5f, 0.5f, 0.7f, 1.f);

    gui->addGroup("Basic types");
    gui->addVariable("bool", bvar)->setTooltip("Test tooltip.");
    gui->addVariable("string", strval);

    gui->addGroup("Validating fields");
    gui->addVariable("int", ivar)->setSpinnable(true);
    gui->addVariable("float", fvar)->setTooltip("Test.");
    gui->addVariable("double", dvar)->setSpinnable(true);

    gui->addGroup("Complex types");
    gui->addVariable("Enumeration", enumval, true)
        ->setItems({"Item 1", "Item 2", "Item 3"});
    gui->addVariable("Color", colval)
        ->setFinalCallback([](const nanogui::Color &c) {
            std::cout << "ColorPicker Final Callback: [" << c.r() << ", "
                      << c.g() << ", " << c.b() << ", " << c.w() << "]"
                      << std::endl;
        });

    gui->addGroup("Other widgets");
    gui->addButton("A button",
                   []() { std::cout << "Button pressed." << std::endl; })
        ->setTooltip("Testing a much longer tooltip, that will wrap around to "
                     "new lines multiple times.");
    ;
}
