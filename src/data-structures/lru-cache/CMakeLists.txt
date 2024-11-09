set(SOURCE_FILES main.cpp)

add_executable(lru-cache ${SOURCE_FILES})

target_link_libraries(lru-cache PRIVATE project_options project_warnings)
