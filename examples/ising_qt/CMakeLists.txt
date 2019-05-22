cmake_minimum_required(VERSION 3.2)
#set(CMAKE_C_COMPILER gcc CACHE STRING "C compiler" FORCE)
#set(CMAKE_CXX_COMPILER g++ CACHE STRING "C++ compiler" FORCE)

set (CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER "g++" CACHE STRING "C++ compiler" FORCE)


# uncomment to set intel compiler
#set(CMAKE_CXX_COMPILER "/opt/intel/system_studio_2019/compilers_and_libraries/linux/bin/intel64/icc" CACHE STRING "C++ compiler" FORCE)

message(STATUS "compiler: ${CMAKE_CXX_COMPILER}")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -ffast-math -Wno-reorder -Wnon-template-friend -Wno-unused-variable -Wno-maybe-uninitialized -Wno-narrowing")
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse4.1 -mno-avx -mno-avx2 -mno-fma -Wall -Wnon-template-friend -Wno-unused-variable")

else("g++" STREQUAL "$CMAKE_CXX_COMPILER")
  
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -ffast-math -msse -mavx -mavx2 -mfma -Wall -Wno-unused-variable")

endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(TOP_LEVEL_DIR "../..")

include_directories("${TOP_LEVEL_DIR}/src/algorithms")
include_directories("${TOP_LEVEL_DIR}/src")
include_directories("${TOP_LEVEL_DIR}/src/print_functions")
include_directories("${TOP_LEVEL_DIR}/src/shaders")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)


#set path to find cmake config files for qt
# change this based on the install directory of your qt version
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};/opt/Qt/5.12.3/gcc_64/lib/cmake")
# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)
# Find the QtCore library
find_package(Qt5Core CONFIG REQUIRED)
# Find the QtCore library
find_package(Qt5Gui CONFIG REQUIRED)

#APEX
set(APEX_MEMMOVE_DIR "${TOP_LEVEL_DIR}/third_party/apex_memmove/")
add_library(apex_memmove "${APEX_MEMMOVE_DIR}/apex_memmove.cpp"
  "${APEX_MEMMOVE_DIR}/apex_memmove.h")
include_directories("${APEX_MEMMOVE_DIR}")

# GLAD
set(GLAD_DIR "${TOP_LEVEL_DIR}/third_party/glad")
add_library(glad SHARED "${GLAD_DIR}/src/glad.c")
target_include_directories(glad PRIVATE "${GLAD_DIR}/include")
include_directories("${GLAD_DIR}/include")

#find OPENGL
find_package(OpenGL)

# add example files
add_executable(ising
  ising.cpp
  )

target_link_libraries(ising 
  apex_memmove
  libdl.so
  ${OPENGL_LIBRARIES}
  Qt5::Widgets
  Qt5::Core
  Qt5::Gui
  )
