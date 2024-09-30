set(SOURCE_FILES
    main.cpp)

add_executable(binary-search
    ${SOURCE_FILES})

target_link_libraries(binary-search PRIVATE
    project_options
    project_warnings
)
