# Concoct CMake Configuration
cmake_minimum_required(VERSION 2.8.12)
project(concoct)
message("Thank you for using Concoct!")
file(GLOB SOURCES src/*.c)
include_directories(include)
add_custom_target(bindir ALL COMMAND ${CMAKE_COMMAND} -E make_directory bin)
add_executable(concoct src/concoct.c src/concoct_lexer.c)
set_target_properties(concoct PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
