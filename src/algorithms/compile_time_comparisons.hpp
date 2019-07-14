#include <iostream>
#include <vector>
#include <array>
#include <string>

namespace compile_time {

template <typename T>
constexpr bool is_equal(T a, T b) {
    return a == b ? true : false;
}

} // namespace compile_time
