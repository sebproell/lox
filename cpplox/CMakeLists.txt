project(Lox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

set(CMAKE_CXX_COMPILER "g++")

add_executable(cpplox src/lox.cpp)
