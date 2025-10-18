set(SOURCE_FILES main.cpp)

add_executable(mutex ${SOURCE_FILES})

target_link_libraries(mutex PRIVATE project_options project_warnings)
