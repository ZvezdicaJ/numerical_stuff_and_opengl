cmake_minimum_required(VERSION 3.2)
#set(CMAKE_C_COMPILER gcc CACHE STRING "C compiler" FORCE)
#set(CMAKE_CXX_COMPILER g++ CACHE STRING "C++ compiler" FORCE)

set (CMAKE_CXX_STANDARD 17)
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


include_directories("../../src/")
include_directories("../../src/algorithms")
include_directories("../../src/print_functions")

set(TOP_LEVEL_DIR "../..")

#APEX
set(APEX_MEMMOVE_DIR "${TOP_LEVEL_DIR}/third_party/apex_memmove/")
add_library(apex_memmove "${APEX_MEMMOVE_DIR}/apex_memmove.cpp"
  "${APEX_MEMMOVE_DIR}/apex_memmove.h")
include_directories("${APEX_MEMMOVE_DIR}")


# add example files
add_executable(ising
  ising.cpp
  )

target_link_libraries(ising
  apex_memmove
  libdl.so
  )
