cmake_minimum_required(VERSION 3.2)
project("Molecular Monte Carlo")
#set(CMAKE_C_COMPILER gcc CACHE STRING "C compiler" FORCE)
#set(CMAKE_CXX_COMPILER g++ CACHE STRING "C++ compiler" FORCE)

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER "g++" CACHE STRING "C++ compiler" FORCE)

# uncomment to set intel compiler
#set(CMAKE_CXX_COMPILER "/opt/intel/system_studio_2019/compilers_and_libraries/linux/bin/intel64/icc" CACHE STRING "C++ compiler" FORCE)

message(STATUS "compiler: ${CMAKE_CXX_COMPILER}")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -ffast-math -Wno-reorder -Wnon-template-friend -Wno-unused-variable -Wmaybe-uninitialized -DMKL_LP64 -m64")
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse4.1 -mno-avx -mno-avx2 -mno-fma -Wall -Wnon-template-friend -Wno-unused-variable")
  message(STATUS "compiler: g++")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -ffast-math -msse -mavx -mavx2 -mfma -Wall -Wno-unused-variable -DMKL_LP64")

  message(STATUS "compiler: icc")
endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)


include_directories("../../src/algorithms")

# add example files
add_executable(simulation
  main.cpp
  )


# add example files
add_executable(phase_diagram
  phase_diagram.cpp
  )

# add example files
add_executable(molecular_dynamics
  molecular_dynamics.cpp
  )

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  target_link_libraries(phase_diagram PUBLIC OpenMP::OpenMP_CXX)
  target_link_libraries(molecular_dynamics PUBLIC OpenMP::OpenMP_CXX)
endif()

link_directories(/opt/intel/system_studio_2019/mkl/lib/intel64_lin/)

set(MKLROOT /opt/intel/system_studio_2019/mkl)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")


  target_link_libraries(molecular_dynamics PUBLIC
    -Wl,--start-group
    ${MKLROOT}/lib/intel64_lin/libmkl_intel_lp64.a
    ${MKLROOT}/lib/intel64_lin/libmkl_gnu_thread.a
    ${MKLROOT}/lib/intel64_lin/libmkl_core.a
    -Wl,--end-group -lgomp -lpthread -lm -ldl
    )

  target_include_directories(molecular_dynamics PUBLIC ${MKLROOT}/include/)

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")

  target_link_libraries(molecular_dynamics PUBLIC
    -Wl,--start-group
    ${MKLROOT}/lib/intel64/libmkl_intel_lp64.a
    ${MKLROOT}/lib/intel64/libmkl_intel_thread.a
    ${MKLROOT}/lib/intel64/libmkl_core.a
    -Wl,--end-group -liomp5 -lpthread -lm -ldl)

  target_include_directories(molecular_dynamics PUBLIC ${MKLROOT}/include/)

endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
