set(SOURCE_FILES
  main.cpp)

add_executable(prototype
  ${SOURCE_FILES})

target_link_libraries(prototype PRIVATE
  project_options
  project_warnings
)
