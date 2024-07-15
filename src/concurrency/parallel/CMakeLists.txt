set(SOURCE_FILES
  main.cpp)

add_executable(parallel
  ${SOURCE_FILES})

target_link_libraries(parallel PRIVATE
  project_options
  project_warnings
)
