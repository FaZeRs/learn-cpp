# Plugin System

A comprehensive plugin system demonstrating dynamic library loading, plugin interfaces, and hot-reloading capabilities.

## Features

- **Dynamic Loading**: Load plugins from shared libraries (.so files) at runtime using `dlopen`/`dlsym`
- **Plugin Interface**: Abstract base class (`IPlugin`) that all plugins must implement
- **Version Checking**: API version compatibility checking to ensure plugin compatibility
- **Plugin Manager**: Centralized management of plugin lifecycle (load, unload, reload)
- **Hot-Reloading**: Ability to reload plugins without restarting the application
- **RAII Design**: Automatic resource management with smart pointers
- **Error Handling**: Comprehensive error handling using `std::expected`

## Architecture

### Components

1. **IPlugin** - Abstract interface that all plugins must implement
   - `get_name()` - Returns plugin name
   - `get_version()` - Returns plugin version
   - `initialize()` - Initialize plugin resources
   - `shutdown()` - Clean up plugin resources
   - `execute()` - Run plugin-specific functionality

2. **PluginHandle** - RAII wrapper for `dlopen`/`dlclose`
   - Automatically closes dynamic library on destruction
   - Provides symbol lookup functionality

3. **PluginManager** - Manages the plugin ecosystem
   - `load_plugin(path)` - Load a plugin from a shared library
   - `unload_plugin(path)` - Unload a specific plugin
   - `reload_plugin(path)` - Hot-reload a plugin
   - `get_plugin(name)` - Retrieve a plugin by name
   - `get_all_plugins()` - Get all loaded plugins
   - `list_plugins()` - Display information about loaded plugins

4. **PluginInfo** - Metadata about a plugin
   - API version
   - Name and version
   - Description

## Creating a Plugin

### Step 1: Include the Interface

```cpp
#include "plugin_interface.hpp"
#include <print>
```

### Step 2: Implement IPlugin

```cpp
class MyPlugin : public IPlugin {
 public:
  std::string get_name() const override { return "MyPlugin"; }
  std::string get_version() const override { return "1.0.0"; }

  bool initialize() override {
    std::println("[MyPlugin] Initializing...");
    // Initialize resources
    return true;
  }

  void shutdown() override {
    std::println("[MyPlugin] Shutting down...");
    // Clean up resources
  }

  void execute() override {
    std::println("[MyPlugin] Executing...");
    // Plugin functionality
  }
};
```

### Step 3: Export Plugin Functions

```cpp
// Use the convenient macro
DECLARE_PLUGIN(MyPlugin, "MyPlugin", "1.0.0", "Description of my plugin")

// Or manually:
extern "C" {
  PluginInfo get_plugin_info() {
    return {PLUGIN_API_VERSION, "MyPlugin", "1.0.0", "Description"};
  }

  IPlugin* create_plugin() {
    return new MyPlugin();
  }

  void destroy_plugin(IPlugin* plugin) {
    delete plugin;
  }
}
```

### Step 4: Compile as Shared Library

```bash
g++ -std=c++23 -shared -fPIC my_plugin.cpp -o my_plugin.so
```

Or with CMake:

```cmake
add_library(my_plugin SHARED my_plugin.cpp)
target_compile_features(my_plugin PRIVATE cxx_std_23)
```

## Using the Plugin System

```cpp
#include "plugin_manager.hpp"

int main() {
  PluginManager manager;

  // Load a plugin
  auto result = manager.load_plugin("my_plugin.so");
  if (!result) {
    std::println("Failed to load: {}", result.error());
    return 1;
  }

  // Get and use the plugin
  IPlugin* plugin = manager.get_plugin("MyPlugin");
  if (plugin) {
    plugin->execute();
  }

  // List all plugins
  manager.list_plugins();

  // Hot-reload a plugin
  manager.reload_plugin("my_plugin.so");

  // Unload when done
  manager.unload_plugin("my_plugin.so");

  return 0;
}
```

## Building the Demo

```bash
cd build
cmake --build . --target real-world-plugin-system
./bin/real-world-plugin-system
```

## Use Cases

- **Application Extensions**: Add functionality without modifying core application
- **Modular Architecture**: Separate concerns into independent modules
- **Third-Party Integrations**: Allow external developers to extend your application
- **Hot-Reloading**: Update plugins without restarting the application
- **A/B Testing**: Swap different implementations at runtime
- **Plugin Marketplaces**: Enable users to install additional features

## Technical Details

### Dynamic Loading

The plugin system uses POSIX `dlopen`, `dlsym`, and `dlclose` functions:
- `dlopen(path, RTLD_LAZY)` - Load shared library
- `dlsym(handle, symbol)` - Get function pointer from library
- `dlclose(handle)` - Unload library

### Symbol Export

Plugins must export C symbols (not C++ mangled names):
```cpp
extern "C" {
  IPlugin* create_plugin();
  void destroy_plugin(IPlugin*);
  PluginInfo get_plugin_info();
}
```

### API Versioning

The system checks API versions to ensure compatibility:
- Plugin exports its API version via `get_plugin_info()`
- PluginManager compares with `PLUGIN_API_VERSION`
- Incompatible plugins are rejected with error message

## Complexity

- **Time Complexity**:
  - Load plugin: O(1) for lookup + OS-dependent dlopen cost
  - Get plugin: O(n) where n = number of loaded plugins
  - List plugins: O(n)

- **Space Complexity**:
  - O(n) where n = number of loaded plugins
  - Each plugin stores: handle, instance, metadata, path

## Thread Safety

The current implementation is **not thread-safe**. For concurrent access:
- Add mutex locks around plugin map operations
- Use `std::shared_mutex` for read-write locking
- Consider per-plugin locks for `execute()` calls

## Limitations

- Linux/POSIX only (uses `dlopen`/`dlsym`)
- No built-in sandboxing or security isolation
- No dependency management between plugins
- API changes require version bump and recompilation

## Future Enhancements

- Windows support (LoadLibrary/GetProcAddress)
- Plugin dependency management
- Plugin configuration files
- Asynchronous plugin loading
- Plugin marketplace/repository
- Security sandboxing
- Automatic plugin discovery in directories
