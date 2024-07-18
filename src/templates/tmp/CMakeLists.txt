set(SOURCE_FILES
    main.cpp)

add_executable(tmp
    ${SOURCE_FILES})

target_link_libraries(tmp PRIVATE
    project_options
    project_warnings
)