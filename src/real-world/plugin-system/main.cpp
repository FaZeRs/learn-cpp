#include <dlfcn.h>

#include <expected>
#include <filesystem>
#include <format>
#include <functional>
#include <map>
#include <memory>
#include <print>
#include <string>
#include <string_view>
#include <vector>

#include "plugin_interface.hpp"

// Factory function type for creating plugin instances
using CreatePluginFunc = IPlugin* (*)();
using DestroyPluginFunc = void (*)(IPlugin*);
using GetPluginInfoFunc = PluginInfo (*)();

// RAII wrapper for dynamic library handle
class PluginHandle {
 public:
  explicit PluginHandle(void* handle) : handle_(handle) {}

  ~PluginHandle() {
    if (handle_ != nullptr) {
      dlclose(handle_);
    }
  }

  PluginHandle(const PluginHandle&) = delete;
  PluginHandle& operator=(const PluginHandle&) = delete;

  PluginHandle(PluginHandle&& other) noexcept : handle_(other.handle_) {
    other.handle_ = nullptr;
  }

  PluginHandle& operator=(PluginHandle&& other) noexcept {
    if (this != &other) {
      if (handle_ != nullptr) {
        dlclose(handle_);
      }
      handle_ = other.handle_;
      other.handle_ = nullptr;
    }
    return *this;
  }

  [[nodiscard]] void* get() const { return handle_; }

  template <typename T>
  [[nodiscard]] T get_symbol(const char* name) const {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    return reinterpret_cast<T>(dlsym(handle_, name));
  }

 private:
  void* handle_;
};

// Loaded plugin container
struct LoadedPlugin {
  std::unique_ptr<PluginHandle> handle;
  std::unique_ptr<IPlugin, std::function<void(IPlugin*)>> instance;
  PluginInfo info;
  std::filesystem::path path;
};

// Plugin Manager
class PluginManager {
 public:
  PluginManager() = default;
  ~PluginManager() { unload_all(); }

  PluginManager(const PluginManager&) = delete;
  PluginManager& operator=(const PluginManager&) = delete;
  PluginManager(PluginManager&&) = delete;
  PluginManager& operator=(PluginManager&&) = delete;

  // Load a plugin from a shared library
  std::expected<void, std::string> load_plugin(
      const std::filesystem::path& path) {
    // Check if already loaded
    auto canonical = std::filesystem::canonical(path);
    if (plugins_.contains(canonical.string())) {
      return std::unexpected("Plugin already loaded");
    }

    // Open the shared library
    void* handle = dlopen(canonical.c_str(), RTLD_LAZY);
    if (handle == nullptr) {
      return std::unexpected(
          std::format("Failed to load plugin: {}", dlerror()));
    }

    auto plugin_handle = std::make_unique<PluginHandle>(handle);

    // Get plugin info function
    auto get_info =
        plugin_handle->get_symbol<GetPluginInfoFunc>("get_plugin_info");
    if (get_info == nullptr) {
      return std::unexpected("Plugin does not export get_plugin_info function");
    }

    // Get plugin info and check API version
    PluginInfo info = get_info();
    if (info.api_version != PLUGIN_API_VERSION) {
      return std::unexpected(
          std::format("Plugin API version mismatch: expected {}, got {}",
                      PLUGIN_API_VERSION, info.api_version));
    }

    // Get create and destroy functions
    auto create_plugin =
        plugin_handle->get_symbol<CreatePluginFunc>("create_plugin");
    auto destroy_plugin =
        plugin_handle->get_symbol<DestroyPluginFunc>("destroy_plugin");

    if (create_plugin == nullptr || destroy_plugin == nullptr) {
      return std::unexpected(
          "Plugin does not export required create/destroy functions");
    }

    // Create plugin instance
    IPlugin* plugin_ptr = create_plugin();
    if (plugin_ptr == nullptr) {
      return std::unexpected("Failed to create plugin instance");
    }

    // Initialize plugin
    if (!plugin_ptr->initialize()) {
      destroy_plugin(plugin_ptr);
      return std::unexpected("Plugin initialization failed");
    }

    // Store loaded plugin
    LoadedPlugin loaded;
    loaded.handle = std::move(plugin_handle);
    loaded.instance = std::unique_ptr<IPlugin, std::function<void(IPlugin*)>>(
        plugin_ptr, destroy_plugin);
    loaded.info = info;
    loaded.path = canonical;

    plugins_[canonical.string()] = std::move(loaded);

    std::println("Loaded plugin: {} v{}", info.name, info.version);
    return {};
  }

  // Unload a specific plugin
  bool unload_plugin(const std::filesystem::path& path) {
    auto canonical = std::filesystem::canonical(path);
    auto it = plugins_.find(canonical.string());
    if (it == plugins_.end()) {
      return false;
    }

    std::println("Unloading plugin: {}", it->second.info.name);
    it->second.instance->shutdown();
    plugins_.erase(it);
    return true;
  }

  // Unload all plugins
  void unload_all() {
    for (auto& [path, plugin] : plugins_) {
      std::println("Unloading plugin: {}", plugin.info.name);
      plugin.instance->shutdown();
    }
    plugins_.clear();
  }

  // Get a plugin by name
  [[nodiscard]] IPlugin* get_plugin(std::string_view name) const {
    for (const auto& [path, plugin] : plugins_) {
      if (plugin.info.name == name) {
        return plugin.instance.get();
      }
    }
    return nullptr;
  }

  // Get all loaded plugins
  [[nodiscard]] std::vector<IPlugin*> get_all_plugins() const {
    std::vector<IPlugin*> result;
    result.reserve(plugins_.size());
    for (const auto& [path, plugin] : plugins_) {
      result.push_back(plugin.instance.get());
    }
    return result;
  }

  // Get plugin count
  [[nodiscard]] size_t get_plugin_count() const { return plugins_.size(); }

  // List all loaded plugins
  void list_plugins() const {
    std::println("Loaded plugins ({}):", plugins_.size());
    for (const auto& [path, plugin] : plugins_) {
      std::println("  - {} v{}: {}", plugin.info.name, plugin.info.version,
                   plugin.info.description);
      std::println("    Path: {}", path);
    }
  }

  // Reload a plugin (unload and load again)
  std::expected<void, std::string> reload_plugin(
      const std::filesystem::path& path) {
    auto canonical = std::filesystem::canonical(path);
    if (!unload_plugin(canonical)) {
      return std::unexpected("Plugin not loaded");
    }
    return load_plugin(canonical);
  }

 private:
  std::map<std::string, LoadedPlugin> plugins_;
};

// Demo function to demonstrate plugin loading
void demonstrate_plugin_system() {
  std::println("=== Plugin System Demo ===\n");

  std::println("1. Creating Plugin Manager");
  std::println("---");
  PluginManager manager;
  std::println("Plugin Manager created successfully");
  std::println();

  // Try to load example plugin if it exists
  std::println("2. Loading example plugin from shared library");
  std::println("---");

  // Look for the plugin in common locations
  std::vector<std::filesystem::path> plugin_paths = {"./example_plugin.so"};

  bool plugin_loaded = false;
  for (const auto& path : plugin_paths) {
    if (std::filesystem::exists(path)) {
      std::println("Found plugin at: {}", path.string());
      auto result = manager.load_plugin(path);
      if (result) {
        plugin_loaded = true;
        std::println("Successfully loaded plugin from .so file!");
        break;
      } else {
        std::println("Failed to load: {}", result.error());
      }
    }
  }

  if (!plugin_loaded) {
    std::println("No compiled plugin found. To compile the example plugin:");
    std::println("  cd src/real-world/plugin-system");
    std::println(
        "  g++ -std=c++23 -shared -fPIC example_plugin.cpp -o "
        "example_plugin.so");
    std::println("  Then run this demo again.");
    std::println();
    std::println(
        "Continuing with inline plugin examples to demonstrate the "
        "system...");
  }

  std::println();

  // If we loaded a plugin, demonstrate using it
  if (plugin_loaded) {
    std::println("3. Using loaded plugin");
    std::println("---");

    manager.list_plugins();
    std::println();

    auto plugin = manager.get_plugin("Example");
    if (plugin != nullptr) {
      plugin->execute();
      std::println();
    }

    std::println("4. Demonstrating hot-reload");
    std::println("---");
    std::println("In a real application, you could modify the plugin source,");
    std::println("recompile it, and reload without restarting the app:");
    std::println("  manager.reload_plugin(\"example_plugin.so\")");
    std::println();

    std::println("5. Unloading plugin");
    std::println("---");
    for (const auto& path : plugin_paths) {
      // cppcheck-suppress useStlAlgorithm
      if (std::filesystem::exists(path)) {
        manager.unload_plugin(path);
        break;
      }
    }
    std::println();
  }

  std::println();

  // Demonstrate plugin manager API
  std::println("{}. Plugin Manager API", plugin_loaded ? 10 : 7);
  std::println("---");
  std::println("Key features:");
  std::println("  - load_plugin(path): Load plugin from .so file");
  std::println("  - unload_plugin(path): Unload specific plugin");
  std::println("  - reload_plugin(path): Hot-reload plugin");
  std::println("  - get_plugin(name): Get plugin instance by name");
  std::println("  - get_all_plugins(): Get all loaded plugins");
  std::println("  - list_plugins(): List all loaded plugins with info");
  std::println();

  std::println("{}. Plugin system architecture", plugin_loaded ? 11 : 8);
  std::println("---");
  std::println("Components:");
  std::println("  - IPlugin: Abstract interface for all plugins");
  std::println("  - PluginHandle: RAII wrapper for dlopen/dlclose");
  std::println("  - PluginManager: Manages plugin lifecycle");
  std::println("  - Factory functions: create_plugin/destroy_plugin");
  std::println();

  std::println("{}. Plugin API features", plugin_loaded ? 12 : 9);
  std::println("---");
  std::println("  - API version checking for compatibility");
  std::println("  - Dynamic library loading with dlopen/dlsym");
  std::println("  - RAII-based resource management");
  std::println("  - Hot-reloading capability");
  std::println("  - Error handling with std::expected");
  std::println();

  std::println("=== Demo Complete ===");
}

int main() {
  demonstrate_plugin_system();
  return 0;
}
