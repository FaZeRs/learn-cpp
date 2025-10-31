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
#include <utility>

enum class LogColor : uint8_t {
  Reset = 0,
  Red = 31,
  Green = 32,
  Yellow = 33,
  Blue = 34,
  Magenta = 35,
  White = 37
};

enum class LogLevel : uint8_t { Debug, Info, Warning, Error, Critical };

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

constexpr std::pair<LogColor, std::string_view> getLevelInfo(LogLevel level) {
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

std::string formatLogMessage(const LogEntry& entry, std::string_view level_str,
                             std::string message_format) {
  auto result = std::move(message_format);
  const std::array<std::pair<std::string_view, std::string>, 7> replacements = {
      {{"{timestamp}", std::format("{:%Y-%m-%d %H:%M:%S}", entry.timestamp)},
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

class LogSink {
 public:
  LogSink() = default;
  virtual ~LogSink() = default;
  LogSink(const LogSink&) = delete;
  LogSink& operator=(const LogSink&) = delete;
  LogSink(LogSink&&) = delete;
  LogSink& operator=(LogSink&&) = delete;
  virtual void write(const LogEntry& entry) = 0;
  virtual void flush() = 0;
  virtual void setMessageFormat(std::string_view message_format) = 0;
};

class ConsoleSink : public LogSink {
 public:
  void write(const LogEntry& entry) override {
    auto [color, level_str] = getLevelInfo(entry.level);
    std::println("\033[{}m{}\033[0m", static_cast<int>(color),
                 formatLogMessage(entry, level_str, message_format_));
  }

  void flush() override {}  // Console output is immediately flushed

  void setMessageFormat(std::string_view message_format) override {
    message_format_ = message_format;
  }

 private:
  std::string message_format_ = "{timestamp} [{level}] {message}";
};

class FileSink : public LogSink {
 public:
  explicit FileSink(const std::string& filename) {
    file_output_.open(filename, std::ios::app);
    if (!file_output_) {
      throw std::runtime_error("Failed to open log file");
    }
  }

  void write(const LogEntry& entry) override {
    auto [_, level_str] = getLevelInfo(entry.level);
    file_output_ << formatLogMessage(entry, level_str, message_format_) << '\n';
  }

  void flush() override { file_output_.flush(); }

  void setMessageFormat(std::string_view message_format) override {
    message_format_ = message_format;
  }

 private:
  std::ofstream file_output_;
  std::string message_format_ = "{timestamp} [{level}] {message}";
};

class RotatingFileSink : public LogSink {
 public:
  RotatingFileSink(const std::string& filename, size_t max_size,
                   size_t max_files)
      : filename_(filename), max_size_(max_size), max_files_(max_files) {
    file_output_.open(filename, std::ios::app);
    if (!file_output_) {
      throw std::runtime_error("Failed to open log file");
    }
  }

  void write(const LogEntry& entry) override {
    if (shouldRotate()) {
      rotate();
    }
    auto [_, level_str] = getLevelInfo(entry.level);
    file_output_ << formatLogMessage(entry, level_str, message_format_) << '\n';
  }

  void flush() override { file_output_.flush(); }

  void setMessageFormat(std::string_view message_format) override {
    message_format_ = message_format;
  }

 private:
  bool shouldRotate() {
    file_output_.seekp(0, std::ios::end);
    return std::cmp_greater_equal(
        static_cast<std::streamoff>(file_output_.tellp()), max_size_);
  }

  void rotate() {
    file_output_.close();

    for (size_t i = max_files_; i-- > 0;) {
      std::filesystem::path current =
          filename_ + (i > 0 ? "." + std::to_string(i) : "");
      std::filesystem::path next = filename_ + "." + std::to_string(i + 1);

      if (std::filesystem::exists(current)) {
        if (i == max_files_ - 1) {
          std::filesystem::remove(current);
        } else {
          std::filesystem::rename(current, next);
        }
      }
    }

    file_output_.open(filename_);
  }

  std::string filename_;
  size_t max_size_;
  size_t max_files_;
  std::ofstream file_output_;
  std::string message_format_ = "{timestamp} [{level}] {message}";
};

class Logger {
 public:
  struct Config {
    LogLevel min_level = LogLevel::Debug;
    size_t buffer_size = 1024;
    bool async_processing = true;
    std::chrono::milliseconds flush_interval{1000};
  };

  static Logger& getInstance() {
    static Logger instance;
    return instance;
  }

  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger(Logger&&) = delete;
  Logger& operator=(Logger&&) = delete;

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
    if (config_.async_processing) {
      startWorker();
    }
    return {};
  }

  void addSink(std::shared_ptr<LogSink> sink) {
    std::scoped_lock lock(mutex_);
    sinks_.push_back(std::move(sink));
  }

  template <typename... Args>
    requires ValidFormatString<Args...>
  constexpr void log(LogLevel level, LogMessage<Args...>&& message) {
    if (level < config_.min_level) return;

    auto msg = std::move(message);
    const auto loc = msg.location;
    const auto formatted = std::apply(
        [&](auto&&... args) {
          return std::format(msg.fmt, std::forward<Args>(args)...);
        },
        std::move(msg.args));

    LogEntry entry{.timestamp = std::chrono::system_clock::now(),
                   .level = level,
                   .message = std::move(formatted),
                   .file = loc.file_name(),
                   .line = loc.line(),
                   .function = loc.function_name(),
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

    for (const auto& entry : batch) {
      for (auto& sink : sinks_) {
        sink->write(entry);
      }
    }

    logs_.insert(logs_.end(), std::make_move_iterator(batch.begin()),
                 std::make_move_iterator(batch.end()));
  }

  void startWorker() {
    if (!config_.async_processing) return;

    worker_thread_ = std::jthread([this](const std::stop_token& stop_token) {
      while (!stop_token.stop_requested()) {
        std::this_thread::sleep_for(config_.flush_interval);
        flush();
      }
    });
  }

  Config config_;
  std::mutex mutex_;
  std::queue<LogEntry> log_queue_;
  std::vector<LogEntry> logs_;
  std::jthread worker_thread_;
  std::atomic<bool> processing_enabled_{true};
  std::vector<std::shared_ptr<LogSink>> sinks_;
};

int main() {
  auto& logger = Logger::getInstance();

  Logger::Config config{.min_level = LogLevel::Debug,
                        .async_processing = true,
                        .flush_interval = std::chrono::milliseconds{1000}};
  if (auto result = logger.configure(config); !result) {
    std::println("Failed to configure logger: {}", result.error().message());
    return 1;
  }

  const auto console_sink = std::make_shared<ConsoleSink>();
  console_sink->setMessageFormat(
      "{timestamp} [{level}] {message} ({file}:{line})");
  logger.addSink(console_sink);
  logger.addSink(std::make_shared<FileSink>("basic.log"));
  logger.addSink(std::make_shared<RotatingFileSink>("rotating.log",
                                                    1024 * 1024,  // 1MB
                                                    3  // Keep 3 files
                                                    ));

  logger.info("Application started");
  logger.debug("Debug value: {}", 42);

  struct User {
    std::string name;
    int age;
  };
  User user{.name = "Alice", .age = 30};
  logger.info("User logged in: {} (age: {})", user.name, user.age);

  logger.error("Something went wrong!");

  return 0;
}
