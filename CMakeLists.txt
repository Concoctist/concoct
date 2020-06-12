# Concoct CMake Configuration
cmake_minimum_required(VERSION 3.1.0)
set(PROJECT concoct)
project(${PROJECT})
message("Thank you for using Concoct!")
include_directories(include)
file(GLOB SOURCES src/*.c)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(MSVC)
  set(CMAKE_C_FLAGS "/W4 /WX")
else()
  set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -Werror")
endif()

add_executable(${PROJECT} ${SOURCES})
