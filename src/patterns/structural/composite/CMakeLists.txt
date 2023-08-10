set(SOURCE_FILES
  main.cpp)

add_executable(composite
  ${SOURCE_FILES})

target_link_libraries(composite PRIVATE
  project_options
  project_warnings
)
