set(SOURCE_FILES
    main.cpp)

add_executable(atomic
    ${SOURCE_FILES})

target_link_libraries(atomic PRIVATE
    project_options
    project_warnings
)