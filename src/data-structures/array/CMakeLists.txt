set(SOURCE_FILES main.cpp)

add_executable(array ${SOURCE_FILES})

target_link_libraries(array PRIVATE project_options project_warnings)
