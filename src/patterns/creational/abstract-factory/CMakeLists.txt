set(SOURCE_FILES
  main.cpp)

add_executable(abstract_factory
  ${SOURCE_FILES})

target_link_libraries(abstract_factory PRIVATE
  project_options
  project_warnings
)
