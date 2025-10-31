#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <map>
#include <print>
#include <ranges>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

/**
 * @brief Configuration manager with INI parsing, validation, and type-safe
 * access
 *
 * This demonstrates:
 * - INI file parsing with sections
 * - Type-safe value retrieval with std::expected
 * - Default values and validation
 * - Change notification callbacks
 * - Live reloading with filesystem monitoring
 * - Value constraints and validators
 */

/**
 * @brief Configuration errors
 */
enum class ConfigError : std::uint8_t {
  FileNotFound,
  ParseError,
  KeyNotFound,
  SectionNotFound,
  ValidationFailed,
  TypeConversionFailed,
  InvalidValue
};

/**
 * @brief Convert config error to string
 */
[[nodiscard]] constexpr std::string_view configErrorToString(
    ConfigError error) {
  switch (error) {
    case ConfigError::FileNotFound:
      return "Configuration file not found";
    case ConfigError::ParseError:
      return "Failed to parse configuration";
    case ConfigError::KeyNotFound:
      return "Key not found in configuration";
    case ConfigError::SectionNotFound:
      return "Section not found in configuration";
    case ConfigError::ValidationFailed:
      return "Configuration validation failed";
    case ConfigError::TypeConversionFailed:
      return "Failed to convert configuration value";
    case ConfigError::InvalidValue:
      return "Invalid configuration value";
    default:
      return "Unknown configuration error";
  }
}

// ============================================================================
// Configuration Value Types
// ============================================================================

/**
 * @brief Variant type for configuration values
 */
using ConfigValue = std::variant<std::string, int64_t, double, bool>;

/**
 * @brief Helper to convert string to typed value
 */
template <typename T>
[[nodiscard]] std::expected<T, ConfigError> parseValue(const std::string& str);

template <>
[[nodiscard]] std::expected<std::string, ConfigError> parseValue(
    const std::string& str) {
  return str;
}

template <>
[[nodiscard]] std::expected<int64_t, ConfigError> parseValue(
    const std::string& str) {
  int64_t value = 0;
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  if (ec != std::errc{} || ptr != str.data() + str.size()) {
    return std::unexpected(ConfigError::TypeConversionFailed);
  }
  return value;
}

template <>
[[nodiscard]] std::expected<double, ConfigError> parseValue(
    const std::string& str) {
  try {
    size_t pos = 0;
    double value = std::stod(str, &pos);
    if (pos != str.size()) {
      return std::unexpected(ConfigError::TypeConversionFailed);
    }
    return value;
  } catch (...) {
    return std::unexpected(ConfigError::TypeConversionFailed);
  }
}

template <>
[[nodiscard]] std::expected<bool, ConfigError> parseValue(
    const std::string& str) {
  std::string lower = str;
  std::ranges::transform(lower, lower.begin(),
                         [](unsigned char c) { return std::tolower(c); });

  if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
    return true;
  }
  if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
    return false;
  }
  return std::unexpected(ConfigError::TypeConversionFailed);
}

// ============================================================================
// Validation
// ============================================================================

/**
 * @brief Validator function type
 */
template <typename T>
using Validator = std::function<bool(const T&)>;

/**
 * @brief Common validators
 */
namespace validators {

template <typename T>
[[nodiscard]] Validator<T> range(T min_val, T max_val) {
  return [min_val, max_val](const T& value) {
    return value >= min_val && value <= max_val;
  };
}

template <typename T>
[[nodiscard]] Validator<T> min(T min_val) {
  return [min_val](const T& value) { return value >= min_val; };
}

template <typename T>
[[nodiscard]] Validator<T> max(T max_val) {
  return [max_val](const T& value) { return value <= max_val; };
}

[[nodiscard]] inline Validator<std::string> oneOf(
    const std::vector<std::string>& allowed) {
  return [allowed](const std::string& value) {
    return std::ranges::find(allowed, value) != allowed.end();
  };
}

[[nodiscard]] inline Validator<std::string> notEmpty() {
  return [](const std::string& value) { return !value.empty(); };
}

}  // namespace validators

// ============================================================================
// Configuration Manager
// ============================================================================

/**
 * @brief Configuration manager class
 */
class ConfigurationManager {
 public:
  /**
   * @brief Change callback type
   */
  using ChangeCallback = std::function<void(
      const std::string& section, const std::string& key,
      const std::string& old_value, const std::string& new_value)>;

  /**
   * @brief Load configuration from INI file
   */
  [[nodiscard]] std::expected<void, ConfigError> loadFromFile(
      const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
      return std::unexpected(ConfigError::FileNotFound);
    }

    std::ifstream file(path);
    if (!file) {
      return std::unexpected(ConfigError::FileNotFound);
    }

    config_path_ = path;
    last_write_time_ = std::filesystem::last_write_time(path);

    return parseINI(file);
  }

  /**
   * @brief Save configuration to file
   */
  [[nodiscard]] std::expected<void, ConfigError> saveToFile(
      const std::filesystem::path& path) const {
    std::ofstream file(path);
    if (!file) {
      return std::unexpected(ConfigError::FileNotFound);
    }

    for (const auto& [section, keys] : config_) {
      file << std::format("[{}]\n", section);
      for (const auto& [key, value] : keys) {
        file << std::format("{} = {}\n", key, value);
      }
      file << "\n";
    }

    return {};
  }

  /**
   * @brief Get string value
   */
  [[nodiscard]] std::expected<std::string, ConfigError> getString(
      const std::string& section, const std::string& key) const {
    return getValue(section, key);
  }

  /**
   * @brief Get string value with default
   */
  [[nodiscard]] std::string getString(const std::string& section,
                                      const std::string& key,
                                      const std::string& default_value) const {
    auto result = getValue(section, key);
    return result.value_or(default_value);
  }

  /**
   * @brief Get integer value
   */
  [[nodiscard]] std::expected<int64_t, ConfigError> getInt(
      const std::string& section, const std::string& key) const {
    auto str = getValue(section, key);
    if (!str) {
      return std::unexpected(str.error());
    }
    return parseValue<int64_t>(*str);
  }

  /**
   * @brief Get integer value with default
   */
  [[nodiscard]] int64_t getInt(const std::string& section,
                               const std::string& key,
                               int64_t default_value) const {
    auto result = getInt(section, key);
    return result.value_or(default_value);
  }

  /**
   * @brief Get double value
   */
  [[nodiscard]] std::expected<double, ConfigError> getDouble(
      const std::string& section, const std::string& key) const {
    auto str = getValue(section, key);
    if (!str) {
      return std::unexpected(str.error());
    }
    return parseValue<double>(*str);
  }

  /**
   * @brief Get double value with default
   */
  [[nodiscard]] double getDouble(const std::string& section,
                                 const std::string& key,
                                 double default_value) const {
    auto result = getDouble(section, key);
    return result.value_or(default_value);
  }

  /**
   * @brief Get boolean value
   */
  [[nodiscard]] std::expected<bool, ConfigError> getBool(
      const std::string& section, const std::string& key) const {
    auto str = getValue(section, key);
    if (!str) {
      return std::unexpected(str.error());
    }
    return parseValue<bool>(*str);
  }

  /**
   * @brief Get boolean value with default
   */
  [[nodiscard]] bool getBool(const std::string& section, const std::string& key,
                             bool default_value) const {
    auto result = getBool(section, key);
    return result.value_or(default_value);
  }

  /**
   * @brief Set value
   */
  void set(const std::string& section, const std::string& key,
           const std::string& value) {
    std::string old_value;
    if (auto it = config_.find(section); it != config_.end()) {
      if (auto key_it = it->second.find(key); key_it != it->second.end()) {
        old_value = key_it->second;
      }
    }

    config_[section][key] = value;

    // Notify listeners
    if (!old_value.empty() || !value.empty()) {
      notifyChange(section, key, old_value, value);
    }
  }

  /**
   * @brief Set integer value
   */
  void setInt(const std::string& section, const std::string& key,
              int64_t value) {
    set(section, key, std::to_string(value));
  }

  /**
   * @brief Set double value
   */
  void setDouble(const std::string& section, const std::string& key,
                 double value) {
    set(section, key, std::to_string(value));
  }

  /**
   * @brief Set boolean value
   */
  void setBool(const std::string& section, const std::string& key, bool value) {
    set(section, key, value ? "true" : "false");
  }

  /**
   * @brief Check if section exists
   */
  [[nodiscard]] bool hasSection(const std::string& section) const {
    return config_.contains(section);
  }

  /**
   * @brief Check if key exists in section
   */
  [[nodiscard]] bool hasKey(const std::string& section,
                            const std::string& key) const {
    if (auto it = config_.find(section); it != config_.end()) {
      return it->second.contains(key);
    }
    return false;
  }

  /**
   * @brief Get all sections
   */
  [[nodiscard]] std::vector<std::string> getSections() const {
    std::vector<std::string> sections;
    sections.reserve(config_.size());
    for (const auto& [section, _] : config_) {
      sections.push_back(section);
    }
    return sections;
  }

  /**
   * @brief Get all keys in a section
   */
  [[nodiscard]] std::vector<std::string> getKeys(
      const std::string& section) const {
    std::vector<std::string> keys;
    if (auto it = config_.find(section); it != config_.end()) {
      for (const auto& [key, _] : it->second) {
        keys.push_back(key);
      }
    }
    return keys;
  }

  /**
   * @brief Register change callback
   */
  void onChanged(ChangeCallback callback) {
    callbacks_.push_back(std::move(callback));
  }

  /**
   * @brief Check if file has been modified and reload
   */
  [[nodiscard]] bool checkAndReload() {
    if (config_path_.empty() || !std::filesystem::exists(config_path_)) {
      return false;
    }

    auto current_time = std::filesystem::last_write_time(config_path_);
    if (current_time != last_write_time_) {
      std::println("[ConfigManager] Configuration file changed, reloading...");
      auto result = loadFromFile(config_path_);
      return result.has_value();
    }

    return false;
  }

  /**
   * @brief Validate configuration with validators
   */
  template <typename T>
  [[nodiscard]] bool validate(const std::string& section,
                              const std::string& key,
                              const Validator<T>& validator) const {
    if constexpr (std::is_same_v<T, std::string>) {
      auto value = getString(section, key);
      return value && validator(*value);
    } else if constexpr (std::is_same_v<T, int64_t>) {
      auto value = getInt(section, key);
      return value && validator(*value);
    } else if constexpr (std::is_same_v<T, double>) {
      auto value = getDouble(section, key);
      return value && validator(*value);
    } else if constexpr (std::is_same_v<T, bool>) {
      auto value = getBool(section, key);
      return value && validator(*value);
    }
    return false;
  }

  /**
   * @brief Print configuration
   */
  void print() const {
    for (const auto& [section, keys] : config_) {
      std::println("[{}]", section);
      for (const auto& [key, value] : keys) {
        std::println("  {} = {}", key, value);
      }
      std::println("");
    }
  }

 private:
  /**
   * @brief Parse INI format
   */
  [[nodiscard]] std::expected<void, ConfigError> parseINI(std::ifstream& file) {
    config_.clear();
    std::string current_section = "default";
    std::string line;
    int line_number = 0;

    while (std::getline(file, line)) {
      ++line_number;

      // Remove whitespace
      line = trim(line);

      // Skip empty lines and comments
      if (line.empty() || line[0] == '#' || line[0] == ';') {
        continue;
      }

      // Section header
      if (line[0] == '[' && line.back() == ']') {
        current_section = line.substr(1, line.size() - 2);
        current_section = trim(current_section);
        continue;
      }

      // Key-value pair
      size_t pos = line.find('=');
      if (pos != std::string::npos) {
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));

        // Remove quotes if present
        if (value.size() >= 2 &&
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
          value = value.substr(1, value.size() - 2);
        }

        config_[current_section][key] = value;
      } else {
        std::println("Warning: Invalid line {} in config file", line_number);
      }
    }

    return {};
  }

  /**
   * @brief Get raw value as string
   */
  [[nodiscard]] std::expected<std::string, ConfigError> getValue(
      const std::string& section, const std::string& key) const {
    auto section_it = config_.find(section);
    if (section_it == config_.end()) {
      return std::unexpected(ConfigError::SectionNotFound);
    }

    auto key_it = section_it->second.find(key);
    if (key_it == section_it->second.end()) {
      return std::unexpected(ConfigError::KeyNotFound);
    }

    return key_it->second;
  }

  /**
   * @brief Trim whitespace from string
   */
  [[nodiscard]] static std::string trim(const std::string& str) {
    auto start = std::ranges::find_if(
        str, [](unsigned char ch) { return !std::isspace(ch); });
    auto end = std::ranges::find_if(
        str | std::views::reverse,
        [](unsigned char ch) { return !std::isspace(ch); });
    return {start, end.base()};
  }

  /**
   * @brief Notify change listeners
   */
  void notifyChange(const std::string& section, const std::string& key,
                    const std::string& old_value,
                    const std::string& new_value) const {
    for (const auto& callback : callbacks_) {
      callback(section, key, old_value, new_value);
    }
  }

  // Section -> Key -> Value
  std::map<std::string, std::map<std::string, std::string>> config_;
  std::vector<ChangeCallback> callbacks_;
  std::filesystem::path config_path_;
  std::filesystem::file_time_type last_write_time_;
};

// ============================================================================
// Demonstrations
// ============================================================================

/**
 * @brief Create a sample configuration file
 */
void createSampleConfig(const std::filesystem::path& path) {
  std::ofstream file(path);
  file << R"([application]
name = MyApp
version = 1.2.3
debug = true

[server]
host = 127.0.0.1
port = 8080
max_connections = 100
timeout = 30.0

[database]
driver = postgresql
host = localhost
port = 5432
name = mydb
username = admin
password = secret123

[features]
enable_analytics = true
enable_logging = true
enable_cache = false
cache_size = 1024

[limits]
max_upload_size = 10485760
max_users = 1000
rate_limit = 100
)";
  std::println("Created sample config: {}", path.string());
}

/**
 * @brief Demonstrate basic configuration loading and access
 */
void demonstrateBasicUsage() {
  std::println("\n=== Basic Configuration Usage ===\n");

  auto config_path = std::filesystem::temp_directory_path() / "config_demo.ini";
  createSampleConfig(config_path);

  ConfigurationManager config;
  auto result = config.loadFromFile(config_path);

  if (!result) {
    std::println("Failed to load config: {}",
                 configErrorToString(result.error()));
    return;
  }

  std::println("Configuration loaded successfully!\n");

  // Access string values
  if (auto name = config.getString("application", "name")) {
    std::println("Application name: {}", *name);
  }

  if (auto version = config.getString("application", "version")) {
    std::println("Version: {}", *version);
  }

  // Access integer values
  if (auto port = config.getInt("server", "port")) {
    std::println("Server port: {}", *port);
  }

  if (auto max_conn = config.getInt("server", "max_connections")) {
    std::println("Max connections: {}", *max_conn);
  }

  // Access double values
  if (auto timeout = config.getDouble("server", "timeout")) {
    std::println("Timeout: {:.1f}s", *timeout);
  }

  // Access boolean values
  if (auto debug = config.getBool("application", "debug")) {
    std::println("Debug mode: {}", *debug ? "enabled" : "disabled");
  }

  // Clean up
  std::filesystem::remove(config_path);
}

/**
 * @brief Demonstrate default values
 */
void demonstrateDefaultValues() {
  std::println("\n=== Default Values ===\n");

  auto config_path =
      std::filesystem::temp_directory_path() / "config_defaults.ini";
  createSampleConfig(config_path);

  ConfigurationManager config;
  (void)config.loadFromFile(config_path);

  // Existing values
  std::println("Existing port: {}", config.getInt("server", "port", 3000));

  // Non-existent values with defaults
  std::println("Non-existent timeout: {}",
               config.getInt("server", "nonexistent", 60));
  std::println("Non-existent feature: {}",
               config.getBool("features", "nonexistent", false));
  std::println("Non-existent name: {}",
               config.getString("app", "nonexistent", "DefaultName"));

  std::filesystem::remove(config_path);
}

/**
 * @brief Demonstrate validation
 */
void demonstrateValidation() {
  std::println("\n=== Configuration Validation ===\n");

  auto config_path =
      std::filesystem::temp_directory_path() / "config_validation.ini";
  createSampleConfig(config_path);

  ConfigurationManager config;
  (void)config.loadFromFile(config_path);

  // Validate port range
  bool port_valid = config.validate<int64_t>(
      "server", "port", validators::range<int64_t>(1, 65535));
  std::println("Port valid (1-65535): {}", port_valid);

  // Validate max connections
  bool conn_valid = config.validate<int64_t>("server", "max_connections",
                                             validators::min<int64_t>(1));
  std::println("Max connections valid (>= 1): {}", conn_valid);

  // Validate timeout
  bool timeout_valid = config.validate<double>(
      "server", "timeout", validators::range<double>(0.0, 300.0));
  std::println("Timeout valid (0-300s): {}", timeout_valid);

  // Validate database driver
  bool driver_valid = config.validate<std::string>(
      "database", "driver",
      validators::oneOf({"postgresql", "mysql", "sqlite"}));
  std::println("Database driver valid: {}", driver_valid);

  std::filesystem::remove(config_path);
}

/**
 * @brief Demonstrate change notifications
 */
void demonstrateChangeNotifications() {
  std::println("\n=== Change Notifications ===\n");

  auto config_path =
      std::filesystem::temp_directory_path() / "config_changes.ini";
  createSampleConfig(config_path);

  ConfigurationManager config;
  (void)config.loadFromFile(config_path);

  // Register change callback
  config.onChanged([](const std::string& section, const std::string& key,
                      const std::string& old_value,
                      const std::string& new_value) {
    std::println("[Change] [{}.{}] '{}' -> '{}'", section, key, old_value,
                 new_value);
  });

  std::println("Changing server port...");
  config.setInt("server", "port", 9090);

  std::println("\nEnabling debug mode...");
  config.setBool("application", "debug", false);

  std::println("\nChanging application name...");
  config.set("application", "name", "UpdatedApp");

  std::filesystem::remove(config_path);
}

/**
 * @brief Demonstrate section and key introspection
 */
void demonstrateIntrospection() {
  std::println("\n=== Configuration Introspection ===\n");

  auto config_path =
      std::filesystem::temp_directory_path() / "config_introspection.ini";
  createSampleConfig(config_path);

  ConfigurationManager config;
  (void)config.loadFromFile(config_path);

  std::println("All sections:");
  for (const auto& section : config.getSections()) {
    std::println("  [{}]", section);
  }

  std::println("\nKeys in 'server' section:");
  for (const auto& key : config.getKeys("server")) {
    std::println("  - {}", key);
  }

  std::println("\nChecking key existence:");
  std::println("  server.port exists: {}",
               config.hasKey("server", "port") ? "yes" : "no");
  std::println("  server.invalid exists: {}",
               config.hasKey("server", "invalid") ? "yes" : "no");

  std::filesystem::remove(config_path);
}

/**
 * @brief Demonstrate saving configuration
 */
void demonstrateSaving() {
  std::println("\n=== Saving Configuration ===\n");

  ConfigurationManager config;

  // Create configuration programmatically
  config.set("app", "name", "TestApp");
  config.set("app", "version", "2.0.0");
  config.setInt("app", "build", 12345);
  config.setBool("app", "production", true);

  config.set("server", "host", "0.0.0.0");
  config.setInt("server", "port", 443);
  config.setDouble("server", "timeout", 60.5);

  auto save_path = std::filesystem::temp_directory_path() / "saved_config.ini";
  auto result = config.saveToFile(save_path);

  if (result) {
    std::println("Configuration saved to: {}", save_path.string());
    std::println("\nSaved content:");
    config.print();
  } else {
    std::println("Failed to save configuration");
  }

  std::filesystem::remove(save_path);
}

/**
 * @brief Main demonstration
 */
void demo() {
  demonstrateBasicUsage();
  demonstrateDefaultValues();
  demonstrateValidation();
  demonstrateChangeNotifications();
  demonstrateIntrospection();
  demonstrateSaving();

  std::println("\n=== Demo Complete ===\n");
}

int main() {
  std::println("Configuration Manager Demonstration");
  std::println("===================================");

  try {
    demo();
  } catch (const std::exception& e) {
    std::println("Error during demonstration: {}", e.what());
    return 1;
  }

  return 0;
}
