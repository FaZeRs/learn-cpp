set(SOURCE_FILES
    main.cpp)

add_executable(ui
    ${SOURCE_FILES})

target_link_libraries(ui PRIVATE
    project_options
    project_warnings
    X11
)
