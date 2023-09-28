set(SOURCE_FILES
  main.cpp)

add_executable(linked_list
  ${SOURCE_FILES})

target_link_libraries(linked_list PRIVATE
  project_options
  project_warnings
)
