#ifndef PLUGIN_INTERFACE_HPP
#define PLUGIN_INTERFACE_HPP

#include <cstdint>
#include <string>

// Plugin API version for compatibility checking
constexpr std::uint32_t PLUGIN_API_VERSION = 1;

// Plugin interface that all plugins must implement
class IPlugin {
 public:
  virtual ~IPlugin() = default;

  IPlugin(const IPlugin&) = delete;
  IPlugin& operator=(const IPlugin&) = delete;
  IPlugin(IPlugin&&) = delete;
  IPlugin& operator=(IPlugin&&) = delete;

  // Get plugin name
  [[nodiscard]] virtual std::string get_name() const = 0;

  // Get plugin version
  [[nodiscard]] virtual std::string get_version() const = 0;

  // Initialize the plugin
  virtual bool initialize() = 0;

  // Shutdown the plugin
  virtual void shutdown() = 0;

  // Execute plugin-specific functionality
  virtual void execute() = 0;

 protected:
  IPlugin() = default;
};

// Plugin metadata
struct PluginInfo {
  std::uint32_t api_version{PLUGIN_API_VERSION};
  std::string name;
  std::string version;
  std::string description;
};

// Macro to define plugin export functions
// Usage in plugin .cpp file:
//   DECLARE_PLUGIN(YourPluginClass, "PluginName", "1.0.0", "Description")
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define DECLARE_PLUGIN(PluginClass, Name, Version, Description)               \
  _Pragma("GCC diagnostic push")                                               \
  _Pragma("GCC diagnostic ignored \"-Wreturn-type-c-linkage\"")               \
  _Pragma("clang diagnostic push")                                             \
  _Pragma("clang diagnostic ignored \"-Wreturn-type-c-linkage\"")             \
  extern "C" {                                                                 \
  PluginInfo get_plugin_info() {                                               \
    return {PLUGIN_API_VERSION, Name, Version, Description};                   \
  }                                                                            \
  /* NOLINTNEXTLINE(cppcoreguidelines-owning-memory) */                       \
  IPlugin* create_plugin() { return new PluginClass(); }                       \
  /* NOLINTNEXTLINE(cppcoreguidelines-owning-memory) */                       \
  void destroy_plugin(IPlugin* plugin) { delete plugin; }                      \
  }                                                                            \
  _Pragma("clang diagnostic pop")                                              \
  _Pragma("GCC diagnostic pop")

#endif  // PLUGIN_INTERFACE_HPP
