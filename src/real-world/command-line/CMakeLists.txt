set(SOURCE_FILES
    main.cpp)

add_executable(command-line
    ${SOURCE_FILES})

target_link_libraries(command-line PRIVATE
    project_options
    project_warnings
)
