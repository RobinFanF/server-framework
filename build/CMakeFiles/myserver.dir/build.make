# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

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

# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/bing/mycode2022/server-framework

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/bing/mycode2022/server-framework/build

# Include any dependencies generated for this target.
include CMakeFiles/myserver.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/myserver.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/myserver.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/myserver.dir/flags.make

CMakeFiles/myserver.dir/src/log.cpp.o: CMakeFiles/myserver.dir/flags.make
CMakeFiles/myserver.dir/src/log.cpp.o: ../src/log.cpp
CMakeFiles/myserver.dir/src/log.cpp.o: CMakeFiles/myserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bing/mycode2022/server-framework/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/myserver.dir/src/log.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/myserver.dir/src/log.cpp.o -MF CMakeFiles/myserver.dir/src/log.cpp.o.d -o CMakeFiles/myserver.dir/src/log.cpp.o -c /home/bing/mycode2022/server-framework/src/log.cpp

CMakeFiles/myserver.dir/src/log.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myserver.dir/src/log.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bing/mycode2022/server-framework/src/log.cpp > CMakeFiles/myserver.dir/src/log.cpp.i

CMakeFiles/myserver.dir/src/log.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myserver.dir/src/log.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bing/mycode2022/server-framework/src/log.cpp -o CMakeFiles/myserver.dir/src/log.cpp.s

CMakeFiles/myserver.dir/src/config.cpp.o: CMakeFiles/myserver.dir/flags.make
CMakeFiles/myserver.dir/src/config.cpp.o: ../src/config.cpp
CMakeFiles/myserver.dir/src/config.cpp.o: CMakeFiles/myserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bing/mycode2022/server-framework/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/myserver.dir/src/config.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/myserver.dir/src/config.cpp.o -MF CMakeFiles/myserver.dir/src/config.cpp.o.d -o CMakeFiles/myserver.dir/src/config.cpp.o -c /home/bing/mycode2022/server-framework/src/config.cpp

CMakeFiles/myserver.dir/src/config.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myserver.dir/src/config.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bing/mycode2022/server-framework/src/config.cpp > CMakeFiles/myserver.dir/src/config.cpp.i

CMakeFiles/myserver.dir/src/config.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myserver.dir/src/config.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bing/mycode2022/server-framework/src/config.cpp -o CMakeFiles/myserver.dir/src/config.cpp.s

CMakeFiles/myserver.dir/src/utils.cpp.o: CMakeFiles/myserver.dir/flags.make
CMakeFiles/myserver.dir/src/utils.cpp.o: ../src/utils.cpp
CMakeFiles/myserver.dir/src/utils.cpp.o: CMakeFiles/myserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bing/mycode2022/server-framework/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/myserver.dir/src/utils.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/myserver.dir/src/utils.cpp.o -MF CMakeFiles/myserver.dir/src/utils.cpp.o.d -o CMakeFiles/myserver.dir/src/utils.cpp.o -c /home/bing/mycode2022/server-framework/src/utils.cpp

CMakeFiles/myserver.dir/src/utils.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myserver.dir/src/utils.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bing/mycode2022/server-framework/src/utils.cpp > CMakeFiles/myserver.dir/src/utils.cpp.i

CMakeFiles/myserver.dir/src/utils.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myserver.dir/src/utils.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bing/mycode2022/server-framework/src/utils.cpp -o CMakeFiles/myserver.dir/src/utils.cpp.s

CMakeFiles/myserver.dir/src/thread.cpp.o: CMakeFiles/myserver.dir/flags.make
CMakeFiles/myserver.dir/src/thread.cpp.o: ../src/thread.cpp
CMakeFiles/myserver.dir/src/thread.cpp.o: CMakeFiles/myserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bing/mycode2022/server-framework/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/myserver.dir/src/thread.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/myserver.dir/src/thread.cpp.o -MF CMakeFiles/myserver.dir/src/thread.cpp.o.d -o CMakeFiles/myserver.dir/src/thread.cpp.o -c /home/bing/mycode2022/server-framework/src/thread.cpp

CMakeFiles/myserver.dir/src/thread.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myserver.dir/src/thread.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bing/mycode2022/server-framework/src/thread.cpp > CMakeFiles/myserver.dir/src/thread.cpp.i

CMakeFiles/myserver.dir/src/thread.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myserver.dir/src/thread.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bing/mycode2022/server-framework/src/thread.cpp -o CMakeFiles/myserver.dir/src/thread.cpp.s

CMakeFiles/myserver.dir/src/fiber.cpp.o: CMakeFiles/myserver.dir/flags.make
CMakeFiles/myserver.dir/src/fiber.cpp.o: ../src/fiber.cpp
CMakeFiles/myserver.dir/src/fiber.cpp.o: CMakeFiles/myserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bing/mycode2022/server-framework/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/myserver.dir/src/fiber.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/myserver.dir/src/fiber.cpp.o -MF CMakeFiles/myserver.dir/src/fiber.cpp.o.d -o CMakeFiles/myserver.dir/src/fiber.cpp.o -c /home/bing/mycode2022/server-framework/src/fiber.cpp

CMakeFiles/myserver.dir/src/fiber.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myserver.dir/src/fiber.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bing/mycode2022/server-framework/src/fiber.cpp > CMakeFiles/myserver.dir/src/fiber.cpp.i

CMakeFiles/myserver.dir/src/fiber.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myserver.dir/src/fiber.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bing/mycode2022/server-framework/src/fiber.cpp -o CMakeFiles/myserver.dir/src/fiber.cpp.s

CMakeFiles/myserver.dir/src/scheduler.cpp.o: CMakeFiles/myserver.dir/flags.make
CMakeFiles/myserver.dir/src/scheduler.cpp.o: ../src/scheduler.cpp
CMakeFiles/myserver.dir/src/scheduler.cpp.o: CMakeFiles/myserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bing/mycode2022/server-framework/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/myserver.dir/src/scheduler.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/myserver.dir/src/scheduler.cpp.o -MF CMakeFiles/myserver.dir/src/scheduler.cpp.o.d -o CMakeFiles/myserver.dir/src/scheduler.cpp.o -c /home/bing/mycode2022/server-framework/src/scheduler.cpp

CMakeFiles/myserver.dir/src/scheduler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myserver.dir/src/scheduler.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bing/mycode2022/server-framework/src/scheduler.cpp > CMakeFiles/myserver.dir/src/scheduler.cpp.i

CMakeFiles/myserver.dir/src/scheduler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myserver.dir/src/scheduler.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bing/mycode2022/server-framework/src/scheduler.cpp -o CMakeFiles/myserver.dir/src/scheduler.cpp.s

CMakeFiles/myserver.dir/src/iomanager.cpp.o: CMakeFiles/myserver.dir/flags.make
CMakeFiles/myserver.dir/src/iomanager.cpp.o: ../src/iomanager.cpp
CMakeFiles/myserver.dir/src/iomanager.cpp.o: CMakeFiles/myserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bing/mycode2022/server-framework/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/myserver.dir/src/iomanager.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/myserver.dir/src/iomanager.cpp.o -MF CMakeFiles/myserver.dir/src/iomanager.cpp.o.d -o CMakeFiles/myserver.dir/src/iomanager.cpp.o -c /home/bing/mycode2022/server-framework/src/iomanager.cpp

CMakeFiles/myserver.dir/src/iomanager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myserver.dir/src/iomanager.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bing/mycode2022/server-framework/src/iomanager.cpp > CMakeFiles/myserver.dir/src/iomanager.cpp.i

CMakeFiles/myserver.dir/src/iomanager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myserver.dir/src/iomanager.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bing/mycode2022/server-framework/src/iomanager.cpp -o CMakeFiles/myserver.dir/src/iomanager.cpp.s

CMakeFiles/myserver.dir/src/timer.cpp.o: CMakeFiles/myserver.dir/flags.make
CMakeFiles/myserver.dir/src/timer.cpp.o: ../src/timer.cpp
CMakeFiles/myserver.dir/src/timer.cpp.o: CMakeFiles/myserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bing/mycode2022/server-framework/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/myserver.dir/src/timer.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/myserver.dir/src/timer.cpp.o -MF CMakeFiles/myserver.dir/src/timer.cpp.o.d -o CMakeFiles/myserver.dir/src/timer.cpp.o -c /home/bing/mycode2022/server-framework/src/timer.cpp

CMakeFiles/myserver.dir/src/timer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myserver.dir/src/timer.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bing/mycode2022/server-framework/src/timer.cpp > CMakeFiles/myserver.dir/src/timer.cpp.i

CMakeFiles/myserver.dir/src/timer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myserver.dir/src/timer.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bing/mycode2022/server-framework/src/timer.cpp -o CMakeFiles/myserver.dir/src/timer.cpp.s

CMakeFiles/myserver.dir/src/fd_manager.cpp.o: CMakeFiles/myserver.dir/flags.make
CMakeFiles/myserver.dir/src/fd_manager.cpp.o: ../src/fd_manager.cpp
CMakeFiles/myserver.dir/src/fd_manager.cpp.o: CMakeFiles/myserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bing/mycode2022/server-framework/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/myserver.dir/src/fd_manager.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/myserver.dir/src/fd_manager.cpp.o -MF CMakeFiles/myserver.dir/src/fd_manager.cpp.o.d -o CMakeFiles/myserver.dir/src/fd_manager.cpp.o -c /home/bing/mycode2022/server-framework/src/fd_manager.cpp

CMakeFiles/myserver.dir/src/fd_manager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myserver.dir/src/fd_manager.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bing/mycode2022/server-framework/src/fd_manager.cpp > CMakeFiles/myserver.dir/src/fd_manager.cpp.i

CMakeFiles/myserver.dir/src/fd_manager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myserver.dir/src/fd_manager.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bing/mycode2022/server-framework/src/fd_manager.cpp -o CMakeFiles/myserver.dir/src/fd_manager.cpp.s

CMakeFiles/myserver.dir/src/hook.cpp.o: CMakeFiles/myserver.dir/flags.make
CMakeFiles/myserver.dir/src/hook.cpp.o: ../src/hook.cpp
CMakeFiles/myserver.dir/src/hook.cpp.o: CMakeFiles/myserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bing/mycode2022/server-framework/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/myserver.dir/src/hook.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/myserver.dir/src/hook.cpp.o -MF CMakeFiles/myserver.dir/src/hook.cpp.o.d -o CMakeFiles/myserver.dir/src/hook.cpp.o -c /home/bing/mycode2022/server-framework/src/hook.cpp

CMakeFiles/myserver.dir/src/hook.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myserver.dir/src/hook.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bing/mycode2022/server-framework/src/hook.cpp > CMakeFiles/myserver.dir/src/hook.cpp.i

CMakeFiles/myserver.dir/src/hook.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myserver.dir/src/hook.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bing/mycode2022/server-framework/src/hook.cpp -o CMakeFiles/myserver.dir/src/hook.cpp.s

# Object files for target myserver
myserver_OBJECTS = \
"CMakeFiles/myserver.dir/src/log.cpp.o" \
"CMakeFiles/myserver.dir/src/config.cpp.o" \
"CMakeFiles/myserver.dir/src/utils.cpp.o" \
"CMakeFiles/myserver.dir/src/thread.cpp.o" \
"CMakeFiles/myserver.dir/src/fiber.cpp.o" \
"CMakeFiles/myserver.dir/src/scheduler.cpp.o" \
"CMakeFiles/myserver.dir/src/iomanager.cpp.o" \
"CMakeFiles/myserver.dir/src/timer.cpp.o" \
"CMakeFiles/myserver.dir/src/fd_manager.cpp.o" \
"CMakeFiles/myserver.dir/src/hook.cpp.o"

# External object files for target myserver
myserver_EXTERNAL_OBJECTS =

../lib/libmyserver.so: CMakeFiles/myserver.dir/src/log.cpp.o
../lib/libmyserver.so: CMakeFiles/myserver.dir/src/config.cpp.o
../lib/libmyserver.so: CMakeFiles/myserver.dir/src/utils.cpp.o
../lib/libmyserver.so: CMakeFiles/myserver.dir/src/thread.cpp.o
../lib/libmyserver.so: CMakeFiles/myserver.dir/src/fiber.cpp.o
../lib/libmyserver.so: CMakeFiles/myserver.dir/src/scheduler.cpp.o
../lib/libmyserver.so: CMakeFiles/myserver.dir/src/iomanager.cpp.o
../lib/libmyserver.so: CMakeFiles/myserver.dir/src/timer.cpp.o
../lib/libmyserver.so: CMakeFiles/myserver.dir/src/fd_manager.cpp.o
../lib/libmyserver.so: CMakeFiles/myserver.dir/src/hook.cpp.o
../lib/libmyserver.so: CMakeFiles/myserver.dir/build.make
../lib/libmyserver.so: CMakeFiles/myserver.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/bing/mycode2022/server-framework/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Linking CXX shared library ../lib/libmyserver.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/myserver.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/myserver.dir/build: ../lib/libmyserver.so
.PHONY : CMakeFiles/myserver.dir/build

CMakeFiles/myserver.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/myserver.dir/cmake_clean.cmake
.PHONY : CMakeFiles/myserver.dir/clean

CMakeFiles/myserver.dir/depend:
	cd /home/bing/mycode2022/server-framework/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/bing/mycode2022/server-framework /home/bing/mycode2022/server-framework /home/bing/mycode2022/server-framework/build /home/bing/mycode2022/server-framework/build /home/bing/mycode2022/server-framework/build/CMakeFiles/myserver.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/myserver.dir/depend

