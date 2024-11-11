if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT ENABLE_CLANG_TIDY)
  add_subdirectory(watcher)
endif()
