set(SOURCE_FILES
  main.cpp)

add_executable(iterator
  ${SOURCE_FILES})

target_link_libraries(iterator PRIVATE
  project_options
  project_warnings
)
