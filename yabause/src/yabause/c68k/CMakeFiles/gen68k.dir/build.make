# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
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

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jason/Projects/mednafen-ps3back/yabause/src/yabause/c68k

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jason/Projects/mednafen-ps3back/yabause/src/yabause/c68k

# Include any dependencies generated for this target.
include CMakeFiles/gen68k.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/gen68k.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/gen68k.dir/flags.make

CMakeFiles/gen68k.dir/c68kexec.c.o: CMakeFiles/gen68k.dir/flags.make
CMakeFiles/gen68k.dir/c68kexec.c.o: c68kexec.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jason/Projects/mednafen-ps3back/yabause/src/yabause/c68k/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/gen68k.dir/c68kexec.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/gen68k.dir/c68kexec.c.o   -c /home/jason/Projects/mednafen-ps3back/yabause/src/yabause/c68k/c68kexec.c

CMakeFiles/gen68k.dir/c68kexec.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gen68k.dir/c68kexec.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/jason/Projects/mednafen-ps3back/yabause/src/yabause/c68k/c68kexec.c > CMakeFiles/gen68k.dir/c68kexec.c.i

CMakeFiles/gen68k.dir/c68kexec.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gen68k.dir/c68kexec.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/jason/Projects/mednafen-ps3back/yabause/src/yabause/c68k/c68kexec.c -o CMakeFiles/gen68k.dir/c68kexec.c.s

CMakeFiles/gen68k.dir/c68kexec.c.o.requires:
.PHONY : CMakeFiles/gen68k.dir/c68kexec.c.o.requires

CMakeFiles/gen68k.dir/c68kexec.c.o.provides: CMakeFiles/gen68k.dir/c68kexec.c.o.requires
	$(MAKE) -f CMakeFiles/gen68k.dir/build.make CMakeFiles/gen68k.dir/c68kexec.c.o.provides.build
.PHONY : CMakeFiles/gen68k.dir/c68kexec.c.o.provides

CMakeFiles/gen68k.dir/c68kexec.c.o.provides.build: CMakeFiles/gen68k.dir/c68kexec.c.o
.PHONY : CMakeFiles/gen68k.dir/c68kexec.c.o.provides.build

CMakeFiles/gen68k.dir/c68k.c.o: CMakeFiles/gen68k.dir/flags.make
CMakeFiles/gen68k.dir/c68k.c.o: c68k.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jason/Projects/mednafen-ps3back/yabause/src/yabause/c68k/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/gen68k.dir/c68k.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/gen68k.dir/c68k.c.o   -c /home/jason/Projects/mednafen-ps3back/yabause/src/yabause/c68k/c68k.c

CMakeFiles/gen68k.dir/c68k.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gen68k.dir/c68k.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/jason/Projects/mednafen-ps3back/yabause/src/yabause/c68k/c68k.c > CMakeFiles/gen68k.dir/c68k.c.i

CMakeFiles/gen68k.dir/c68k.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gen68k.dir/c68k.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/jason/Projects/mednafen-ps3back/yabause/src/yabause/c68k/c68k.c -o CMakeFiles/gen68k.dir/c68k.c.s

CMakeFiles/gen68k.dir/c68k.c.o.requires:
.PHONY : CMakeFiles/gen68k.dir/c68k.c.o.requires

CMakeFiles/gen68k.dir/c68k.c.o.provides: CMakeFiles/gen68k.dir/c68k.c.o.requires
	$(MAKE) -f CMakeFiles/gen68k.dir/build.make CMakeFiles/gen68k.dir/c68k.c.o.provides.build
.PHONY : CMakeFiles/gen68k.dir/c68k.c.o.provides

CMakeFiles/gen68k.dir/c68k.c.o.provides.build: CMakeFiles/gen68k.dir/c68k.c.o
.PHONY : CMakeFiles/gen68k.dir/c68k.c.o.provides.build

CMakeFiles/gen68k.dir/gen68k.c.o: CMakeFiles/gen68k.dir/flags.make
CMakeFiles/gen68k.dir/gen68k.c.o: gen68k.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jason/Projects/mednafen-ps3back/yabause/src/yabause/c68k/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/gen68k.dir/gen68k.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/gen68k.dir/gen68k.c.o   -c /home/jason/Projects/mednafen-ps3back/yabause/src/yabause/c68k/gen68k.c

CMakeFiles/gen68k.dir/gen68k.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gen68k.dir/gen68k.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/jason/Projects/mednafen-ps3back/yabause/src/yabause/c68k/gen68k.c > CMakeFiles/gen68k.dir/gen68k.c.i

CMakeFiles/gen68k.dir/gen68k.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gen68k.dir/gen68k.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/jason/Projects/mednafen-ps3back/yabause/src/yabause/c68k/gen68k.c -o CMakeFiles/gen68k.dir/gen68k.c.s

CMakeFiles/gen68k.dir/gen68k.c.o.requires:
.PHONY : CMakeFiles/gen68k.dir/gen68k.c.o.requires

CMakeFiles/gen68k.dir/gen68k.c.o.provides: CMakeFiles/gen68k.dir/gen68k.c.o.requires
	$(MAKE) -f CMakeFiles/gen68k.dir/build.make CMakeFiles/gen68k.dir/gen68k.c.o.provides.build
.PHONY : CMakeFiles/gen68k.dir/gen68k.c.o.provides

CMakeFiles/gen68k.dir/gen68k.c.o.provides.build: CMakeFiles/gen68k.dir/gen68k.c.o
.PHONY : CMakeFiles/gen68k.dir/gen68k.c.o.provides.build

# Object files for target gen68k
gen68k_OBJECTS = \
"CMakeFiles/gen68k.dir/c68kexec.c.o" \
"CMakeFiles/gen68k.dir/c68k.c.o" \
"CMakeFiles/gen68k.dir/gen68k.c.o"

# External object files for target gen68k
gen68k_EXTERNAL_OBJECTS =

gen68k: CMakeFiles/gen68k.dir/c68kexec.c.o
gen68k: CMakeFiles/gen68k.dir/c68k.c.o
gen68k: CMakeFiles/gen68k.dir/gen68k.c.o
gen68k: CMakeFiles/gen68k.dir/build.make
gen68k: CMakeFiles/gen68k.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable gen68k"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gen68k.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/gen68k.dir/build: gen68k
.PHONY : CMakeFiles/gen68k.dir/build

CMakeFiles/gen68k.dir/requires: CMakeFiles/gen68k.dir/c68kexec.c.o.requires
CMakeFiles/gen68k.dir/requires: CMakeFiles/gen68k.dir/c68k.c.o.requires
CMakeFiles/gen68k.dir/requires: CMakeFiles/gen68k.dir/gen68k.c.o.requires
.PHONY : CMakeFiles/gen68k.dir/requires

CMakeFiles/gen68k.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/gen68k.dir/cmake_clean.cmake
.PHONY : CMakeFiles/gen68k.dir/clean

CMakeFiles/gen68k.dir/depend:
	cd /home/jason/Projects/mednafen-ps3back/yabause/src/yabause/c68k && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jason/Projects/mednafen-ps3back/yabause/src/yabause/c68k /home/jason/Projects/mednafen-ps3back/yabause/src/yabause/c68k /home/jason/Projects/mednafen-ps3back/yabause/src/yabause/c68k /home/jason/Projects/mednafen-ps3back/yabause/src/yabause/c68k /home/jason/Projects/mednafen-ps3back/yabause/src/yabause/c68k/CMakeFiles/gen68k.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/gen68k.dir/depend

