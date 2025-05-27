set(SOURCE_FILES
    main.cpp)

add_executable(file-manager ${SOURCE_FILES})

target_link_libraries(file-manager PRIVATE
    project_options
    project_warnings
)
