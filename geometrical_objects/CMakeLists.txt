cmake_minimum_required(VERSION 3.2)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set (CMAKE_CXX_STANDARD 14)

find_package(glfw3 REQUIRED)
message(STATUS "glfw libraries: ${GLFW_LIBRARIES}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")

# GLFW
#set(GLFW_DIR "../../../Downloads/glfw")
#set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
#set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
#set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
#set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
#add_subdirectory("${GLFW_DIR}")
#target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
#target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
#target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# GLAD
set(GLAD_DIR /home/jure/Downloads/glad)
add_library(glad SHARED "${GLAD_DIR}/src/glad.c")
target_include_directories(glad PRIVATE "${GLAD_DIR}/include")

add_executable(opengl_test
  src/opengl_test.cpp
  src/check_shaders.cpp
  src/display_functions.cpp)

target_include_directories(opengl_test PRIVATE "${GLAD_DIR}/include")
target_link_libraries(opengl_test glad /usr/local/lib/libglfw.so)

add_executable(test_auxiliary_functions testing/auxiliary_functions_test.cpp)

