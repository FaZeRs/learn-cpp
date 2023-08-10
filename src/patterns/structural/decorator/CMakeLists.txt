set(SOURCE_FILES
  main.cpp)

add_executable(decorator
  ${SOURCE_FILES})

target_link_libraries(decorator PRIVATE
  project_options
  project_warnings
)
