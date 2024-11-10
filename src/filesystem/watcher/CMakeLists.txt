set(SOURCE_FILES main.cpp)

add_executable(file-watcher ${SOURCE_FILES})

target_link_libraries(file-watcher PRIVATE project_options project_warnings)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(file-watcher PRIVATE -stdlib=libc++)
  target_link_options(file-watcher PRIVATE -stdlib=libc++)
endif()
