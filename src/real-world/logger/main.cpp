#include <chrono>
#include <concepts>
#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <mutex>
#include <print>
#include <queue>
#include <ranges>
#include <source_location>
#include <string_view>
#include <thread>

enum class LogColor {
  Reset = 0,
  Red = 31,
  Green = 32,
  Yellow = 33,
  Blue = 34,
  Magenta = 35,
  White = 37
};

enum class LogLevel { Debug, Info, Warning, Error, Critical };

template <typename... Args>
concept ValidFormatString =
    requires(std::format_string<Args...> fmt, Args... args) {
      {
        std::format(fmt, std::forward<Args>(args)...)
      } -> std::convertible_to<std::string>;
    };

template <typename... Args>
struct FormatValidator {
  static constexpr bool validate(std::format_string<Args...>) { return true; }
};

template <typename... Args>
struct LogMessage {
  std::source_location location;
  std::format_string<Args...> fmt;
  std::tuple<Args...> args;

  constexpr LogMessage(const std::source_location& loc,
                       std::format_string<Args...> format, Args&&... arguments)
      : location(loc), fmt(format), args(std::forward<Args>(arguments)...) {}
};

class Logger {
 public:
  struct Config {
    LogLevel min_level = LogLevel::Debug;
    std::string log_file;
    size_t max_file_size = 10 * 1024 * 1024;  // 10MB
    size_t max_files = 5;
    bool console_output = true;
    std::string_view message_format = "[{level}] {message}";
    size_t buffer_size = 1024;
    bool async_processing = true;
    std::chrono::milliseconds flush_interval{1000};
  };

  static Logger& getInstance() {
    static Logger instance;
    return instance;
  }

  ~Logger() {
    processing_enabled_ = false;
    if (worker_thread_.joinable()) {
      worker_thread_.request_stop();
      worker_thread_.join();
    }
    flush();
  }

  void flush() {
    std::scoped_lock lock(mutex_);
    processLogs(true);
  }

  std::expected<void, std::error_code> configure(const Config& config) {
    std::scoped_lock lock(mutex_);
    config_ = config;
    if (!config_.log_file.empty()) {
      file_output_.open(config_.log_file, std::ios::app);
      if (!file_output_) {
        return std::unexpected(std::make_error_code(std::errc::io_error));
      }
    }
    if (config_.async_processing) {
      startWorker();
    }
    return {};
  }

  template <typename... Args>
    requires ValidFormatString<Args...>
  constexpr void log(LogLevel level, LogMessage<Args...>&& message) {
    if (level < config_.min_level) return;

    const auto formatted = std::apply(
        [&](auto&&... args) {
          return std::format(message.fmt, std::forward<Args>(args)...);
        },
        message.args);

    LogEntry entry{.timestamp = std::chrono::system_clock::now(),
                   .level = level,
                   .message = std::move(formatted),
                   .file = message.location.file_name(),
                   .line = message.location.line(),
                   .function = message.location.function_name(),
                   .thread_id = std::this_thread::get_id()};

    std::scoped_lock lock(mutex_);
    log_queue_.push(std::move(entry));
    if (!config_.async_processing) {
      processLogs();
    }
  }

  template <typename... Args>
  constexpr void debug(std::format_string<Args...> fmt, Args&&... args) {
    log(LogLevel::Debug, LogMessage<Args...>(std::source_location::current(),
                                             fmt, std::forward<Args>(args)...));
  }

  template <typename... Args>
  constexpr void info(std::format_string<Args...> fmt, Args&&... args) {
    log(LogLevel::Info, LogMessage<Args...>(std::source_location::current(),
                                            fmt, std::forward<Args>(args)...));
  }

  template <typename... Args>
  constexpr void error(std::format_string<Args...> fmt, Args&&... args) {
    log(LogLevel::Error, LogMessage<Args...>(std::source_location::current(),
                                             fmt, std::forward<Args>(args)...));
  }

  template <typename... Args>
  constexpr void warning(std::format_string<Args...> fmt, Args&&... args) {
    log(LogLevel::Warning,
        LogMessage<Args...>(std::source_location::current(), fmt,
                            std::forward<Args>(args)...));
  }

  template <typename... Args>
  constexpr void critical(std::format_string<Args...> fmt, Args&&... args) {
    log(LogLevel::Critical,
        LogMessage<Args...>(std::source_location::current(), fmt,
                            std::forward<Args>(args)...));
  }

 private:
  Logger() = default;

  struct LogEntry {
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    std::string message;
    std::string_view file;
    uint_least32_t line;
    std::string_view function;
    std::thread::id thread_id;

    auto operator<=>(const LogEntry&) const = default;
  };

  void processLogs(const bool force = false) {
    if (!force && log_queue_.size() < config_.buffer_size) {
      return;
    }

    std::vector<LogEntry> batch;
    const size_t batch_size = std::min(
        log_queue_.size(), force ? log_queue_.size() : config_.buffer_size);
    batch.reserve(batch_size);

    for (size_t i = 0; i < batch_size; ++i) {
      batch.push_back(std::move(log_queue_.front()));
      log_queue_.pop();
    }

    std::span batch_span{batch};
    for (const auto& entry : batch_span) {
      auto [color, level_str] = getLevelInfo(entry.level);
      std::string formatted_message = formatLogMessage(entry, level_str);

      if (config_.console_output) {
        std::println("\033[{}m{}\033[0m", static_cast<int>(color),
                     formatted_message);
      }

      if (file_output_.is_open()) {
        writeToFile(formatted_message);
      }
    }

    logs_.insert(logs_.end(), std::make_move_iterator(batch.begin()),
                 std::make_move_iterator(batch.end()));
  }

  void writeToFile(const std::string& message) {
    if (shouldRotateLog()) {
      rotateLog();
    }
    file_output_ << message << std::endl;
  }

  bool shouldRotateLog() {
    if (!file_output_.is_open()) return false;
    file_output_.seekp(0, std::ios::end);
    return static_cast<size_t>(file_output_.tellp()) >= config_.max_file_size;
  }

  void rotateLog() {
    file_output_.close();

    // Rotate existing log files
    for (size_t i = config_.max_files; i-- > 0;) {
      std::filesystem::path current =
          config_.log_file + (i > 0 ? "." + std::to_string(i) : "");
      std::filesystem::path next =
          config_.log_file + "." + std::to_string(i + 1);

      if (std::filesystem::exists(current)) {
        if (i == config_.max_files - 1) {
          std::filesystem::remove(current);
        } else {
          std::filesystem::rename(current, next);
        }
      }
    }

    file_output_.open(config_.log_file);
  }

  constexpr std::pair<LogColor, std::string_view> getLevelInfo(
      LogLevel level) const {
    switch (level) {
      using enum LogLevel;
      case Debug:
        return {LogColor::Blue, "DEBUG"};
      case Info:
        return {LogColor::Green, "INFO"};
      case Warning:
        return {LogColor::Yellow, "WARN"};
      case Error:
        return {LogColor::Red, "ERROR"};
      case Critical:
        return {LogColor::Magenta, "CRIT"};
    }
    return {LogColor::White, "UNKNOWN"};
  }

  constexpr std::string formatLogMessage(const LogEntry& entry,
                                         std::string_view level_str) const {
    auto result = std::string(config_.message_format);

    const std::array<std::pair<std::string_view, std::string>, 7> replacements =
        {{{"{timestamp}", std::format("{:%Y-%m-%d %H:%M:%S}", entry.timestamp)},
          {"{level}", std::string(level_str)},
          {"{message}", entry.message},
          {"{file}", std::string(entry.file)},
          {"{line}", std::to_string(entry.line)},
          {"{function}", std::string(entry.function)},
          {"{thread}", std::format("{}", entry.thread_id)}}};

    for (const auto& [pattern, value] : replacements) {
      size_t pos = 0;
      while ((pos = result.find(pattern, pos)) != std::string::npos) {
        result.replace(pos, pattern.length(), value);
        pos += value.length();
      }
    }

    return result;
  }

  void startWorker() {
    if (!config_.async_processing) return;

    worker_thread_ = std::jthread([this](std::stop_token stop_token) {
      while (!stop_token.stop_requested()) {
        std::this_thread::sleep_for(config_.flush_interval);
        flush();
      }
    });
  }

  Config config_;
  std::ofstream file_output_;
  std::mutex mutex_;
  std::queue<LogEntry> log_queue_;
  std::vector<LogEntry> logs_;
  std::jthread worker_thread_;
  std::atomic<bool> processing_enabled_{true};
};

int main() {
  auto& logger = Logger::getInstance();

  Logger::Config config{
      .min_level = LogLevel::Debug,
      .log_file = "application.log",
      .max_file_size = 1024 * 1024,  // 1MB
      .max_files = 3,
      .console_output = true,
      .message_format = "{timestamp} [{level}] {message} ({file}:{line})",
      .async_processing = true,
      .flush_interval = std::chrono::milliseconds{1000}};
  if (auto result = logger.configure(config); !result) {
    std::println("Failed to configure logger: {}", result.error().message());
    return 1;
  }

  logger.info("Application started");
  logger.debug("Debug value: {}", 42);

  struct User {
    std::string name;
    int age;
  };
  User user{"Alice", 30};
  logger.info("User logged in: {} (age: {})", user.name, user.age);

  logger.error("Something went wrong!");

  return 0;
}
