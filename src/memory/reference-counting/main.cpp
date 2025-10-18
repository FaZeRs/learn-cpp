#include <atomic>
#include <cassert>
#include <compare>
#include <concepts>
#include <format>
#include <iostream>
#include <memory>
#include <print>
#include <source_location>
#include <utility>

template <typename T>
  requires std::destructible<T>
class RefCounted {
 public:
  template <typename... Args>
  constexpr explicit RefCounted(Args&&... args) noexcept(
      std::is_nothrow_constructible_v<T, Args...>)
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
      : ctrl_(new ControlBlock(new T(std::forward<Args>(args)...))) {}

  constexpr RefCounted(const RefCounted& other) noexcept : ctrl_(other.ctrl_) {
    ctrl_->count++;
  }

  [[nodiscard]] constexpr RefCounted(RefCounted&& other) noexcept
      : ctrl_(std::exchange(other.ctrl_, nullptr)) {}

  constexpr RefCounted& operator=(const RefCounted& other) noexcept {
    if (this != &other) {
      release();
      ctrl_ = other.ctrl_;
      ctrl_->count++;
    }
    return *this;
  }

  constexpr RefCounted& operator=(RefCounted&& other) noexcept {
    if (this != &other) {
      release();
      ctrl_ = std::exchange(other.ctrl_, nullptr);
    }
    return *this;
  }

  ~RefCounted() { release(); }

  [[nodiscard]] friend constexpr auto operator<=>(
      const RefCounted& lhs, const RefCounted& rhs) noexcept {
    return lhs.ctrl_ <=> rhs.ctrl_;
  }

  [[nodiscard]] friend constexpr bool operator==(
      const RefCounted& lhs, const RefCounted& rhs) noexcept = default;

  [[nodiscard]] size_t use_count() const noexcept {  // C++17 attribute
    return ctrl_ ? ctrl_->count.load() : 0;
  }

  [[nodiscard]] constexpr bool unique() const noexcept {
    return use_count() == 1;
  }

  T& operator*() noexcept {
    assert(ctrl_ && "Dereferencing null pointer");
    return *ctrl_->data;
  }
  const T& operator*() const noexcept {
    assert(ctrl_ && "Dereferencing null pointer");
    return *ctrl_->data;
  }
  T* operator->() noexcept {
    assert(ctrl_ && "Dereferencing null pointer");
    return ctrl_->data;
  }
  const T* operator->() const noexcept {
    assert(ctrl_ && "Dereferencing null pointer");
    return ctrl_->data;
  }

  [[nodiscard]] constexpr T* get() const noexcept {
    return ctrl_ ? ctrl_->data : nullptr;
  }

  [[nodiscard]] constexpr explicit operator bool() const noexcept {
    return ctrl_ != nullptr;
  }

  [[nodiscard]] constexpr std::string debug_info() const {
    if (!ctrl_) return "null reference";
    return std::format("RefCounted[count={}, created at {}:{}, addr={}]",
                       ctrl_->count.load(), ctrl_->creation_loc.file_name(),
                       ctrl_->creation_loc.line(),
                       static_cast<const void*>(ctrl_->data));
  }

  void reset() noexcept {
    release();
    ctrl_ = nullptr;
  }

 private:
  void release() {
    if (ctrl_ && --ctrl_->count == 0) {
      delete ctrl_;
    }
  }

  struct ControlBlock {
    T* data;
    std::atomic<size_t> count;
    std::source_location creation_loc;

    constexpr explicit ControlBlock(T* ptr, const std::source_location& loc =
                                                std::source_location::current())
        : data(ptr), count(1), creation_loc(loc) {}
    ControlBlock(const ControlBlock&) = delete;
    ControlBlock& operator=(const ControlBlock&) = delete;
    ControlBlock(ControlBlock&&) = delete;
    ControlBlock& operator=(ControlBlock&&) = delete;
    ~ControlBlock() { delete data; }
  };

  ControlBlock* ctrl_;
};

class Resource {
 public:
  constexpr explicit Resource(std::string n) noexcept : name_(std::move(n)) {
    std::println("Resource '{}' constructed", name_);
  }
  virtual ~Resource() { std::println("Resource '{}' destroyed", name_); }
  constexpr Resource(const Resource&) noexcept = default;
  constexpr Resource(Resource&&) noexcept = default;
  Resource& operator=(const Resource&) noexcept = default;
  Resource& operator=(Resource&&) noexcept = default;

  [[nodiscard]] constexpr const std::string& getName() const noexcept {
    return name_;
  }

 private:
  std::string name_;
};

int main() {
  // Create a reference-counted Resource
  RefCounted<Resource> res1{"First"};
  std::println("{}", res1.debug_info());
  std::println("Reference count: {}", res1.use_count());

  {
    // Create another reference
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    auto res2 = res1;
    std::println("Resource pointer: {}", static_cast<void*>(res2.get()));
    std::println("{}", res2.debug_info());
    std::println("Reference count: {}", res1.use_count());

    // Create a third reference
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    RefCounted<Resource> res3 = res2;
    std::println("Reference count: {}", res1.use_count());

    // Access the resource
    std::println("Resource name: {}", res3->getName());
  }  // res2 and res3 go out of scope here

  std::println("Reference count: {}", res1.use_count());

  auto res2 = std::move(res1);  // res1 is now nullptr

  // cppcheck-suppress accessMoved
  std::println("Reference count: {}", res1.use_count());
  if (res2.unique()) {
    std::println("res2 is unique");
  }
  std::println("{}", res2.debug_info());
  res2.reset();  // Explicitly release the resource
  std::println("After reset: {}", res2.debug_info());

  return 0;
}
