add_subdirectory(patterns)
add_subdirectory(data-structures)
add_subdirectory(sorting)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_subdirectory(modules)
endif()
add_subdirectory(concurrency)
add_subdirectory(templates)
add_subdirectory(memory)
add_subdirectory(search)
