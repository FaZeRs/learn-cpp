set(SOURCE_FILES
    main.cpp)

add_executable(undo-framework
    ${SOURCE_FILES})

target_link_libraries(undo-framework PRIVATE
    project_options
    project_warnings
)
