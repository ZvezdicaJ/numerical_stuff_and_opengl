cmake_minimum_required(VERSION 3.2)
#set(CMAKE_C_COMPILER gcc CACHE STRING "C compiler" FORCE)
#set(CMAKE_CXX_COMPILER g++ CACHE STRING "C++ compiler" FORCE)

set (CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER "g++" CACHE STRING "C++ compiler" FORCE)


# uncomment to set intel compiler
#set(CMAKE_CXX_COMPILER "/opt/intel/system_studio_2019/compilers_and_libraries/linux/bin/intel64/icc" CACHE STRING "C++ compiler" FORCE)

message(STATUS "compiler: ${CMAKE_CXX_COMPILER}")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -ffast-math -Wall -Wnon-template-friend -Wno-unused-variable -Wmaybe-uninitialized")
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse4.1 -mno-avx -mno-avx2 -mno-fma -Wall -Wnon-template-friend -Wno-unused-variable")

else("g++" STREQUAL "$CMAKE_CXX_COMPILER")
  
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -ffast-math -msse -mavx -mavx2 -mfma -Wall -Wno-unused-variable")

endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(glfw3 REQUIRED)
message(STATUS "glfw libraries: ${GLFW_LIBRARIES}")

include_directories("../../src/shaders")
include_directories("../../src/")
include_directories("../../src/display_and_drawing_functions")
include_directories("../../src/shapes")
include_directories("../../src/algorithms")
include_directories("../../src/print_functions")

set(TOP_LEVEL_DIR "../..")

# GLAD
set(GLAD_DIR "${TOP_LEVEL_DIR}/third_party/glad")
add_library(glad SHARED "${GLAD_DIR}/src/glad.c")
target_include_directories(glad PRIVATE "${GLAD_DIR}/include")
include_directories("${GLAD_DIR}/include")

#APEX
set(APEX_MEMMOVE_DIR "${TOP_LEVEL_DIR}/third_party/apex_memmove/")
add_library(apex_memmove "${APEX_MEMMOVE_DIR}/apex_memmove.cpp"
  "${APEX_MEMMOVE_DIR}/apex_memmove.h")
include_directories("${APEX_MEMMOVE_DIR}")

#DISPLAY FUNCTIONS - events, input,...
add_library(display_functions "${TOP_LEVEL_DIR}/src/display_and_drawing_functions/display_functions.cpp")
target_link_libraries(display_functions  glad /usr/local/lib/libglfw.so apex_memmove)

# add example files
add_executable(ising ising.cpp)#  ../../src/shaders/shader_class.cpp)
#target_include_directories(opengl_test PRIVATE "${GLAD_DIR}/include" "./examples/opengl_test")
target_link_libraries(ising display_functions  glad /usr/local/lib/libglfw.so apex_memmove libdl.so)
