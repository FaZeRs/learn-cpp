set(SOURCE_FILES
  main.cpp)

add_executable(visitor
  ${SOURCE_FILES})

target_link_libraries(visitor PRIVATE
  project_options
  project_warnings
)
