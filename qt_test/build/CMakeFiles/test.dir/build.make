# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/yangsiyu/Workspace/camera_v4l2/qt_test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/yangsiyu/Workspace/camera_v4l2/qt_test/build

# Include any dependencies generated for this target.
include CMakeFiles/test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/test.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test.dir/flags.make

test_autogen/timestamp: /usr/bin/moc
test_autogen/timestamp: /usr/bin/uic
test_autogen/timestamp: CMakeFiles/test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/yangsiyu/Workspace/camera_v4l2/qt_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Automatic MOC and UIC for target test"
	/usr/bin/cmake -E cmake_autogen /home/yangsiyu/Workspace/camera_v4l2/qt_test/build/CMakeFiles/test_autogen.dir/AutogenInfo.json ""
	/usr/bin/cmake -E touch /home/yangsiyu/Workspace/camera_v4l2/qt_test/build/test_autogen/timestamp

CMakeFiles/test.dir/test_autogen/mocs_compilation.cpp.o: CMakeFiles/test.dir/flags.make
CMakeFiles/test.dir/test_autogen/mocs_compilation.cpp.o: test_autogen/mocs_compilation.cpp
CMakeFiles/test.dir/test_autogen/mocs_compilation.cpp.o: CMakeFiles/test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/yangsiyu/Workspace/camera_v4l2/qt_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/test.dir/test_autogen/mocs_compilation.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/test.dir/test_autogen/mocs_compilation.cpp.o -MF CMakeFiles/test.dir/test_autogen/mocs_compilation.cpp.o.d -o CMakeFiles/test.dir/test_autogen/mocs_compilation.cpp.o -c /home/yangsiyu/Workspace/camera_v4l2/qt_test/build/test_autogen/mocs_compilation.cpp

CMakeFiles/test.dir/test_autogen/mocs_compilation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/test.dir/test_autogen/mocs_compilation.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yangsiyu/Workspace/camera_v4l2/qt_test/build/test_autogen/mocs_compilation.cpp > CMakeFiles/test.dir/test_autogen/mocs_compilation.cpp.i

CMakeFiles/test.dir/test_autogen/mocs_compilation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/test.dir/test_autogen/mocs_compilation.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yangsiyu/Workspace/camera_v4l2/qt_test/build/test_autogen/mocs_compilation.cpp -o CMakeFiles/test.dir/test_autogen/mocs_compilation.cpp.s

CMakeFiles/test.dir/main.cc.o: CMakeFiles/test.dir/flags.make
CMakeFiles/test.dir/main.cc.o: /home/yangsiyu/Workspace/camera_v4l2/qt_test/main.cc
CMakeFiles/test.dir/main.cc.o: CMakeFiles/test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/yangsiyu/Workspace/camera_v4l2/qt_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/test.dir/main.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/test.dir/main.cc.o -MF CMakeFiles/test.dir/main.cc.o.d -o CMakeFiles/test.dir/main.cc.o -c /home/yangsiyu/Workspace/camera_v4l2/qt_test/main.cc

CMakeFiles/test.dir/main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/test.dir/main.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yangsiyu/Workspace/camera_v4l2/qt_test/main.cc > CMakeFiles/test.dir/main.cc.i

CMakeFiles/test.dir/main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/test.dir/main.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yangsiyu/Workspace/camera_v4l2/qt_test/main.cc -o CMakeFiles/test.dir/main.cc.s

CMakeFiles/test.dir/mainwindow.cc.o: CMakeFiles/test.dir/flags.make
CMakeFiles/test.dir/mainwindow.cc.o: /home/yangsiyu/Workspace/camera_v4l2/qt_test/mainwindow.cc
CMakeFiles/test.dir/mainwindow.cc.o: CMakeFiles/test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/yangsiyu/Workspace/camera_v4l2/qt_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/test.dir/mainwindow.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/test.dir/mainwindow.cc.o -MF CMakeFiles/test.dir/mainwindow.cc.o.d -o CMakeFiles/test.dir/mainwindow.cc.o -c /home/yangsiyu/Workspace/camera_v4l2/qt_test/mainwindow.cc

CMakeFiles/test.dir/mainwindow.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/test.dir/mainwindow.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yangsiyu/Workspace/camera_v4l2/qt_test/mainwindow.cc > CMakeFiles/test.dir/mainwindow.cc.i

CMakeFiles/test.dir/mainwindow.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/test.dir/mainwindow.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yangsiyu/Workspace/camera_v4l2/qt_test/mainwindow.cc -o CMakeFiles/test.dir/mainwindow.cc.s

# Object files for target test
test_OBJECTS = \
"CMakeFiles/test.dir/test_autogen/mocs_compilation.cpp.o" \
"CMakeFiles/test.dir/main.cc.o" \
"CMakeFiles/test.dir/mainwindow.cc.o"

# External object files for target test
test_EXTERNAL_OBJECTS =

test: CMakeFiles/test.dir/test_autogen/mocs_compilation.cpp.o
test: CMakeFiles/test.dir/main.cc.o
test: CMakeFiles/test.dir/mainwindow.cc.o
test: CMakeFiles/test.dir/build.make
test: /usr/lib/libQt5Widgets.so.5.15.13
test: /usr/lib/libQt5Gui.so.5.15.13
test: /usr/lib/libQt5Core.so.5.15.13
test: CMakeFiles/test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/yangsiyu/Workspace/camera_v4l2/qt_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test.dir/build: test
.PHONY : CMakeFiles/test.dir/build

CMakeFiles/test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test.dir/clean

CMakeFiles/test.dir/depend: test_autogen/timestamp
	cd /home/yangsiyu/Workspace/camera_v4l2/qt_test/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yangsiyu/Workspace/camera_v4l2/qt_test /home/yangsiyu/Workspace/camera_v4l2/qt_test /home/yangsiyu/Workspace/camera_v4l2/qt_test/build /home/yangsiyu/Workspace/camera_v4l2/qt_test/build /home/yangsiyu/Workspace/camera_v4l2/qt_test/build/CMakeFiles/test.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/test.dir/depend

