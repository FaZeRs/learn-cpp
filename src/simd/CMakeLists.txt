set(SOURCE_FILES main.cpp)

add_executable(simd ${SOURCE_FILES})

target_link_libraries(simd PRIVATE project_options project_warnings)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(simd PRIVATE -mavx2 -mfma)
endif()
