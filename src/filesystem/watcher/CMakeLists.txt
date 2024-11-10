set(SOURCE_FILES main.cpp)

add_executable(file-watcher ${SOURCE_FILES})

target_link_libraries(file-watcher PRIVATE project_options project_warnings)
