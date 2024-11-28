set(SOURCE_FILES
    main.cpp)

add_executable(move-semantics
    ${SOURCE_FILES})

target_link_libraries(move-semantics PRIVATE
    project_options
    project_warnings
)