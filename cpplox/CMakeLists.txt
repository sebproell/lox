cmake_minimum_required(VERSION 3.22)

project(Lox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
add_subdirectorY(src)

enable_testing()
add_subdirectory(test)
