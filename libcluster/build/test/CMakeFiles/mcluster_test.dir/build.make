# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.15.5/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.15.5/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/zimolin/Desktop/senior capstone/libcluster"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/zimolin/Desktop/senior capstone/libcluster/build"

# Include any dependencies generated for this target.
include test/CMakeFiles/mcluster_test.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/mcluster_test.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/mcluster_test.dir/flags.make

test/CMakeFiles/mcluster_test.dir/mcluster_test.cpp.o: test/CMakeFiles/mcluster_test.dir/flags.make
test/CMakeFiles/mcluster_test.dir/mcluster_test.cpp.o: ../test/mcluster_test.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/zimolin/Desktop/senior capstone/libcluster/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/CMakeFiles/mcluster_test.dir/mcluster_test.cpp.o"
	cd "/Users/zimolin/Desktop/senior capstone/libcluster/build/test" && /Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/mcluster_test.dir/mcluster_test.cpp.o -c "/Users/zimolin/Desktop/senior capstone/libcluster/test/mcluster_test.cpp"

test/CMakeFiles/mcluster_test.dir/mcluster_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mcluster_test.dir/mcluster_test.cpp.i"
	cd "/Users/zimolin/Desktop/senior capstone/libcluster/build/test" && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/zimolin/Desktop/senior capstone/libcluster/test/mcluster_test.cpp" > CMakeFiles/mcluster_test.dir/mcluster_test.cpp.i

test/CMakeFiles/mcluster_test.dir/mcluster_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mcluster_test.dir/mcluster_test.cpp.s"
	cd "/Users/zimolin/Desktop/senior capstone/libcluster/build/test" && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/zimolin/Desktop/senior capstone/libcluster/test/mcluster_test.cpp" -o CMakeFiles/mcluster_test.dir/mcluster_test.cpp.s

# Object files for target mcluster_test
mcluster_test_OBJECTS = \
"CMakeFiles/mcluster_test.dir/mcluster_test.cpp.o"

# External object files for target mcluster_test
mcluster_test_EXTERNAL_OBJECTS =

mcluster_test: test/CMakeFiles/mcluster_test.dir/mcluster_test.cpp.o
mcluster_test: test/CMakeFiles/mcluster_test.dir/build.make
mcluster_test: ../lib/libcluster.dylib
mcluster_test: /usr/local/opt/libomp/lib/libomp.a
mcluster_test: test/CMakeFiles/mcluster_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/Users/zimolin/Desktop/senior capstone/libcluster/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../mcluster_test"
	cd "/Users/zimolin/Desktop/senior capstone/libcluster/build/test" && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mcluster_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/mcluster_test.dir/build: mcluster_test

.PHONY : test/CMakeFiles/mcluster_test.dir/build

test/CMakeFiles/mcluster_test.dir/clean:
	cd "/Users/zimolin/Desktop/senior capstone/libcluster/build/test" && $(CMAKE_COMMAND) -P CMakeFiles/mcluster_test.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/mcluster_test.dir/clean

test/CMakeFiles/mcluster_test.dir/depend:
	cd "/Users/zimolin/Desktop/senior capstone/libcluster/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/zimolin/Desktop/senior capstone/libcluster" "/Users/zimolin/Desktop/senior capstone/libcluster/test" "/Users/zimolin/Desktop/senior capstone/libcluster/build" "/Users/zimolin/Desktop/senior capstone/libcluster/build/test" "/Users/zimolin/Desktop/senior capstone/libcluster/build/test/CMakeFiles/mcluster_test.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : test/CMakeFiles/mcluster_test.dir/depend

