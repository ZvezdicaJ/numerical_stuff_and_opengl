#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
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

#include "auxiliary_functions.hpp"
#include "print_functions.hpp"

#include "ising_shader.hpp"
#include "Ising_model.hpp"
#include "spin_array.hpp"

#include <QApplication>
#include <QWidget>
#include <QtCore>
#include <QMainWindow>

//#include "ising_windows.hpp"

int main(int argc, char **argv) {

    /*    unsigned size = 50;
    std::array<float, 3> starting_pos = {0.37, 0.1, 0.0};
    SpinArray<float> spin_array(size, starting_pos);
    IsingModel<float> alg1(size);
    char algorithm_choice = 'M';
    aligned_vector<float> vert = spin_array.get_vertexes();
    alg1.set_temperature(2.2);

    std::array<float, 2> ising_pos;
    std::vector<float> energy, magnetization;

    bool enabled = true;
    */

    QApplication app(argc, argv);

    QMainWindow w;

    SpinArray<float> spin_array(&w);

    w.show();
    return app.exec();
}
