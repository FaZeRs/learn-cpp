set(SOURCE_FILES
  main.cpp)

add_executable(factory_method
  ${SOURCE_FILES})

target_link_libraries(factory_method PRIVATE
  project_options
  project_warnings
)
