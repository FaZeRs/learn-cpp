set(SOURCE_FILES
  main.cpp)

add_executable(bridge
  ${SOURCE_FILES})

target_link_libraries(bridge PRIVATE
  project_options
  project_warnings
)
