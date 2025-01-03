set(SOURCE_FILES
    main.cpp)

add_executable(logger
    ${SOURCE_FILES})

target_link_libraries(logger PRIVATE
    project_options
    project_warnings
)
