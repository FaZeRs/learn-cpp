set(SOURCE_FILES
    main.cpp)

add_executable(simd
    ${SOURCE_FILES})

target_link_libraries(simd PRIVATE
    project_options
    project_warnings
)

if (MSVC)
    target_compile_options(simd PRIVATE /arch:AVX2)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(simd PRIVATE -mavx2 -mfma)
endif()
