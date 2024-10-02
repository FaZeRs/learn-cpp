set(SOURCE_FILES main.cpp)

add_executable(linear-search ${SOURCE_FILES})

target_link_libraries(linear-search PRIVATE project_options project_warnings)
