set(SOURCE_FILES main.cpp)

add_executable(circular_buffer ${SOURCE_FILES})

target_link_libraries(circular_buffer PRIVATE project_options project_warnings)
