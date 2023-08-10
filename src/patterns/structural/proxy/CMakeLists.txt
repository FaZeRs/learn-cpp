set(SOURCE_FILES
  main.cpp)

add_executable(proxy
  ${SOURCE_FILES})

target_link_libraries(proxy PRIVATE
  project_options
  project_warnings
)
