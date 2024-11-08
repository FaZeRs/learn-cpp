set(SOURCE_FILES main.cpp)

add_executable(thread-pool ${SOURCE_FILES})

target_link_libraries(thread-pool PRIVATE project_options project_warnings)
