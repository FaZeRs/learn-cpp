#include <array>
#include <chrono>
#include <concepts>
#include <coroutine>
#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <print>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// Add string_view literal
using namespace std::string_view_literals;

/**
 * @brief Enum representing file operation errors
 */
enum class FileError {
  NotFound,
  AccessDenied,
  AlreadyExists,
  IoError,
  InvalidPath,
  Unknown
};

/**
 * @brief Class representing a file in the file system
 */
class File {
 public:
  /**
   * @brief Construct a new File object
   * @param path Path to the file
   */
  explicit File(std::filesystem::path path) : file_path_(std::move(path)) {}

  /**
   * @brief Get the name of the file
   * @return std::string Name of the file
   */
  [[nodiscard]] std::string getName() const {
    return file_path_.filename().string();
  }

  /**
   * @brief Get the size of the file
   * @return std::uintmax_t Size of the file in bytes
   */
  [[nodiscard]] std::uintmax_t getSize() const {
    return std::filesystem::exists(file_path_)
               ? std::filesystem::file_size(file_path_)
               : 0;
  }

  /**
   * @brief Get the path of the file
   * @return std::filesystem::path Path of the file
   */
  [[nodiscard]] const std::filesystem::path& getPath() const {
    return file_path_;
  }

  /**
   * @brief Check if the file exists
   * @return bool True if the file exists, false otherwise
   */
  [[nodiscard]] bool exists() const {
    return std::filesystem::exists(file_path_);
  }

  /**
   * @brief Get last modification time of the file
   * @return std::filesystem::file_time_type Last modification time
   */
  [[nodiscard]] std::filesystem::file_time_type getLastModifiedTime() const {
    if (!exists()) {
      return std::filesystem::file_time_type{};
    }
    return std::filesystem::last_write_time(file_path_);
  }

  /**
   * @brief Format the last modification time
   * @return std::string Formatted time string
   */
  [[nodiscard]] std::string getFormattedModTime() const {
    if (!exists()) {
      return "N/A";
    }

    const auto file_time = std::filesystem::last_write_time(file_path_);
    const auto sys_time = std::chrono::file_clock::to_sys(file_time);
    return std::format("{:%Y-%m-%d %H:%M:%S}", sys_time);
  }

  /**
   * @brief Load the content of the file
   * @return std::expected<void, FileError> Success or error
   */
  std::expected<void, FileError> loadContent() {
    if (!exists()) {
      return std::unexpected(FileError::NotFound);
    }

    try {
      std::ifstream file_stream(file_path_, std::ios::in | std::ios::binary);
      if (!file_stream) {
        return std::unexpected(FileError::AccessDenied);
      }

      file_stream.seekg(0, std::ios::end);
      file_content_.resize(
          static_cast<std::string::size_type>(file_stream.tellg()));
      file_stream.seekg(0, std::ios::beg);
      file_stream.read(file_content_.data(),
                       static_cast<std::streamsize>(file_content_.size()));

      is_content_loaded_ = true;
      return {};
    } catch (const std::exception& e) {
      std::println("Error loading file content: {}", e.what());
      return std::unexpected(FileError::IoError);
    }
  }

  /**
   * @brief Save content to the file
   * @param content Content to save
   * @return std::expected<void, FileError> Success or error
   */
  std::expected<void, FileError> saveContent(std::string_view content) {
    try {
      std::ofstream file_stream(file_path_, std::ios::out | std::ios::binary);
      if (!file_stream) {
        return std::unexpected(FileError::AccessDenied);
      }

      file_stream.write(content.data(),
                        static_cast<std::streamsize>(content.size()));
      file_content_ = content;
      is_content_loaded_ = true;
      return {};
    } catch (const std::exception& e) {
      std::println("Error saving file content: {}", e.what());
      return std::unexpected(FileError::IoError);
    }
  }

  /**
   * @brief Get the content of the file
   * @return std::expected<std::string_view, FileError> Content of the file or
   * error
   */
  [[nodiscard]] std::expected<std::string_view, FileError> getContent() const {
    if (is_content_loaded_) {
      return file_content_;
    }
    return std::unexpected(FileError::NotFound);
  }

  /**
   * @brief Asynchronous file loader
   */
  struct AsyncFileLoader {
    struct promise_type {
      std::expected<std::string, FileError> value_;

      AsyncFileLoader get_return_object() {
        return AsyncFileLoader{
            std::coroutine_handle<promise_type>::from_promise(*this)};
      }
      std::suspend_never initial_suspend() noexcept { return {}; }
      std::suspend_always final_suspend() noexcept { return {}; }
      void return_value(std::expected<std::string, FileError> value) {
        value_ = std::move(value);
      }
      void unhandled_exception() {
        value_ = std::unexpected(FileError::Unknown);
      }
    };

    std::coroutine_handle<promise_type> handle_;

    explicit AsyncFileLoader(std::coroutine_handle<promise_type> h)
        : handle_(h) {}
    ~AsyncFileLoader() {
      if (handle_) handle_.destroy();
    }

    [[nodiscard]] std::expected<std::string, FileError> result() const {
      return handle_.promise().value_;
    }
  };

  /**
   * @brief Load content asynchronously
   * @return AsyncFileLoader
   */
  [[nodiscard]] AsyncFileLoader loadContentAsync() {
    if (!exists()) {
      co_return std::unexpected(FileError::NotFound);
    }

    try {
      std::ifstream file_stream(file_path_, std::ios::in | std::ios::binary);
      if (!file_stream) {
        co_return std::unexpected(FileError::AccessDenied);
      }

      file_stream.seekg(0, std::ios::end);
      std::string content;
      content.resize(static_cast<std::string::size_type>(file_stream.tellg()));
      file_stream.seekg(0, std::ios::beg);
      file_stream.read(content.data(),
                       static_cast<std::streamsize>(content.size()));

      file_content_ = content;
      is_content_loaded_ = true;
      co_return content;
    } catch (const std::exception& e) {
      std::println("Error loading file content: {}", e.what());
      co_return std::unexpected(FileError::IoError);
    }
  }

  /**
   * @brief Three-way comparison operator for files
   * @param other Other file to compare with
   * @return std::strong_ordering Ordering result
   */
  [[nodiscard]] auto operator<=>(const File& other) const = default;

  /**
   * @brief Equality comparison operator for files
   * @param other Other file to compare with
   * @return bool True if files are equal
   */
  [[nodiscard]] bool operator==(const File& other) const = default;

 private:
  std::filesystem::path file_path_;
  mutable std::string file_content_;
  mutable bool is_content_loaded_ = false;
};

/**
 * @brief Class representing a directory in the file system
 */
class Directory {
 public:
  /**
   * @brief Construct a new Directory object
   * @param path Path to the directory
   */
  explicit Directory(std::filesystem::path path) : dir_path_(std::move(path)) {}

  /**
   * @brief Get the name of the directory
   * @return std::string Name of the directory
   */
  [[nodiscard]] std::string getName() const {
    return dir_path_.filename().string();
  }

  /**
   * @brief Get the path of the directory
   * @return std::filesystem::path Path of the directory
   */
  [[nodiscard]] const std::filesystem::path& getPath() const {
    return dir_path_;
  }

  /**
   * @brief Check if the directory exists
   * @return bool True if the directory exists, false otherwise
   */
  [[nodiscard]] bool exists() const {
    return std::filesystem::exists(dir_path_) &&
           std::filesystem::is_directory(dir_path_);
  }

  /**
   * @brief Create the directory if it doesn't exist
   * @return std::expected<void, FileError> Success or error
   */
  std::expected<void, FileError> create() const {
    if (exists()) {
      return {};
    }

    try {
      if (std::filesystem::create_directories(dir_path_)) {
        return {};
      }
      return std::unexpected(FileError::IoError);
    } catch (const std::exception& e) {
      std::println("Error creating directory: {}", e.what());
      return std::unexpected(FileError::IoError);
    }
  }

  /**
   * @brief List all files in the directory
   * @return std::vector<File> Vector of files in the directory
   */
  [[nodiscard]] std::vector<File> listFiles() const {
    if (!exists()) {
      return {};
    }

    try {
      return std::filesystem::directory_iterator{dir_path_} |
             std::views::filter(
                 [](const auto& entry) { return entry.is_regular_file(); }) |
             std::views::transform(
                 [](const auto& entry) { return File{entry.path()}; }) |
             std::ranges::to<std::vector>();
    } catch (const std::exception& e) {
      std::println("Error listing files: {}", e.what());
      return {};
    }
  }

  /**
   * @brief List all subdirectories in the directory
   * @return std::vector<Directory> Vector of subdirectories in the directory
   */
  [[nodiscard]] std::vector<Directory> listSubdirectories() const {
    if (!exists()) {
      return {};
    }

    try {
      return std::filesystem::directory_iterator{dir_path_} |
             std::views::filter(
                 [](const auto& entry) { return entry.is_directory(); }) |
             std::views::transform(
                 [](const auto& entry) { return Directory{entry.path()}; }) |
             std::ranges::to<std::vector>();
    } catch (const std::exception& e) {
      std::println("Error listing subdirectories: {}", e.what());
      return {};
    }
  }

  /**
   * @brief Three-way comparison operator for directories
   * @param other Other directory to compare with
   * @return std::strong_ordering Ordering result
   */
  [[nodiscard]] auto operator<=>(const Directory& other) const = default;

  /**
   * @brief Equality comparison operator for directories
   * @param other Other directory to compare with
   * @return bool True if directories are equal
   */
  [[nodiscard]] bool operator==(const Directory& other) const = default;

 private:
  std::filesystem::path dir_path_;
};

/**
 * @brief Concept for file-like objects
 */
template <typename T>
concept FilePathProvider = requires(T a) {
  { a.getPath() } -> std::convertible_to<std::filesystem::path>;
  { a.exists() } -> std::convertible_to<bool>;
};

/**
 * @brief Concept for processors of file objects
 */
template <typename T>
concept FileProcessor = std::invocable<T, const File&>;

/**
 * @brief Class for managing files and directories
 */
class FileManager {
 public:
  /**
   * @brief Construct a new File Manager object
   * @param initial_path Initial path to start from
   */
  explicit FileManager(const std::filesystem::path& initial_path =
                           std::filesystem::current_path())
      : current_directory_(initial_path) {
    if (!current_directory_.exists()) {
      std::println("Warning: Initial directory does not exist. Creating it.");
      current_directory_.create();
    }
  }

  /**
   * @brief Get the current directory
   * @return const Directory& Current directory
   */
  [[nodiscard]] const Directory& getCurrentDirectory() const {
    return current_directory_;
  }

  /**
   * @brief Change the current directory
   * @param path Path to the new directory
   * @return std::expected<void, FileError> Success or error
   */
  std::expected<void, FileError> changeDirectory(
      const std::filesystem::path& path) {
    Directory new_dir(path);
    if (!new_dir.exists()) {
      return std::unexpected(FileError::NotFound);
    }
    current_directory_ = new_dir;
    return {};
  }

  /**
   * @brief Create a new file
   * @param file_name Name of the file
   * @param content Initial content of the file
   * @return std::expected<File, FileError> The created file or error
   */
  std::expected<File, FileError> createFile(std::string_view file_name,
                                            std::string_view content = "") {
    auto file_path = current_directory_.getPath() / file_name;
    File file(file_path);

    if (file.exists()) {
      std::println("File already exists: {}", file_name);
      return std::unexpected(FileError::AlreadyExists);
    }

    auto result = file.saveContent(content);
    if (!result) {
      return std::unexpected(result.error());
    }

    return file;
  }

  /**
   * @brief Read a file
   * @param file_name Name of the file
   * @return std::expected<File, FileError> The read file or error
   */
  [[nodiscard]] std::expected<File, FileError> readFile(
      std::string_view file_name) const {
    auto file_path = current_directory_.getPath() / file_name;
    File file(file_path);

    if (!file.exists()) {
      std::println("File does not exist: {}", file_name);
      return std::unexpected(FileError::NotFound);
    }

    auto result = file.loadContent();
    if (!result) {
      return std::unexpected(result.error());
    }

    return file;
  }

  /**
   * @brief Update a file
   * @param file_name Name of the file
   * @param content New content of the file
   * @return std::expected<void, FileError> Success or error
   */
  std::expected<void, FileError> updateFile(std::string_view file_name,
                                            std::string_view content) const {
    auto file_path = current_directory_.getPath() / file_name;
    File file(file_path);

    if (!file.exists()) {
      std::println("File does not exist: {}", file_name);
      return std::unexpected(FileError::NotFound);
    }

    return file.saveContent(content);
  }

  /**
   * @brief Delete a file
   * @param file_name Name of the file
   * @return std::expected<void, FileError> Success or error
   */
  std::expected<void, FileError> deleteFile(std::string_view file_name) const {
    auto file_path = current_directory_.getPath() / file_name;

    try {
      if (std::filesystem::remove(file_path)) {
        return {};
      }
      return std::unexpected(FileError::NotFound);
    } catch (const std::exception& e) {
      std::println("Error deleting file: {}", e.what());
      return std::unexpected(FileError::IoError);
    }
  }

  /**
   * @brief Create a new directory
   * @param dir_name Name of the directory
   * @return std::expected<Directory, FileError> The created directory or error
   */
  std::expected<Directory, FileError> createDirectory(
      std::string_view dir_name) {
    auto dir_path = current_directory_.getPath() / dir_name;
    Directory dir(dir_path);

    if (dir.exists()) {
      std::println("Directory already exists: {}", dir_name);
      return std::unexpected(FileError::AlreadyExists);
    }

    auto result = dir.create();
    if (!result) {
      return std::unexpected(result.error());
    }

    return dir;
  }

  /**
   * @brief Find files by pattern using ranges
   * @param pattern Pattern to search for
   * @return std::vector<File> Vector of files matching the pattern
   */
  [[nodiscard]] std::vector<File> findFiles(std::string_view pattern) const {
    auto files = current_directory_.listFiles();

    return files | std::views::filter([pattern](const File& file) {
             return file.getName().contains(pattern);
           }) |
           std::ranges::to<std::vector>();
  }

  /**
   * @brief Process files with a specific extension
   * @param extension File extension to filter by
   * @param processor Function to process each file
   */
  template <typename Func>
    requires std::invocable<Func, const File&>
  void processFilesByExtension(std::string_view extension,
                               Func processor) const {
    auto files = current_directory_.listFiles();

    for (const auto& file :
         files | std::views::filter([extension](const File& f) {
           return f.getPath().extension() == extension;
         })) {
      processor(file);
    }
  }

  /**
   * @brief Copy a file
   * @param source_file_name Source file name
   * @param target_file_name Target file name
   * @return std::expected<void, FileError> Success or error
   */
  std::expected<void, FileError> copyFile(
      std::string_view source_file_name,
      std::string_view target_file_name) const {
    auto source_path = current_directory_.getPath() / source_file_name;
    auto target_path = current_directory_.getPath() / target_file_name;

    try {
      std::filesystem::copy_file(
          source_path, target_path,
          std::filesystem::copy_options::overwrite_existing);
      return {};
    } catch (const std::filesystem::filesystem_error& e) {
      if (e.code() == std::errc::no_such_file_or_directory) {
        return std::unexpected(FileError::NotFound);
      }
      std::println("Error copying file: {}", e.what());
      return std::unexpected(FileError::IoError);
    } catch (const std::exception& e) {
      std::println("Error copying file: {}", e.what());
      return std::unexpected(FileError::Unknown);
    }
  }

  /**
   * @brief Move a file
   * @param source_file_name Source file name
   * @param target_file_name Target file name
   * @return std::expected<void, FileError> Success or error
   */
  std::expected<void, FileError> moveFile(
      std::string_view source_file_name,
      std::string_view target_file_name) const {
    auto source_path = current_directory_.getPath() / source_file_name;
    auto target_path = current_directory_.getPath() / target_file_name;

    try {
      std::filesystem::rename(source_path, target_path);
      return {};
    } catch (const std::filesystem::filesystem_error& e) {
      if (e.code() == std::errc::no_such_file_or_directory) {
        return std::unexpected(FileError::NotFound);
      }
      std::println("Error moving file: {}", e.what());
      return std::unexpected(FileError::IoError);
    } catch (const std::exception& e) {
      std::println("Error moving file: {}", e.what());
      return std::unexpected(FileError::Unknown);
    }
  }

  /**
   * @brief Read and process multiple files in batch
   * @param file_names List of file names to read
   * @param processor Function to process each file
   * @return std::vector<std::pair<std::string, FileError>> Vector of file names
   * and errors
   */
  template <FileProcessor Func>
  [[nodiscard]] std::vector<std::pair<std::string, FileError>> processBatch(
      std::span<const std::string_view> file_names, Func processor) const {
    std::vector<std::pair<std::string, FileError>> errors;

    for (const auto& file_name : file_names) {
      auto file_result = readFile(file_name);
      if (file_result) {
        processor(*file_result);
      } else {
        errors.emplace_back(std::string{file_name}, file_result.error());
      }
    }

    return errors;
  }

  /**
   * @brief Save a batch of files
   * @param file_contents Map of file names to contents
   * @return std::vector<std::pair<std::string, FileError>> Vector of file names
   * and errors
   */
  [[nodiscard]] std::vector<std::pair<std::string, FileError>> saveBatch(
      const std::unordered_map<std::string_view, std::string_view>&
          file_contents) const {
    std::vector<std::pair<std::string, FileError>> errors;

    for (const auto& [file_name, content] : file_contents) {
      auto update_result = updateFile(file_name, content);
      if (!update_result) {
        errors.emplace_back(std::string{file_name}, update_result.error());
      }
    }

    return errors;
  }

 private:
  Directory current_directory_;
};

/**
 * @brief Get a string representation of a FileError
 * @param error The error to convert
 * @return std::string_view String representation of the error
 */
[[nodiscard]] constexpr std::string_view fileErrorToString(FileError error) {
  switch (error) {
    case FileError::NotFound:
      return "File not found";
    case FileError::AccessDenied:
      return "Access denied";
    case FileError::AlreadyExists:
      return "File already exists";
    case FileError::IoError:
      return "I/O error";
    case FileError::InvalidPath:
      return "Invalid path";
    case FileError::Unknown:
      return "Unknown error";
    default:
      return "Unrecognized error";
  }
}

/**
 * @brief Print file information in a formatted way
 * @param file The file to print information about
 */
void printFileInfo(const File& file) {
  std::println("{:<20} | {:<10} | {}", file.getName(), file.getSize(),
               file.getFormattedModTime());
}

/**
 * @brief Demonstration of the FileManager capabilities
 */
void demo() {
  try {
    // Create temporary directory for the demonstration
    auto temp_path =
        std::filesystem::temp_directory_path() / "file_manager_demo";
    std::filesystem::create_directories(temp_path);

    std::println("Demonstration directory: {}", temp_path.string());
    std::println("");

    // Initialize file manager with the temporary directory
    FileManager manager(temp_path);

    // Create a text file
    std::println("Creating a text file...");
    if (auto file_result = manager.createFile(
            "example.txt"sv, "Hello, this is an example file content!"sv);
        file_result) {
      const auto& text_file = *file_result;
      std::println("Created file: {} ({} bytes)", text_file.getName(),
                   text_file.getSize());
    } else {
      std::println("Failed to create file: {}",
                   fileErrorToString(file_result.error()));
    }

    // Create a directory
    std::println("\nCreating a directory...");
    if (auto dir_result = manager.createDirectory("subdir"sv); dir_result) {
      const auto& sub_dir = *dir_result;
      std::println("Created directory: {}", sub_dir.getName());
    } else {
      std::println("Failed to create directory: {}",
                   fileErrorToString(dir_result.error()));
    }

    // List all files in the current directory
    std::println("\nListing files in the current directory:");
    std::println("{:<20} | {:<10} | {}", "Name", "Size", "Modified");
    std::println("{:-<20}-+-{:-<10}-+-{:-<20}", "", "", "");

    for (const auto& file : manager.getCurrentDirectory().listFiles()) {
      printFileInfo(file);
    }

    // Read the file content
    std::println("\nReading file content:");
    if (auto file_result = manager.readFile("example.txt"sv); file_result) {
      if (auto content = file_result->getContent(); content) {
        std::println("Content: {}", *content);
      } else {
        std::println("Content not loaded: {}",
                     fileErrorToString(content.error()));
      }
    } else {
      std::println("Failed to read file: {}",
                   fileErrorToString(file_result.error()));
    }

    // Update the file content
    std::println("\nUpdating file content...");
    if (auto result = manager.updateFile("example.txt"sv,
                                         "This is the updated content!"sv);
        result) {
      std::println("File updated successfully.");

      // Read the updated content
      if (auto updated_result = manager.readFile("example.txt"sv);
          updated_result) {
        if (auto content = updated_result->getContent(); content) {
          std::println("New content: {}", *content);
        }
      }
    } else {
      std::println("Failed to update file: {}",
                   fileErrorToString(result.error()));
    }

    // Create multiple files for batch operations
    std::println("\nCreating multiple files for batch operations...");
    manager.createFile("batch1.txt"sv, "Batch file 1 content"sv);
    manager.createFile("batch2.txt"sv, "Batch file 2 content"sv);
    manager.createFile("batch3.txt"sv, "Batch file 3 content"sv);

    // Batch process files
    std::println("\nBatch processing files:");
    std::array file_names = {"batch1.txt"sv, "batch2.txt"sv, "batch3.txt"sv,
                             "nonexistent.txt"sv};

    auto batch_errors = manager.processBatch(file_names, [](const File& file) {
      auto content = file.getContent();
      if (content) {
        std::println("Processing '{}': content length = {}", file.getName(),
                     content->length());
      }
    });

    if (!batch_errors.empty()) {
      std::println("\nBatch processing errors:");
      for (const auto& [file_name, error] : batch_errors) {
        std::println("- {}: {}", file_name, fileErrorToString(error));
      }
    }

    // Batch update files
    std::println("\nBatch updating files:");
    std::unordered_map<std::string_view, std::string_view> updates{
        {"batch1.txt"sv, "Updated batch file 1 content"sv},
        {"batch2.txt"sv, "Updated batch file 2 content"sv},
        {"nonexistent.txt"sv, "This file doesn't exist"sv}};

    auto update_errors = manager.saveBatch(updates);

    if (!update_errors.empty()) {
      std::println("Batch update errors:");
      for (const auto& [file_name, error] : update_errors) {
        std::println("- {}: {}", file_name, fileErrorToString(error));
      }
    }

    // Create another file for copy demonstration
    manager.createFile("source.txt"sv, "This file will be copied."sv);

    // Copy a file
    std::println("\nCopying a file...");
    if (auto result = manager.copyFile("source.txt"sv, "destination.txt"sv);
        result) {
      std::println("File copied successfully.");

      // List files again to see the copied file
      std::println("\nListing files after copy operation:");
      std::println("{:<20} | {:<10} | {}", "Name", "Size", "Modified");
      std::println("{:-<20}-+-{:-<10}-+-{:-<20}", "", "", "");

      for (const auto& file : manager.getCurrentDirectory().listFiles()) {
        printFileInfo(file);
      }
    } else {
      std::println("Failed to copy file: {}",
                   fileErrorToString(result.error()));
    }

    // Move a file
    std::println("\nMoving a file...");
    if (auto result = manager.moveFile("destination.txt"sv, "moved_file.txt"sv);
        result) {
      std::println("File moved successfully.");
    } else {
      std::println("Failed to move file: {}",
                   fileErrorToString(result.error()));
    }

    // Delete a file
    std::println("\nDeleting a file...");
    if (auto result = manager.deleteFile("source.txt"sv); result) {
      std::println("File deleted successfully.");
    } else {
      std::println("Failed to delete file: {}",
                   fileErrorToString(result.error()));
    }

    // List subdirectories
    std::println("\nListing subdirectories:");
    for (const auto& dir : manager.getCurrentDirectory().listSubdirectories()) {
      std::println("- {}", dir.getName());
    }

    // Process files by extension
    std::println("\nProcessing files with .txt extension:");
    manager.processFilesByExtension(".txt"sv, [](const File& file) {
      std::println("Processing: {}", file.getName());
    });

    // Find files by pattern
    std::println("\nFinding files containing 'moved':");
    for (const auto& file : manager.findFiles("moved"sv)) {
      printFileInfo(file);
    }

    // Change to subdirectory
    std::println("\nChanging to subdirectory...");
    if (auto result = manager.changeDirectory(temp_path / "subdir"); result) {
      std::println("Current directory: {}",
                   manager.getCurrentDirectory().getPath().string());

      // Create a file in subdirectory
      manager.createFile("subdir_file.txt"sv,
                         "This file is in the subdirectory."sv);

      // List files in subdirectory
      std::println("\nListing files in subdirectory:");
      std::println("{:<20} | {:<10} | {}", "Name", "Size", "Modified");
      std::println("{:-<20}-+-{:-<10}-+-{:-<20}", "", "", "");
      for (const auto& file : manager.getCurrentDirectory().listFiles()) {
        printFileInfo(file);
      }
    } else {
      std::println("Failed to change directory: {}",
                   fileErrorToString(result.error()));
    }

    // Cleanup demonstration files
    std::println("\nCleaning up demonstration files...");
    try {
      std::filesystem::remove_all(temp_path);
      std::println("Cleanup successful.");
    } catch (const std::exception& e) {
      std::println("Error during cleanup: {}", e.what());
    }

  } catch (const std::exception& e) {
    std::println("Demonstration failed: {}", e.what());
  }
}

int main() {
  std::println("File Manager Demonstration");
  std::println("==========================\n");

  demo();

  return 0;
}
