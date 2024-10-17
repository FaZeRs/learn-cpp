cmake_minimum_required(VERSION 3.29 FATAL_ERROR)
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

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(math_module PRIVATE -stdlib=libc++)
    target_link_options(math_module PRIVATE -stdlib=libc++)
endif()