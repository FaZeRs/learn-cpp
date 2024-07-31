set(SOURCE_FILES
  main.cpp)

add_executable(memory-pools
  ${SOURCE_FILES})

target_link_libraries(memory-pools PRIVATE
  project_options
  project_warnings
)
