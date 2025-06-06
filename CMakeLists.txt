cmake_minimum_required(VERSION 3.27)

include(cmake/CPM.cmake)

cpmaddpackage("gh:fazers/cmake#1.0.0")
set(CMAKE_MODULE_PATH "${cmake_SOURCE_DIR};")

project(
  learn-cpp
  VERSION "1.0.0"
  DESCRIPTION "My personal C++ learning project"
  HOMEPAGE_URL "https://naurislinde.dev"
  LANGUAGES CXX C)

include(cmake/Options.cmake)
include(cmake/CompilationFlags.cmake)

add_library(project_warnings INTERFACE)
add_library(project_options INTERFACE)

include(compiler-warnings)
set_project_warnings(
  project_warnings
  ${WARNINGS_AS_ERRORS}
  ""
  ""
  "")

include(sanitizers)
enable_sanitizers(
  project_options
  ${ENABLE_SANITIZER_ADDRESS}
  ${ENABLE_SANITIZER_LEAK}
  ${ENABLE_SANITIZER_UNDEFINED}
  ${ENABLE_SANITIZER_THREAD}
  ${ENABLE_SANITIZER_MEMORY})

if(ENABLE_CACHE)
  include(cache)
  enable_cache()
endif()

include(static-analyzers)

if(ENABLE_CLANG_TIDY)
  enable_clang_tidy(project_options ${WARNINGS_AS_ERRORS})
endif()

if(ENABLE_CPPCHECK)
  enable_cppcheck(project_options ${WARNINGS_AS_ERRORS} "")
endif()

if(ENABLE_INCLUDE_WHAT_YOU_USE)
  enable_include_what_you_use(project_options)
endif()

target_compile_features(project_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

add_subdirectory(src)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Release cxx: ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "Debug cxx: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS " ")

if(BUILD_TYPE STREQUAL "release")
  message(STATUS "Complete cxx: ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
  message(STATUS "Complete ld: ${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
else()
  message(STATUS "Complete cxx: ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
  message(STATUS "Complete ld: ${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
endif()
