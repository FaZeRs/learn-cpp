set(SOURCE_FILES
  main.cpp)

add_executable(heap
  ${SOURCE_FILES})

target_link_libraries(heap PRIVATE
  project_options
  project_warnings
)
