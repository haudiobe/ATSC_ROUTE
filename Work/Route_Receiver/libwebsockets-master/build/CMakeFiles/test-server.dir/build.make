# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.3

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master/build

# Include any dependencies generated for this target.
include CMakeFiles/test-server.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test-server.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test-server.dir/flags.make

CMakeFiles/test-server.dir/test-server/test-server.c.o: CMakeFiles/test-server.dir/flags.make
CMakeFiles/test-server.dir/test-server/test-server.c.o: ../test-server/test-server.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/test-server.dir/test-server/test-server.c.o"
	/opt/android-toolchain/arm-linux-androideabi/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/test-server.dir/test-server/test-server.c.o   -c /var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master/test-server/test-server.c

CMakeFiles/test-server.dir/test-server/test-server.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/test-server.dir/test-server/test-server.c.i"
	/opt/android-toolchain/arm-linux-androideabi/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master/test-server/test-server.c > CMakeFiles/test-server.dir/test-server/test-server.c.i

CMakeFiles/test-server.dir/test-server/test-server.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/test-server.dir/test-server/test-server.c.s"
	/opt/android-toolchain/arm-linux-androideabi/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master/test-server/test-server.c -o CMakeFiles/test-server.dir/test-server/test-server.c.s

CMakeFiles/test-server.dir/test-server/test-server.c.o.requires:

.PHONY : CMakeFiles/test-server.dir/test-server/test-server.c.o.requires

CMakeFiles/test-server.dir/test-server/test-server.c.o.provides: CMakeFiles/test-server.dir/test-server/test-server.c.o.requires
	$(MAKE) -f CMakeFiles/test-server.dir/build.make CMakeFiles/test-server.dir/test-server/test-server.c.o.provides.build
.PHONY : CMakeFiles/test-server.dir/test-server/test-server.c.o.provides

CMakeFiles/test-server.dir/test-server/test-server.c.o.provides.build: CMakeFiles/test-server.dir/test-server/test-server.c.o


# Object files for target test-server
test__server_OBJECTS = \
"CMakeFiles/test-server.dir/test-server/test-server.c.o"

# External object files for target test-server
test__server_EXTERNAL_OBJECTS =

bin/libwebsockets-test-server: CMakeFiles/test-server.dir/test-server/test-server.c.o
bin/libwebsockets-test-server: CMakeFiles/test-server.dir/build.make
bin/libwebsockets-test-server: lib/libwebsockets.a
bin/libwebsockets-test-server: /opt/android-toolchain/sysroot/usr/lib/libz.so
bin/libwebsockets-test-server: /opt/android-toolchain/sysroot/usr/lib/libssl.so
bin/libwebsockets-test-server: /opt/android-toolchain/sysroot/usr/lib/libcrypto.so
bin/libwebsockets-test-server: CMakeFiles/test-server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable bin/libwebsockets-test-server"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test-server.dir/link.txt --verbose=$(VERBOSE)
	/usr/bin/cmake -E make_directory /var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master/build/bin/../share/libwebsockets-test-server
	/usr/bin/cmake -E copy /var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master/test-server/favicon.ico /var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master/build/bin/../share/libwebsockets-test-server
	/usr/bin/cmake -E copy /var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master/test-server/leaf.jpg /var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master/build/bin/../share/libwebsockets-test-server
	/usr/bin/cmake -E copy /var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master/test-server/libwebsockets.org-logo.png /var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master/build/bin/../share/libwebsockets-test-server
	/usr/bin/cmake -E copy /var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master/test-server/test.html /var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master/build/bin/../share/libwebsockets-test-server

# Rule to build all files generated by this target.
CMakeFiles/test-server.dir/build: bin/libwebsockets-test-server

.PHONY : CMakeFiles/test-server.dir/build

CMakeFiles/test-server.dir/requires: CMakeFiles/test-server.dir/test-server/test-server.c.o.requires

.PHONY : CMakeFiles/test-server.dir/requires

CMakeFiles/test-server.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test-server.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test-server.dir/clean

CMakeFiles/test-server.dir/depend:
	cd /var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master /var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master /var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master/build /var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master/build /var/www/html/android/ATSC_ROUTE/Work/Route_Receiver/libwebsockets-master/build/CMakeFiles/test-server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test-server.dir/depend

