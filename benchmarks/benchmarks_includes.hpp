
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
#include <chrono>
#include <thread>
#include <functional>
#include <boost/align/aligned_allocator.hpp> // this is for aligned std::vector
#include <benchmark/benchmark.h>

#include "apex_memmove.h"
#include "type_definitions.hpp"
#include "convex_hull.hpp"

#include "auxiliary_functions.hpp"
#include "print_functions.hpp"

#include "bitonic_sort.hpp"
#include "modified_bitonic_sort.hpp"
#include "hybrid_sort.hpp"
