cmake_minimum_required(VERSION 3.2)
#set(CMAKE_C_COMPILER gcc CACHE STRING "C compiler" FORCE)
#set(CMAKE_CXX_COMPILER g++ CACHE STRING "C++ compiler" FORCE)

set(CMAKE_CXX_COMPILER "g++" CACHE STRING "C++ compiler" FORCE)


# uncomment to set intel compiler
#set(CMAKE_CXX_COMPILER "/opt/intel/system_studio_2019/compilers_and_libraries/linux/bin/intel64/icc" CACHE STRING "C++ compiler" FORCE)

message(STATUS "compiler: ${CMAKE_CXX_COMPILER}")
if("g++" STREQUAL "$CMAKE_CXX_COMPILER")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -ffast-math -Wall -Wnon-template-friend -Wno-unused-variable -Wmaybe-uninitialized")
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse4.1 -mno-avx -mno-avx2 -mno-fma -Wall -Wnon-template-friend -Wno-unused-variable")

else("g++" STREQUAL "$CMAKE_CXX_COMPILER")
  
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -ffast-math -msse -mavx -mavx2 -mfma -Wall -Wno-unused-variable")

endif("g++" STREQUAL "$CMAKE_CXX_COMPILER")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set (CMAKE_CXX_STANDARD 14)

find_package(glfw3 REQUIRED)
message(STATUS "glfw libraries: ${GLFW_LIBRARIES}")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native")

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
set(GLAD_DIR ./third_party/glad)
add_library(glad SHARED "${GLAD_DIR}/src/glad.c")
target_include_directories(glad PRIVATE "${GLAD_DIR}/include")

set(APEX_MEMMOVE_DIR ./third_party/apex_memmove/)
add_library(apex_memmove "${APEX_MEMMOVE_DIR}/apex_memmove.cpp"
  "${APEX_MEMMOVE_DIR}/apex_memmove.h")
include_directories("${APEX_MEMMOVE_DIR}")

include_directories("./src/shaders")
include_directories("./src/")
include_directories("./src/display_and_drawing_functions")
include_directories("./src/shapes")
include_directories("./src/algorithms")
include_directories("./src/print_functions")

target_include_directories(opengl_test PRIVATE "${GLAD_DIR}/include" "./examples/opengl_test")
target_link_libraries(opengl_test glad /usr/local/lib/libglfw.so apex_memmove)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

#this is to generate auxiliary functions
add_executable(auxiliary_functions_data testing/auxiliary_functions_test.cpp)
target_include_directories(test_auxiliary_functions PRIVATE "${GLAD_DIR}/include" "./examples/opengl_test")

# this is for gtests
add_executable(runTests
  testing/gtest_tests.cpp)
target_include_directories(runTests PRIVATE "${GLAD_DIR}/include")
target_link_libraries(runTests glad /usr/local/lib/libglfw.so libdl.so
  apex_memmove)

target_link_libraries(runTests ${GTEST_LIBRARIES} pthread)
