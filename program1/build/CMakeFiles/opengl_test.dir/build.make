# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jure/Documents/opengl_ucenje/program1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jure/Documents/opengl_ucenje/program1/build

# Include any dependencies generated for this target.
include CMakeFiles/opengl_test.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/opengl_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/opengl_test.dir/flags.make

CMakeFiles/opengl_test.dir/opengl_test.cpp.o: CMakeFiles/opengl_test.dir/flags.make
CMakeFiles/opengl_test.dir/opengl_test.cpp.o: ../opengl_test.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jure/Documents/opengl_ucenje/program1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/opengl_test.dir/opengl_test.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/opengl_test.dir/opengl_test.cpp.o -c /home/jure/Documents/opengl_ucenje/program1/opengl_test.cpp

CMakeFiles/opengl_test.dir/opengl_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opengl_test.dir/opengl_test.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jure/Documents/opengl_ucenje/program1/opengl_test.cpp > CMakeFiles/opengl_test.dir/opengl_test.cpp.i

CMakeFiles/opengl_test.dir/opengl_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opengl_test.dir/opengl_test.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jure/Documents/opengl_ucenje/program1/opengl_test.cpp -o CMakeFiles/opengl_test.dir/opengl_test.cpp.s

CMakeFiles/opengl_test.dir/functions.cpp.o: CMakeFiles/opengl_test.dir/flags.make
CMakeFiles/opengl_test.dir/functions.cpp.o: ../functions.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jure/Documents/opengl_ucenje/program1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/opengl_test.dir/functions.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/opengl_test.dir/functions.cpp.o -c /home/jure/Documents/opengl_ucenje/program1/functions.cpp

CMakeFiles/opengl_test.dir/functions.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opengl_test.dir/functions.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jure/Documents/opengl_ucenje/program1/functions.cpp > CMakeFiles/opengl_test.dir/functions.cpp.i

CMakeFiles/opengl_test.dir/functions.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opengl_test.dir/functions.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jure/Documents/opengl_ucenje/program1/functions.cpp -o CMakeFiles/opengl_test.dir/functions.cpp.s

# Object files for target opengl_test
opengl_test_OBJECTS = \
"CMakeFiles/opengl_test.dir/opengl_test.cpp.o" \
"CMakeFiles/opengl_test.dir/functions.cpp.o"

# External object files for target opengl_test
opengl_test_EXTERNAL_OBJECTS =

../bin/opengl_test: CMakeFiles/opengl_test.dir/opengl_test.cpp.o
../bin/opengl_test: CMakeFiles/opengl_test.dir/functions.cpp.o
../bin/opengl_test: CMakeFiles/opengl_test.dir/build.make
../bin/opengl_test: ../lib/libglad.so
../bin/opengl_test: /usr/local/lib/libglfw.so
../bin/opengl_test: CMakeFiles/opengl_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jure/Documents/opengl_ucenje/program1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ../bin/opengl_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/opengl_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/opengl_test.dir/build: ../bin/opengl_test

.PHONY : CMakeFiles/opengl_test.dir/build

CMakeFiles/opengl_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/opengl_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/opengl_test.dir/clean

CMakeFiles/opengl_test.dir/depend:
	cd /home/jure/Documents/opengl_ucenje/program1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jure/Documents/opengl_ucenje/program1 /home/jure/Documents/opengl_ucenje/program1 /home/jure/Documents/opengl_ucenje/program1/build /home/jure/Documents/opengl_ucenje/program1/build /home/jure/Documents/opengl_ucenje/program1/build/CMakeFiles/opengl_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/opengl_test.dir/depend

