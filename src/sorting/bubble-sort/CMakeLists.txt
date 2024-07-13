set(SOURCE_FILES
  main.cpp)

add_executable(bubble-sort
  ${SOURCE_FILES})

target_link_libraries(bubble-sort PRIVATE
  project_options
  project_warnings
)
