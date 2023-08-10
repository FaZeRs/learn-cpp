set(SOURCE_FILES
  main.cpp)

add_executable(singleton
  ${SOURCE_FILES})

target_link_libraries(singleton PRIVATE
  project_options
  project_warnings
)
