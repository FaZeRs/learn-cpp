set(SOURCE_FILES
  main.cpp)

add_executable(builder
  ${SOURCE_FILES})

target_link_libraries(builder PRIVATE
  project_options
  project_warnings
)
