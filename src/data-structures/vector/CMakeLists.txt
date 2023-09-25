set(SOURCE_FILES
  main.cpp)

add_executable(vector
  ${SOURCE_FILES})

target_link_libraries(vector PRIVATE
  project_options
  project_warnings
)
