set(SOURCE_FILES main.cpp)

add_executable(ternary-search ${SOURCE_FILES})

target_link_libraries(ternary-search PRIVATE project_options project_warnings)
