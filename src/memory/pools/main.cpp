// IWYU pragma: no_include <bits/chrono.h>
#include <chrono>  // IWYU pragma: keep
#include <concepts>
#include <cstddef>
#include <functional>
#include <latch>
#include <memory>
#include <mutex>
#include <print>
#include <shared_mutex>
#include <thread>
#include <vector>

class MemoryBlock {
 public:
  MemoryBlock() = default;
  MemoryBlock(const MemoryBlock&) = delete;
  MemoryBlock& operator=(const MemoryBlock&) = delete;
  MemoryBlock(MemoryBlock&&) noexcept = default;
  MemoryBlock& operator=(MemoryBlock&&) noexcept = default;
  virtual ~MemoryBlock() = default;

  // cppcheck-suppress functionStatic
  virtual void reset() { /* Reset the block to its initial state */ }
};

template <typename T>
concept Poolable = std::derived_from<T, MemoryBlock> && requires(T a) {
  { a.reset() } -> std::same_as<void>;
};

class MemoryPool {
 public:
  MemoryPool() = default;

  void init(size_t size = 1024) {
    std::println("Initializing memory pool with size: {}", size);
    for (size_t i = 0; i < size; ++i) {
      pool_.emplace_back(std::make_unique<MemoryBlock>());
    }
    std::println("Memory pool initialized with {} blocks", pool_.size());
  }

  template <Poolable T>
  T* allocate() {
    std::shared_lock lock(mutex_);
    if (pool_.empty()) {
      expandPool(10);
    }
    T* block = static_cast<T*>(pool_.back().release());
    pool_.pop_back();
    std::println("Allocated a block. Pool size now: {}", pool_.size());
    return block;
  }

  template <Poolable T>
  void deallocate(T* block) {
    static_assert(std::is_base_of_v<MemoryBlock, T>,
                  "T must be derived from MemoryBlock");
    std::unique_lock lock(mutex_);
    block->reset();
    pool_.emplace_back(std::unique_ptr<MemoryBlock>(block));
    std::println("Deallocated a block. Pool size now: {}", pool_.size());
  }

 private:
  std::vector<std::unique_ptr<MemoryBlock>,
              std::allocator<std::unique_ptr<MemoryBlock>>>
      pool_;
  mutable std::shared_mutex mutex_;

  void expandPool(size_t size = 1024) {
    std::println("Expanding pool by {}", size);
    for (size_t i = 0; i < size; ++i) {
      pool_.emplace_back(std::make_unique<MemoryBlock>());
    }
  }
};

void worker(MemoryPool& pool, std::latch& latch) {
  for (int i = 0; i < 100; ++i) {
    auto block = pool.allocate<MemoryBlock>();
    if (block) {
      // Simulate work...
      std::println("Worker is working...");
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      pool.deallocate(block);
    }
  }
  latch.count_down();
}

int main() {
  constexpr int pool_size = 50;
  MemoryPool pool;
  pool.init(pool_size);

  constexpr int num_workers = 5;
  std::latch latch(num_workers);
  std::vector<std::jthread> workers;
  workers.reserve(num_workers);
  for (int i = 0; i < num_workers; ++i) {
    workers.emplace_back(worker, std::ref(pool), std::ref(latch));
  }
  latch.wait();
  std::println("All workers completed.");
  return 0;
}
