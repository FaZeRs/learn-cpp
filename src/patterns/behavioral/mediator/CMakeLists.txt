set(SOURCE_FILES
  main.cpp)

add_executable(mediator
  ${SOURCE_FILES})

target_link_libraries(mediator PRIVATE
  project_options
  project_warnings
)
