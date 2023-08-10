set(SOURCE_FILES
  main.cpp)

add_executable(facade
  ${SOURCE_FILES})

target_link_libraries(facade PRIVATE
  project_options
  project_warnings
)
