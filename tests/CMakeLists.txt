cmake_minimum_required(VERSION 3.5)

project(unit-tests LANGUAGES CXX)

add_executable(unit-tests tst_mytest.cpp)

CPMAddPackage(
  NAME Catch2
  VERSION 3.3.2
  GITHUB_REPOSITORY catchorg/Catch2
  SYSTEM YES
)

target_link_libraries(unit-tests PRIVATE
  project_warnings
  project_options
  Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(unit-tests)