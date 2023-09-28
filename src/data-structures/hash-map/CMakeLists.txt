set(SOURCE_FILES
  main.cpp)

add_executable(hash_map
  ${SOURCE_FILES})

target_link_libraries(hash_map PRIVATE
  project_options
  project_warnings
)
