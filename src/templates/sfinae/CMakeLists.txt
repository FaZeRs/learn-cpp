set(SOURCE_FILES
    main.cpp)

add_executable(sfinae
    ${SOURCE_FILES})

target_link_libraries(sfinae PRIVATE
    project_options
    project_warnings
)