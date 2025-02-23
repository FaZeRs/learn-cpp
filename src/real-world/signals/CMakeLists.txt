set(SOURCE_FILES
    main.cpp)

add_executable(signals
    ${SOURCE_FILES})

target_link_libraries(signals PRIVATE
    project_options
    project_warnings
)
