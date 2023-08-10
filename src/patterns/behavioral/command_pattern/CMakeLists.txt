set(SOURCE_FILES
  main.cpp)

add_executable(command_pattern
  ${SOURCE_FILES})

target_link_libraries(command_pattern PRIVATE
  project_options
  project_warnings
)
