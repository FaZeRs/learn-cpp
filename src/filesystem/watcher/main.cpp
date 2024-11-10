#include <chrono>
#include <expected>
#include <filesystem>
#include <format>
#include <functional>
#include <iostream>
#include <print>
#include <ranges>
#include <stop_token>
#include <string_view>
#include <system_error>
#include <thread>
#include <unordered_map>

enum class FileWatcherError { PathNotFound, WatcherStartFailed, ScanFailed };

class FileWatcherCategory : public std::error_category {
 public:
  [[nodiscard]] const char* name() const noexcept override {
    return "FileWatcher";
  }

  [[nodiscard]] std::string message(int ev) const override {
    switch (static_cast<FileWatcherError>(ev)) {
      case FileWatcherError::PathNotFound:
        return "Watched path not found";
      case FileWatcherError::WatcherStartFailed:
        return "Failed to start file watcher";
      case FileWatcherError::ScanFailed:
        return "Failed to scan directory";
      default:
        return "Unknown error";
    }
  }
};

const FileWatcherCategory& file_watcher_category() {
  static FileWatcherCategory category;
  return category;
}

namespace std {
template <>
struct is_error_code_enum<FileWatcherError> : true_type {};
}  // namespace std

[[nodiscard]] std::error_code make_error_code(FileWatcherError e) {
  return {static_cast<int>(e), file_watcher_category()};
}

using namespace std::chrono_literals;
using namespace std::literals;

class FileWatcher {
 public:
  using WatcherCallback =
      std::function<void(const std::filesystem::path&, std::string_view,
                         std::filesystem::file_type, std::uintmax_t)>;

  struct Config {
    std::filesystem::path path_to_watch;
    WatcherCallback callback;
    std::chrono::duration<int> polling_interval = 1s;
  };

  explicit FileWatcher(Config config)
      : path_to_watch_(std::move(config.path_to_watch)),
        callback_(std::move(config.callback)),
        polling_interval_(config.polling_interval) {
    if (!std::filesystem::exists(path_to_watch_)) {
      throw std::system_error(make_error_code(FileWatcherError::PathNotFound));
    }
  }
  FileWatcher(const FileWatcher&) = delete;
  FileWatcher& operator=(const FileWatcher&) = delete;
  FileWatcher(FileWatcher&&) noexcept = default;
  FileWatcher& operator=(FileWatcher&&) noexcept = default;

  [[nodiscard]] std::expected<std::stop_source, std::error_code> start() {
    if (watcher_thread_.joinable()) {
      return std::unexpected(
          make_error_code(FileWatcherError::WatcherStartFailed));
    }

    std::stop_source source;
    try {
      watcher_thread_ = std::jthread(
          [this, stop_token = source.get_token()](std::stop_token token) {
            while (!token.stop_requested()) {
              if (auto scan_result = scan(); scan_result) {
                process_changes(*scan_result);
                paths_ = std::move(*scan_result);
              }
              std::this_thread::sleep_for(polling_interval_);
            }
          });
    } catch (const std::exception& e) {
      return std::unexpected(
          make_error_code(FileWatcherError::WatcherStartFailed));
    }
    return source;
  }

  ~FileWatcher() = default;

 private:
  struct FileInfo {
    std::chrono::file_clock::time_point timestamp;
    std::filesystem::file_type type;
    std::uintmax_t size;
    bool operator==(const FileInfo& other) const noexcept {
      return timestamp == other.timestamp && type == other.type &&
             size == other.size;
    }
  };

  void process_changes(const std::unordered_map<std::filesystem::path,
                                                FileInfo>& current_files) {
    // Check for modified and created files
    for (const auto& [path, info] : current_files) {
      if (auto it = paths_.find(path); it != paths_.end()) {
        if (it->second != info) {
          callback_(path, "modified"sv, info.type, info.size);
        }
      } else {
        callback_(path, "created"sv, info.type, info.size);
      }
    }

    // Check for deleted files
    for (const auto& [path, info] : paths_) {
      if (!current_files.contains(path)) {
        callback_(path, "deleted"sv, info.type, info.size);
      }
    }
  }

  [[nodiscard]] std::expected<
      std::unordered_map<std::filesystem::path, FileInfo>, std::error_code>
  scan() const {
    try {
      std::unordered_map<std::filesystem::path, FileInfo> paths;

      auto entries =
          std::filesystem::recursive_directory_iterator(
              path_to_watch_,
              std::filesystem::directory_options::skip_permission_denied) |
          std::views::filter(
              [](const auto& e) { return !std::filesystem::is_symlink(e); }) |
          std::views::transform([](const auto& entry) {
            const auto& path = entry.path();
            const auto size = std::filesystem::is_regular_file(entry)
                                  ? std::filesystem::file_size(path)
                                  : 0;
            return std::pair{
                path,
                FileInfo{.timestamp = std::filesystem::last_write_time(path),
                         .type = entry.status().type(),
                         .size = size}};
          });

      paths.insert(std::ranges::begin(entries), std::ranges::end(entries));
      return paths;
    } catch (const std::exception& e) {
      return std::unexpected(make_error_code(FileWatcherError::ScanFailed));
    }
  }

  std::filesystem::path path_to_watch_;
  WatcherCallback callback_;
  std::chrono::duration<int> polling_interval_;
  std::unordered_map<std::filesystem::path, FileInfo> paths_;
  std::jthread watcher_thread_;
};

[[nodiscard]] constexpr std::string_view file_type_to_string(
    std::filesystem::file_type type) {
  switch (type) {
    case std::filesystem::file_type::regular:
      return "file"sv;
    case std::filesystem::file_type::directory:
      return "directory"sv;
    case std::filesystem::file_type::symlink:
      return "symlink"sv;
    case std::filesystem::file_type::block:
      return "block"sv;
    case std::filesystem::file_type::character:
      return "character"sv;
    case std::filesystem::file_type::fifo:
      return "fifo"sv;
    case std::filesystem::file_type::socket:
      return "socket"sv;
    case std::filesystem::file_type::none:
      return "none"sv;
    default:
      return "unknown"sv;
  }
}

int main() {
  std::println("Started watching directory. Press Enter to exit...");

  auto watcher = FileWatcher{
      {.path_to_watch = ".",
       .callback = [](const std::filesystem::path& path,
                      std::string_view action, std::filesystem::file_type type,
                      std::uintmax_t size) {
         const auto now = std::chrono::system_clock::now();
         const auto time = std::chrono::current_zone()->to_local(now);

         std::println("{:%H:%M:%S} {} ({}, {} bytes) was {}", time,
                      path.string(), file_type_to_string(type), size, action);
       }}};

  if (auto result = watcher.start(); result) {
    auto stop_source = std::move(*result);
    std::cin.get();
    stop_source.request_stop();
  } else {
    std::println("Error starting watcher: {}", result.error().message());
    return 1;
  }

  return 0;
}
