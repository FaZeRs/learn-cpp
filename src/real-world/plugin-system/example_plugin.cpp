// Example plugin implementation
// This file demonstrates how to create a plugin that can be compiled as a
// shared library (.so) and loaded dynamically by the plugin system.
//
// To compile this plugin:
//   g++ -std=c++23 -shared -fPIC example_plugin.cpp -o example_plugin.so
//
// To use in the plugin system:
//   PluginManager manager;
//   manager.load_plugin("example_plugin.so");

#include "plugin_interface.hpp"

#include <print>

class ExamplePlugin : public IPlugin {
 public:
  [[nodiscard]] std::string get_name() const override { return "Example"; }

  [[nodiscard]] std::string get_version() const override { return "1.0.0"; }

  bool initialize() override {
    std::println("[ExamplePlugin] Initializing...");
    // Perform initialization tasks here
    return true;
  }

  void shutdown() override {
    std::println("[ExamplePlugin] Shutting down...");
    // Perform cleanup tasks here
  }

  void execute() override {
    std::println("[ExamplePlugin] Executing plugin functionality...");
    std::println("[ExamplePlugin] Hello from the example plugin!");
    // Implement plugin-specific functionality here
  }
};

// Export the plugin using the macro
DECLARE_PLUGIN(ExamplePlugin, "Example", "1.0.0",
               "An example plugin demonstrating the plugin system")
