set(SOURCE_FILES
  main.cpp)

add_executable(memento
  ${SOURCE_FILES})

target_link_libraries(memento PRIVATE
  project_options
  project_warnings
)
