cmake_minimum_required(3.02)

find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL)

add_executable(opengl_test opengl_test.cpp)
cmake_target_link_libraries(opengl_test glfw3_LIBRARIES)

