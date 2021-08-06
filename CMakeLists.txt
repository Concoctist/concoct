# Concoct CMake Configuration
cmake_minimum_required(VERSION 3.1.0)
set(PROJECT concoct)
set(OBJTEST objtest)
set(STACKTEST stacktest)
set(INTERPRETTEST interprettest)
project(${PROJECT})
include_directories(include)
file(GLOB SOURCES src/*.c src/vm/*.c)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(OBJTEST_SOURCES src/debug.c src/memory.c src/types.c src/tests/objtest.c)
set(STACKTEST_SOURCES src/debug.c src/memory.c src/types.c src/vm/instructions.c src/vm/stack.c src/tests/stacktest.c)
set(INTERPRETTEST_SOURCES src/debug.c src/memory.c src/types.c src/vm/instructions.c src/vm/opcodes.c src/vm/stack.c src/vm/vm.c src/tests/interprettest.c)

if(MSVC)
	set(CMAKE_C_FLAGS "/W4 /WX /D_CRT_SECURE_NO_WARNINGS")
else()
	set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic")
endif()

add_executable(${PROJECT} ${SOURCES})
add_executable(${OBJTEST} ${OBJTEST_SOURCES})
add_executable(${STACKTEST} ${STACKTEST_SOURCES})
add_executable(${INTERPRETTEST} ${INTERPRETTEST_SOURCES})

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
	message("Setting default build type to: Debug")
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
	"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
	FORCE)
endif()

# Populate git revision, git hash, build time, build date, and build type variables for improved version information
find_package(Git)
execute_process(COMMAND "${GIT_EXECUTABLE}" rev-list --count HEAD WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
				OUTPUT_VARIABLE GIT_REV ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
				OUTPUT_VARIABLE GIT_HASH ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
string(TIMESTAMP BUILD_TIME "%H:%M")
string(TIMESTAMP BUILD_DATE "%m-%d-%Y")
set(BUILD_TYPE "${CMAKE_BUILD_TYPE}")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/version.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/include/version.h" @ONLY)

# Check for math library availability
include(CheckFunctionExists)
if(NOT ROUND_FUNCTION_EXISTS AND NOT NEED_LINKING_AGAINST_LIBM)
	CHECK_FUNCTION_EXISTS(round ROUND_FUNCTION_EXISTS)
	if(NOT ROUND_FUNCTION_EXISTS)
		unset(ROUND_FUNCTION_EXISTS CACHE)
		list(APPEND CMAKE_REQUIRED_LIBRARIES m)
		CHECK_FUNCTION_EXISTS(round ROUND_FUNCTION_EXISTS)
		if(ROUND_FUNCTION_EXISTS)
			set(NEED_LINKING_AGAINST_LIBM True CACHE BOOL "" FORCE)
		else()
			message(FATAL_ERROR "round() function is not available!")
		endif()
	endif()
endif()
if(NEED_LINKING_AGAINST_LIBM)
	target_link_libraries(${PROJECT} m)
	target_link_libraries(${OBJTEST} m)
	target_link_libraries(${STACKTEST} m)
    target_link_libraries(${INTERPRETTEST} m)
endif()

# Strip binary for release builds
if(CMAKE_BUILD_TYPE STREQUAL Release)
	add_custom_command(TARGET ${PROJECT} POST_BUILD COMMAND ${CMAKE_STRIP} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT})
	add_custom_command(TARGET ${OBJTEST} POST_BUILD COMMAND ${CMAKE_STRIP} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${OBJTEST})
	add_custom_command(TARGET ${STACKTEST} POST_BUILD COMMAND ${CMAKE_STRIP} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${STACKTEST})
    add_custom_command(TARGET ${INTERPRETTEST} POST_BUILD COMMAND ${CMAKE_STRIP} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${INTERPRETTEST})
endif()

message("Thank you for using Concoct!")
