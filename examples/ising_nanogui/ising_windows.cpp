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

#include "display_functions.hpp"
#include "ising_shader.hpp"
#include "spin_array.hpp"
#include "text_rendering.hpp"

#include "nanogui/nanogui.h"
#include "ising_windows.hpp"

enum test_enum { Item1 = 0, Item2, Item3 };

void settings_window(nanogui::Screen *screen, GLFWwindow *window,
                     IsingModel<float> &algorithm,
                     char *algorithm_choice,
                     std::vector<float> &energy,
                     std::vector<float> &magnetization) {

    nanogui::Window *nanoguiWindow =
        new nanogui::Window(screen, "Settings");
    nanoguiWindow->setFontSize(30);
    nanoguiWindow->setPosition(Eigen::Vector2i(100, 100));
    nanoguiWindow->setLayout(new nanogui::GroupLayout());

    static int counter = 0;
    counter++;
    std::cout << "counter: " << counter << std::endl;
    bool bvar = true;
    int ivar = 12345678;
    double dvar = 3.1415926;
    float fvar = (float)dvar;
    std::string strval = "A string";
    test_enum enumval = Item2;

    nanogui::Color colval(0.5f, 0.5f, 0.7f, 1.f);
    // nanoguiWindow->setGroupFontSize(30);
    // nanoguiWindow->setLabelFontSize(30);
    // gui->setFixedSize(Eigen::Vector2i(150, 50));

    /* No need to store a pointer, the data structure will be
       automatically freed when the parent window is deleted */

    nanogui::Button *b = new nanogui::Button(nanoguiWindow, "stop");
    nanogui::Label *algorithm_group =
        new nanogui::Label(nanoguiWindow, "Choose algorithm");
    algorithm_group->setFontSize(30);

    nanogui::CheckBox *select_metropolis =
        new nanogui::CheckBox(nanoguiWindow, "select metropolis");
    select_metropolis->setFontSize(30);

    nanogui::CheckBox *select_recursive_wolff =
        new nanogui::CheckBox(nanoguiWindow,
                              "select recursive wolff");
    select_recursive_wolff->setFontSize(30);

    nanogui::CheckBox *select_nonrecursive_wolff =
        new nanogui::CheckBox(nanoguiWindow,
                              "select nonrecursive wolff");
    select_nonrecursive_wolff->setFontSize(30);

    b->setCallback([algorithm_choice, b, select_metropolis,
                    select_recursive_wolff,
                    select_nonrecursive_wolff] {
        if (*algorithm_choice == '0') {
            b->setCaption("Stop");
            *algorithm_choice = 'M';
            select_metropolis->setChecked(true);
        } else {
            b->setCaption("Start");
            *algorithm_choice = '0';
            select_metropolis->setChecked(false);
            select_recursive_wolff->setChecked(false);
            select_nonrecursive_wolff->setChecked(false);
        }
        // std::cout << "alg: " << +algorithm_choice << std::endl;
    });
    b->setTooltip("click to start or stop simulation");
    b->setFontSize(30);

    select_metropolis->setChecked(true);
    select_recursive_wolff->setChecked(false);
    select_nonrecursive_wolff->setChecked(false);

    std::function<void(const bool &)> f1 =
        [select_metropolis, select_recursive_wolff,
         select_nonrecursive_wolff, algorithm_choice,
         b](const bool &value) {
            if (value) {
                *algorithm_choice = 'M';
                select_metropolis->setChecked(true);
                select_recursive_wolff->setChecked(false);
                select_nonrecursive_wolff->setChecked(false);
                b->setCaption("Stop");
            } else {
                select_metropolis->setChecked(false);
                *algorithm_choice = '0';
                b->setCaption("Start");
            }
        };

    std::function<void(const bool &)> f2 =
        [select_metropolis, select_recursive_wolff,
         select_nonrecursive_wolff, algorithm_choice,
         b](const bool &value) {
            if (value) {
                *algorithm_choice = 'W';
                select_recursive_wolff->setChecked(true);
                select_nonrecursive_wolff->setChecked(false);
                select_metropolis->setChecked(false);
                b->setCaption("Stop");
            } else {
                select_recursive_wolff->setChecked(false);
                *algorithm_choice = '0';
                b->setCaption("Start");
            }
        };

    std::function<void(const bool &)> f3 =
        [select_metropolis, select_nonrecursive_wolff,
         select_recursive_wolff, algorithm_choice,
         b](const bool &value) {
            if (value) {
                *algorithm_choice = 'N';
                select_nonrecursive_wolff->setChecked(true);
                select_recursive_wolff->setChecked(false);
                select_metropolis->setChecked(false);
                b->setCaption("Stop");
            } else {
                select_nonrecursive_wolff->setChecked(false);
                *algorithm_choice = '0';
                b->setCaption("Start");
            }
        };

    select_metropolis->setCallback(f1);
    select_recursive_wolff->setCallback(f2);
    select_nonrecursive_wolff->setCallback(f3);
    //        select_metropolis->setPosition();
    /*
    std::function<void(const bool &)> f1 = [&](const bool
    &value) { if (value) algorithm_choice = 'M'; else
            algorithm_choice = '0';
    };
    std::function<bool()> f2 = [&]() {
        if (algorithm_choice == 'M')
            return true;
        else
            return false;
    };

    // getter for second field
    std::function<void(const bool &)> f3 = [&](const bool
    &value) { if (value) algorithm_choice = 'W'; else
            algorithm_choice = '0';
    };
    // setter for second field
    std::function<bool()> f4 = [&]() {
        if (algorithm_choice == 'W')
            return true;
        else
            return false;
    };

    nanogui::detail::FormWidget<bool> *select_metropolis =
        gui->addVariable("Metropolis algorithm", f1, f2);

    nanogui::detail::FormWidget<bool> *select_wolff =
        gui->addVariable("Wolff algorithm", f3, f4);

    select_metropolis->setTooltip("Test tooltip.");
    select_wolff->setTooltip("Test tooltip.");

    gui->addVariable("string", strval);

    gui->addGroup("Validating fields");
    // gui->mVariableSpacing = 10;
    { // add integer field
        nanogui::detail::FormWidget<int> *integer_field =
            gui->addVariable("int", ivar);
        integer_field->setSpinnable(true);
        int new_height = (int)(1.1 *
    (float)(integer_field->fontSize()));
        integer_field->setFixedHeight(new_height);
    }
    gui->addVariable("float", fvar)->setTooltip("Test.");

    // gui->addVariable("double", dvar)->setSpinnable(true);

    gui->addGroup("Complex types");
    gui->addVariable("Enumeration", enumval, true)
        ->setItems({"Item 1", "Item 2", "Item 3"});
    gui->addVariable("Color", colval)
        ->setFinalCallback([](const nanogui::Color &c) {
            std::cout << "ColorPicker Final Callback: [" <<
    c.r() <<
    ", "
                      << c.g() << ", " << c.b() << ", " << c.w()
    <<
    "]"
                      << std::endl;
        });

    gui->addGroup("Other widgets");
    gui->addButton("A button",
                   []() { std::cout << "Button pressed." <<
    std::endl; })
        ->setTooltip("Testing a much longer tooltip, that will
    wrap around to " "new lines multiple times.");
    */
    screen->performLayout();
}
