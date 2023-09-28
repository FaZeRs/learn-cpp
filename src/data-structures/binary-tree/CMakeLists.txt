set(SOURCE_FILES
  main.cpp)

add_executable(binary_tree
  ${SOURCE_FILES})

target_link_libraries(binary_tree PRIVATE
  project_options
  project_warnings
)
