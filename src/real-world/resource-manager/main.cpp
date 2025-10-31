#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <format>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <print>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

/**
 * @brief Resource manager for loading, caching, and managing application
 * assets
 *
 * This demonstrates:
 * - RAII resource management
 * - Reference counting with shared_ptr
 * - LRU cache eviction
 * - Lazy loading
 * - Resource lifecycle tracking
 * - Type-safe resource handles
 */

/**
 * @brief Error types for resource operations
 */
enum class ResourceError : std::uint8_t {
  NotFound,
  LoadFailed,
  InvalidPath,
  AlreadyExists,
  CacheFull,
  TypeMismatch
};

/**
 * @brief Convert resource error to string
 */
[[nodiscard]] constexpr std::string_view resourceErrorToString(
    ResourceError error) {
  switch (error) {
    case ResourceError::NotFound:
      return "Resource not found";
    case ResourceError::LoadFailed:
      return "Failed to load resource";
    case ResourceError::InvalidPath:
      return "Invalid resource path";
    case ResourceError::AlreadyExists:
      return "Resource already exists";
    case ResourceError::CacheFull:
      return "Resource cache is full";
    case ResourceError::TypeMismatch:
      return "Resource type mismatch";
    default:
      return "Unknown error";
  }
}

// ============================================================================
// Resource Types (Simulated)
// ============================================================================

/**
 * @brief Base class for all resources
 */
class Resource {
 public:
  explicit Resource(std::string path) : path_(std::move(path)) {}
  virtual ~Resource() = default;

  Resource(const Resource&) = delete;
  Resource& operator=(const Resource&) = delete;
  Resource(Resource&&) = default;
  Resource& operator=(Resource&&) = default;

  [[nodiscard]] const std::string& getPath() const { return path_; }
  [[nodiscard]] virtual std::string_view getType() const = 0;
  [[nodiscard]] virtual size_t getMemoryUsage() const = 0;

 private:
  std::string path_;
};

/**
 * @brief Image resource (simulated)
 */
class ImageResource : public Resource {
 public:
  ImageResource(std::string path, int width, int height)
      : Resource(std::move(path)), width_(width), height_(height) {
    // Simulate image data
    data_.resize(static_cast<size_t>(width_) * static_cast<size_t>(height_) *
                 4);
  }

  [[nodiscard]] std::string_view getType() const override { return "Image"; }
  [[nodiscard]] size_t getMemoryUsage() const override { return data_.size(); }
  [[nodiscard]] int getWidth() const { return width_; }
  [[nodiscard]] int getHeight() const { return height_; }

 private:
  int width_;
  int height_;
  std::vector<std::uint8_t> data_;
};

/**
 * @brief Font resource (simulated)
 */
class FontResource : public Resource {
 public:
  FontResource(std::string path, int size)
      : Resource(std::move(path)), size_(size) {
    // Simulate font data
    font_data_.resize(1024 * static_cast<size_t>(size_));
  }

  [[nodiscard]] std::string_view getType() const override { return "Font"; }
  [[nodiscard]] size_t getMemoryUsage() const override {
    return font_data_.size();
  }
  [[nodiscard]] int getSize() const { return size_; }

 private:
  int size_;
  std::vector<std::uint8_t> font_data_;
};

/**
 * @brief Config resource (simulated)
 */
class ConfigResource : public Resource {
 public:
  explicit ConfigResource(std::string path) : Resource(std::move(path)) {
    // Simulate config loading
    config_data_["version"] = "1.0.0";
    config_data_["debug"] = "true";
  }

  [[nodiscard]] std::string_view getType() const override { return "Config"; }
  [[nodiscard]] size_t getMemoryUsage() const override {
    size_t total = 0;
    for (const auto& [key, value] : config_data_) {
      total += key.size() + value.size();
    }
    return total;
  }

  [[nodiscard]] std::optional<std::string> get(const std::string& key) const {
    auto it = config_data_.find(key);
    if (it != config_data_.end()) {
      return it->second;
    }
    return std::nullopt;
  }

  void set(const std::string& key, const std::string& value) {
    config_data_[key] = value;
  }

 private:
  std::unordered_map<std::string, std::string> config_data_;
};

/**
 * @brief Audio resource (simulated)
 */
class AudioResource : public Resource {
 public:
  AudioResource(std::string path, int duration_ms)
      : Resource(std::move(path)), duration_ms_(duration_ms) {
    // Simulate audio data (44.1kHz, 16-bit, stereo)
    audio_data_.resize(static_cast<size_t>(duration_ms_) * 44100 / 1000 * 2 *
                       2);
  }

  [[nodiscard]] std::string_view getType() const override { return "Audio"; }
  [[nodiscard]] size_t getMemoryUsage() const override {
    return audio_data_.size();
  }
  [[nodiscard]] int getDuration() const { return duration_ms_; }

 private:
  int duration_ms_;
  std::vector<std::uint8_t> audio_data_;
};

// ============================================================================
// Resource Manager
// ============================================================================

/**
 * @brief Cache entry with access tracking for LRU
 */
struct CacheEntry {
  std::shared_ptr<Resource> resource;
  std::chrono::steady_clock::time_point last_access;
  size_t access_count = 1;

  explicit CacheEntry(std::shared_ptr<Resource> res)
      : resource(std::move(res)),
        last_access(std::chrono::steady_clock::now()) {}

  void updateAccess() {
    last_access = std::chrono::steady_clock::now();
    ++access_count;
  }
};

/**
 * @brief Resource manager with caching and lifecycle management
 */
class ResourceManager {
 public:
  explicit ResourceManager(size_t max_cache_size_bytes = 100 * 1024 * 1024)
      : max_cache_size_(max_cache_size_bytes) {}

  /**
   * @brief Load an image resource
   */
  [[nodiscard]] std::expected<std::shared_ptr<ImageResource>, ResourceError>
  loadImage(const std::string& path, int width, int height) {
    return loadResource<ImageResource>(
        path, [&]() -> std::shared_ptr<ImageResource> {
          std::println("[ResourceManager] Loading image: {} ({}x{})", path,
                       width, height);
          return std::make_shared<ImageResource>(path, width, height);
        });
  }

  /**
   * @brief Load a font resource
   */
  [[nodiscard]] std::expected<std::shared_ptr<FontResource>, ResourceError>
  loadFont(const std::string& path, int size) {
    return loadResource<FontResource>(
        path, [&]() -> std::shared_ptr<FontResource> {
          std::println("[ResourceManager] Loading font: {} (size: {})", path,
                       size);
          return std::make_shared<FontResource>(path, size);
        });
  }

  /**
   * @brief Load a config resource
   */
  [[nodiscard]] std::expected<std::shared_ptr<ConfigResource>, ResourceError>
  loadConfig(const std::string& path) {
    return loadResource<ConfigResource>(
        path, [&]() -> std::shared_ptr<ConfigResource> {
          std::println("[ResourceManager] Loading config: {}", path);
          return std::make_shared<ConfigResource>(path);
        });
  }

  /**
   * @brief Load an audio resource
   */
  [[nodiscard]] std::expected<std::shared_ptr<AudioResource>, ResourceError>
  loadAudio(const std::string& path, int duration_ms) {
    return loadResource<AudioResource>(
        path, [&]() -> std::shared_ptr<AudioResource> {
          std::println("[ResourceManager] Loading audio: {} ({}ms)", path,
                       duration_ms);
          return std::make_shared<AudioResource>(path, duration_ms);
        });
  }

  /**
   * @brief Unload a specific resource by path
   */
  void unload(const std::string& path) {
    std::lock_guard lock(mutex_);

    auto it = cache_.find(path);
    if (it != cache_.end()) {
      std::println("[ResourceManager] Unloading resource: {}", path);
      current_cache_size_ -= it->second.resource->getMemoryUsage();
      cache_.erase(it);
    }
  }

  /**
   * @brief Clear all cached resources
   */
  void clear() {
    std::lock_guard lock(mutex_);
    std::println("[ResourceManager] Clearing all resources");
    cache_.clear();
    current_cache_size_ = 0;
  }

  /**
   * @brief Get current cache size in bytes
   */
  [[nodiscard]] size_t getCacheSize() const {
    std::lock_guard lock(mutex_);
    return current_cache_size_;
  }

  /**
   * @brief Get number of cached resources
   */
  [[nodiscard]] size_t getCacheCount() const {
    std::lock_guard lock(mutex_);
    return cache_.size();
  }

  /**
   * @brief Get cache statistics
   */
  struct CacheStats {
    size_t total_resources;
    size_t total_memory_bytes;
    size_t max_memory_bytes;
    std::vector<std::pair<std::string, size_t>> resources_by_size;
  };

  [[nodiscard]] CacheStats getStats() const {
    std::lock_guard lock(mutex_);

    CacheStats stats{.total_resources = cache_.size(),
                     .total_memory_bytes = current_cache_size_,
                     .max_memory_bytes = max_cache_size_,
                     .resources_by_size = {}};

    for (const auto& [path, entry] : cache_) {
      stats.resources_by_size.emplace_back(path,
                                           entry.resource->getMemoryUsage());
    }

    // Sort by size descending
    std::ranges::sort(
        stats.resources_by_size,
        [](const auto& a, const auto& b) { return a.second > b.second; });

    return stats;
  }

  /**
   * @brief Preload resources (batch loading)
   */
  template <typename LoadFunc>
  // cppcheck-suppress functionStatic
  void preloadBatch(
      const std::vector<std::pair<std::string, LoadFunc>>& items,
      const std::function<void(const std::string&, bool)>& callback = nullptr) {
    for (const auto& [path, load_func] : items) {
      try {
        load_func();
        if (callback) {
          callback(path, true);
        }
      } catch (...) {
        if (callback) {
          callback(path, false);
        }
      }
    }
  }

 private:
  /**
   * @brief Generic resource loading with caching
   */
  template <typename T, typename LoadFunc>
  [[nodiscard]] std::expected<std::shared_ptr<T>, ResourceError> loadResource(
      const std::string& path, LoadFunc&& loader) {
    std::lock_guard lock(mutex_);

    // Check if already cached
    auto it = cache_.find(path);
    if (it != cache_.end()) {
      it->second.updateAccess();

      // Verify type matches
      if (auto casted = std::dynamic_pointer_cast<T>(it->second.resource)) {
        std::println("[ResourceManager] Cache hit: {}", path);
        return casted;
      }
      return std::unexpected(ResourceError::TypeMismatch);
    }

    // Load new resource
    std::shared_ptr<Resource> resource = std::forward<LoadFunc>(loader)();
    if (!resource) {
      return std::unexpected(ResourceError::LoadFailed);
    }

    size_t resource_size = resource->getMemoryUsage();

    // Check if we need to evict resources
    while (current_cache_size_ + resource_size > max_cache_size_ &&
           !cache_.empty()) {
      evictLRU();
    }

    // Check if resource is still too large
    if (current_cache_size_ + resource_size > max_cache_size_) {
      return std::unexpected(ResourceError::CacheFull);
    }

    // Add to cache
    cache_.emplace(path, CacheEntry(resource));
    current_cache_size_ += resource_size;

    std::println("[ResourceManager] Cached resource: {} ({} bytes)", path,
                 resource_size);

    return std::dynamic_pointer_cast<T>(resource);
  }

  /**
   * @brief Evict least recently used resource
   */
  void evictLRU() {
    if (cache_.empty()) {
      return;
    }

    // Find LRU entry
    auto lru_it =
        std::ranges::min_element(cache_, [](const auto& a, const auto& b) {
          return a.second.last_access < b.second.last_access;
        });

    std::println("[ResourceManager] Evicting LRU resource: {}", lru_it->first);
    current_cache_size_ -= lru_it->second.resource->getMemoryUsage();
    cache_.erase(lru_it);
  }

  mutable std::mutex mutex_;
  std::unordered_map<std::string, CacheEntry> cache_;
  size_t max_cache_size_;
  size_t current_cache_size_ = 0;
};

// ============================================================================
// Resource Handle (RAII wrapper)
// ============================================================================

/**
 * @brief RAII handle for automatic resource unloading
 */
template <typename T>
class ResourceHandle {
 public:
  ResourceHandle(std::shared_ptr<T> resource, ResourceManager* manager,
                 std::string path)
      : resource_(std::move(resource)),
        manager_(manager),
        path_(std::move(path)) {}

  ~ResourceHandle() {
    if (auto_unload_ && manager_) {
      manager_->unload(path_);
    }
  }

  ResourceHandle(const ResourceHandle&) = delete;
  ResourceHandle& operator=(const ResourceHandle&) = delete;

  ResourceHandle(ResourceHandle&& other) noexcept
      : resource_(std::move(other.resource_)),
        manager_(other.manager_),
        path_(std::move(other.path_)),
        auto_unload_(other.auto_unload_) {
    other.manager_ = nullptr;
  }

  ResourceHandle& operator=(ResourceHandle&& other) noexcept {
    if (this != &other) {
      if (auto_unload_ && manager_) {
        manager_->unload(path_);
      }
      resource_ = std::move(other.resource_);
      manager_ = other.manager_;
      path_ = std::move(other.path_);
      auto_unload_ = other.auto_unload_;
      other.manager_ = nullptr;
    }
    return *this;
  }

  [[nodiscard]] T* get() const { return resource_.get(); }
  [[nodiscard]] T* operator->() const { return resource_.get(); }
  [[nodiscard]] T& operator*() const { return *resource_; }
  [[nodiscard]] explicit operator bool() const { return resource_ != nullptr; }

  void setAutoUnload(bool enable) { auto_unload_ = enable; }

 private:
  std::shared_ptr<T> resource_;
  ResourceManager* manager_;
  std::string path_;
  bool auto_unload_ = false;
};

// ============================================================================
// Demonstrations
// ============================================================================

/**
 * @brief Demonstrate basic resource loading and caching
 */
void demonstrateBasicLoading() {
  std::println("\n=== Basic Resource Loading ===\n");

  ResourceManager manager(10 * 1024 * 1024);  // 10MB cache

  // Load images
  auto img1 = manager.loadImage("textures/player.png", 512, 512);
  if (img1) {
    std::println("Loaded image: {} ({}x{})", (*img1)->getPath(),
                 (*img1)->getWidth(), (*img1)->getHeight());
  }

  auto img2 = manager.loadImage("textures/enemy.png", 256, 256);
  if (img2) {
    std::println("Loaded image: {} ({}x{})", (*img2)->getPath(),
                 (*img2)->getWidth(), (*img2)->getHeight());
  }

  // Load font
  auto font = manager.loadFont("fonts/arial.ttf", 24);
  if (font) {
    std::println("Loaded font: {} (size: {})", (*font)->getPath(),
                 (*font)->getSize());
  }

  // Load config
  auto config = manager.loadConfig("settings.json");
  if (config) {
    std::println("Loaded config: {}", (*config)->getPath());
    if (auto version = (*config)->get("version")) {
      std::println("  Version: {}", *version);
    }
  }

  std::println("\nCache stats: {} resources, {} bytes", manager.getCacheCount(),
               manager.getCacheSize());
}

/**
 * @brief Demonstrate cache hit behavior
 */
void demonstrateCacheHit() {
  std::println("\n=== Cache Hit Demonstration ===\n");

  ResourceManager manager(10 * 1024 * 1024);

  // First load (cache miss)
  std::println("First load:");
  auto img1 = manager.loadImage("textures/logo.png", 1024, 1024);

  // Second load (cache hit)
  std::println("\nSecond load:");
  auto img2 = manager.loadImage("textures/logo.png", 1024, 1024);

  // Verify same resource
  if (img1 && img2) {
    std::println("\nSame resource? {}", img1->get() == img2->get());
  }
}

/**
 * @brief Demonstrate LRU eviction
 */
void demonstrateLRUEviction() {
  std::println("\n=== LRU Eviction Demonstration ===\n");

  ResourceManager manager(2 * 1024 * 1024);  // Small 2MB cache

  std::println("Max cache size: 2MB\n");

  // Load resources until cache is full
  auto img1 = manager.loadImage("textures/bg1.png", 512, 512);  // ~1MB
  std::println("Cache: {} resources, {:.2f} MB\n", manager.getCacheCount(),
               static_cast<double>(manager.getCacheSize()) / (1024.0 * 1024.0));

  auto img2 = manager.loadImage("textures/bg2.png", 512, 512);  // ~1MB
  std::println("Cache: {} resources, {:.2f} MB\n", manager.getCacheCount(),
               static_cast<double>(manager.getCacheSize()) / (1024.0 * 1024.0));

  // This should trigger eviction of img1 (LRU)
  std::println("Loading large image (will trigger eviction)...");
  auto img3 = manager.loadImage("textures/bg3.png", 512, 512);  // ~1MB
  std::println("Cache: {} resources, {:.2f} MB\n", manager.getCacheCount(),
               static_cast<double>(manager.getCacheSize()) / (1024.0 * 1024.0));
}

/**
 * @brief Demonstrate RAII resource handles
 */
void demonstrateResourceHandles() {
  std::println("\n=== RAII Resource Handles ===\n");

  ResourceManager manager(10 * 1024 * 1024);

  std::println("Creating scoped resource handle...");
  {
    auto img = manager.loadImage("textures/temp.png", 256, 256);
    if (img) {
      ResourceHandle handle(*img, &manager, "textures/temp.png");
      handle.setAutoUnload(true);
      std::println("Resource loaded: {}", handle->getPath());
      std::println("Cache count: {}", manager.getCacheCount());
    }
  }  // Handle goes out of scope, resource automatically unloaded

  std::println("\nAfter handle destruction:");
  std::println("Cache count: {}", manager.getCacheCount());
}

/**
 * @brief Demonstrate batch preloading
 */
void demonstratePreloading() {
  std::println("\n=== Batch Preloading ===\n");

  ResourceManager manager(20 * 1024 * 1024);

  std::vector<std::pair<std::string, std::function<void()>>> resources;
  resources.emplace_back("ui/button.png", [&]() {
    (void)manager.loadImage("ui/button.png", 128, 64);
  });
  resources.emplace_back("ui/panel.png", [&]() {
    (void)manager.loadImage("ui/panel.png", 512, 512);
  });
  resources.emplace_back("fonts/title.ttf", [&]() {
    (void)manager.loadFont("fonts/title.ttf", 48);
  });
  resources.emplace_back("sounds/click.wav", [&]() {
    (void)manager.loadAudio("sounds/click.wav", 100);
  });

  std::println("Preloading {} resources...", resources.size());

  manager.preloadBatch(resources, [](const std::string& path, bool success) {
    std::println("  {} - {}", path, success ? "OK" : "FAILED");
  });

  std::println("\nPreloading complete!");
  std::println("Cache: {} resources, {:.2f} MB", manager.getCacheCount(),
               static_cast<double>(manager.getCacheSize()) / (1024.0 * 1024.0));
}

/**
 * @brief Demonstrate cache statistics
 */
void demonstrateCacheStats() {
  std::println("\n=== Cache Statistics ===\n");

  ResourceManager manager(20 * 1024 * 1024);  // 20MB cache for demo

  // Load various resources
  (void)manager.loadImage("textures/large.png", 1024, 1024);
  (void)manager.loadImage("textures/medium.png", 512, 512);
  (void)manager.loadImage("textures/small.png", 128, 128);
  (void)manager.loadFont("fonts/large.ttf", 72);
  (void)manager.loadFont("fonts/small.ttf", 12);
  (void)manager.loadAudio("sounds/music.ogg", 10000);  // 10 seconds instead of 60
  (void)manager.loadConfig("game.cfg");

  auto stats = manager.getStats();

  std::println("Total Resources: {}", stats.total_resources);
  std::println(
      "Total Memory: {:.2f} MB / {:.2f} MB",
      static_cast<double>(stats.total_memory_bytes) / (1024.0 * 1024.0),
      static_cast<double>(stats.max_memory_bytes) / (1024.0 * 1024.0));
  std::println("Usage: {:.1f}%",
               (static_cast<double>(stats.total_memory_bytes) /
                static_cast<double>(stats.max_memory_bytes)) *
                   100.0);

  std::println("\nResources by size:");
  for (const auto& [path, size] : stats.resources_by_size) {
    std::println("  {:40} - {:.2f} KB", path,
                 static_cast<double>(size) / 1024.0);
  }
}

/**
 * @brief Main demonstration
 */
void demo() {
  demonstrateBasicLoading();
  demonstrateCacheHit();
  demonstrateLRUEviction();
  demonstrateResourceHandles();
  demonstratePreloading();
  demonstrateCacheStats();

  std::println("\n=== Demo Complete ===\n");
}

int main() {
  std::println("Resource Manager Demonstration");
  std::println("==============================");

  try {
    demo();
  } catch (const std::exception& e) {
    std::println("Error during demonstration: {}", e.what());
    return 1;
  }

  return 0;
}
