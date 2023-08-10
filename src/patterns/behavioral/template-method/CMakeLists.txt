set(SOURCE_FILES
  main.cpp)

add_executable(template_method
  ${SOURCE_FILES})

target_link_libraries(template_method PRIVATE
  project_options
  project_warnings
)
