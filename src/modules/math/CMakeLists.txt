cmake_minimum_required(VERSION 3.29)
project(math_module CXX)
set(CMAKE_CXX_STANDARD 23)

add_library(math)
target_sources(math
    PUBLIC
    FILE_SET CXX_MODULES FILES
    math.cpp
)

add_executable(math_module main.cpp)
target_link_libraries(math_module math)