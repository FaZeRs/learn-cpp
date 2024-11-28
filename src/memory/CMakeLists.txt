add_subdirectory(pools)
# GCC and clang-tidy do not work well together when using std::expected
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT ENABLE_CLANG_TIDY)
  add_subdirectory(raii)
endif()
add_subdirectory(reference-counting)
add_subdirectory(move-semantics)