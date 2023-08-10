set(SOURCE_FILES
  main.cpp)

add_executable(flyweight
  ${SOURCE_FILES})

target_link_libraries(flyweight PRIVATE
  project_options
  project_warnings
)
