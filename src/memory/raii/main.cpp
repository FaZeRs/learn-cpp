// IWYU pragma: no_include <__fwd/fstream.h>
// IWYU pragma: no_include <__fwd/ios.h>
#include <expected>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

template <typename T>
concept FilePath = requires(T t) {
  { std::filesystem::path(t) };
};

class FileManager {
 public:
  static std::expected<FileManager, std::string> create(
      const std::filesystem::path& filePath) {
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::app);
    if (!file.is_open()) {
      return std::unexpected{"Failed to open the file: " + filePath.string()};
    }
    return FileManager{std::move(file), filePath};
  }

  FileManager(const FileManager&) = delete;
  FileManager& operator=(const FileManager&) = delete;

  FileManager(FileManager&& other) noexcept
      : file_(std::move(other.file_)), filePath_(std::move(other.filePath_)) {
    other.file_.close();
  }

  FileManager& operator=(FileManager&& other) noexcept {
    if (this != &other) {
      file_.close();
      file_ = std::move(other.file_);
      filePath_ = std::move(other.filePath_);
      other.file_.close();
    }
    return *this;
  }

  ~FileManager() {
    if (file_.is_open()) {
      file_.close();
      std::cout << "File closed: " << filePath_.string() << "\n";
    }
  }

  void write(const std::string& data) {
    if (file_.is_open()) {
      file_ << data << "\n";
    }
  }

 private:
  std::fstream file_;
  std::filesystem::path filePath_;

  FileManager(std::fstream file, const std::filesystem::path& filePath)
      : file_(std::move(file)), filePath_(filePath) {
    std::cout << "File opened: " << filePath.string() << "\n";
  }
};

template <FilePath T>
void processFile(T&& filePath) {
  auto fileManager = FileManager::create(std::forward<T>(filePath));
  if (!fileManager) {
    std::cerr << fileManager.error() << "\n";
    return;
  }
  fileManager->write("Hello, World!");
}

int main() {
  processFile("example.txt");
  return 0;
}
