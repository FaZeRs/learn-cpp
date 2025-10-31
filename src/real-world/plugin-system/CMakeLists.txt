# Main plugin system executable
add_executable(plugin-system main.cpp)

target_compile_features(plugin-system PRIVATE cxx_std_23)

# Link with dl library for dynamic loading
target_link_libraries(plugin-system PRIVATE ${CMAKE_DL_LIBS})

# Example plugin as a shared library
add_library(example_plugin SHARED example_plugin.cpp)

target_compile_features(example_plugin PRIVATE cxx_std_23)

# Output the plugin to the bin directory for easy loading
set_target_properties(
  example_plugin
  PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
             PREFIX ""
             OUTPUT_NAME "example_plugin")
