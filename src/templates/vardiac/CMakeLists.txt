set(SOURCE_FILES
    main.cpp)

add_executable(vardiac
    ${SOURCE_FILES})

target_link_libraries(vardiac PRIVATE
    project_options
    project_warnings
)