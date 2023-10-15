cmake_minimum_required(VERSION 3.22)

project(Lox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

set(CMAKE_CXX_COMPILER "g++")

add_subdirectorY(src)

enable_testing()
add_subdirectory(test)
