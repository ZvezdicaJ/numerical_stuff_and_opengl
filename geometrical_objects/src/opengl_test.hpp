
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

#include "check_shaders.hpp"
#include "shaders.hpp"
#include "shader_class.hpp"
#include "drawing_functions.hpp"
#include "auxiliary_functions.hpp"
#include "shape.hpp"
#include "print_functions.hpp"
#include "sphere.hpp"
#include "circle.hpp"

#include "display_functions.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
