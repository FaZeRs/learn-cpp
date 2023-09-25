set(SOURCE_FILES
  main.cpp)

add_executable(stack
  ${SOURCE_FILES})

target_link_libraries(stack PRIVATE
  project_options
  project_warnings
)
