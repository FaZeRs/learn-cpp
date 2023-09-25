set(SOURCE_FILES
  main.cpp)

add_executable(queue
  ${SOURCE_FILES})

target_link_libraries(queue PRIVATE
  project_options
  project_warnings
)
