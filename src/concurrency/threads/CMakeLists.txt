set(SOURCE_FILES main.cpp)

add_executable(threads ${SOURCE_FILES})

target_link_libraries(threads PRIVATE project_options project_warnings)
