set(SOURCE_FILES main.cpp)

add_executable(reference-counting ${SOURCE_FILES})

target_link_libraries(reference-counting PRIVATE project_options project_warnings)
