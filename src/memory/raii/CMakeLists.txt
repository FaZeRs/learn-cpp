set(SOURCE_FILES main.cpp)

add_executable(raii ${SOURCE_FILES})

target_link_libraries(raii PRIVATE project_options project_warnings)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(raii PRIVATE -stdlib=libc++)
  target_link_options(raii PRIVATE -stdlib=libc++)
endif()
