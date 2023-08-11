set(SOURCE_FILES
  main.cpp)

find_package(Threads REQUIRED)

add_executable(singleton
  ${SOURCE_FILES})

target_link_libraries(singleton PRIVATE
  project_options
  project_warnings
  Threads::Threads
)
