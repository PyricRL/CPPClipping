cmake_minimum_required(VERSION 3.10)
project(MyApp)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)

add_executable(myapp main.cpp)
