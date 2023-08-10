set(SOURCE_FILES
  main.cpp)

add_executable(chain_of_responsibility
  ${SOURCE_FILES})

target_link_libraries(chain_of_responsibility PRIVATE
  project_options
  project_warnings
)
