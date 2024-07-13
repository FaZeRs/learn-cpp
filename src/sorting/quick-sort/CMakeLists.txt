set(SOURCE_FILES
  main.cpp)

add_executable(quick-sort
  ${SOURCE_FILES})

target_link_libraries(quick-sort PRIVATE
  project_options
  project_warnings
)
