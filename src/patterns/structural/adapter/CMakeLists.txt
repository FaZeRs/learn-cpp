set(SOURCE_FILES
  main.cpp)

add_executable(adapter
  ${SOURCE_FILES})

target_link_libraries(adapter PRIVATE
  project_options
  project_warnings
)
