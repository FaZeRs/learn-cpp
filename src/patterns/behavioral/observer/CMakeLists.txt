set(SOURCE_FILES
  main.cpp)

add_executable(observer
  ${SOURCE_FILES})

target_link_libraries(observer PRIVATE
  project_options
  project_warnings
)
