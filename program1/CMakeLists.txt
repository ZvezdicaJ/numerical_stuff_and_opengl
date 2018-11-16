cmake_minimum_required(VERSION 3.2)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
message(STATUS "glfw libraries: ${GLFW_LIBRARIES}")
message(STATUS "opengl libraries: $OPEN_LIBRARIES")

add_executable(opengl_test opengl_test.cpp)
target_link_libraries(opengl_test /usr/local/lib/libglfw3.a $OPENGL_LIBRARIES)

