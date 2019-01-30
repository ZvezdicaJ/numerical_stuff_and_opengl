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

#include "../../third_party/apex_memmove/apex_memmove.h"
#include "../src/type_definitions.hpp"
#include "../src/convex_hull.hpp"

#include "../src/check_shaders.hpp"
#include "../src/shaders.hpp"
#include "../src/shader_class.hpp"
#include "../src/auxiliary_functions.hpp"
#include "../src/print_functions.hpp"
#include "../src/drawing_functions.hpp"
#include "../src/shape.hpp"
#include "../src/sphere.hpp"
#include "../src/circle.hpp"
#include "../src/rectangle.hpp"
#include "../src/box.hpp"
#include "../src/star.hpp"
#include "../src/disk.hpp"
#include "../src/display_functions.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
