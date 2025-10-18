#include <algorithm>
#include <array>
#include <concepts>
#include <iterator>
#include <mutex>
#include <optional>
#include <print>
#include <ranges>
#include <span>
#include <stdexcept>

template <typename T, size_t Size>
  requires std::default_initializable<T> && std::copyable<T>
class CircularBuffer
    : public std::ranges::view_interface<CircularBuffer<T, Size>> {
  static_assert(Size > 0, "Buffer size must be greater than 0");

 public:
  using value_type = T;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = T&;
  using const_reference = const T&;

  constexpr CircularBuffer() = default;
  constexpr CircularBuffer(std::initializer_list<T> init) noexcept {
    for (const auto& item : init) {
      push(item);
    }
  }

  constexpr void push(const T& item) noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    buffer.at(write_pos) = item;
    advance_write_pos();
    if (full()) [[unlikely]] {
      advance_read_pos();
    } else {
      ++count;
    }
  }

  template <typename... Args>
    requires std::constructible_from<T, Args...>
  constexpr void emplace(Args&&... args) noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    buffer.at(write_pos) = T(std::forward<Args>(args)...);
    advance_write_pos();
    if (full()) [[unlikely]] {
      advance_read_pos();
    } else {
      ++count;
    }
  }

  [[nodiscard]] constexpr std::optional<T> pop() noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    if (empty()) [[unlikely]] {
      return std::nullopt;
    }

    T item = buffer.at(read_pos);
    advance_read_pos();
    --count;
    return item;
  }

  [[nodiscard]] constexpr std::span<const T> view() const noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    if (empty()) [[unlikely]] {
      return std::span<const T, 0>{};
    }
    auto buffer_span = std::span{buffer};
    if (write_pos > read_pos) {
      return buffer_span.subspan(read_pos, count);
    }
    static std::array<T, Size> wrapped_view{};
    std::copy(buffer.begin() + read_pos, buffer.end(), wrapped_view.begin());
    std::copy(buffer.begin(), buffer.begin() + write_pos,
              wrapped_view.begin() + (Size - read_pos));
    return std::span<const T>{wrapped_view.data(), count};
  }

  [[nodiscard]] constexpr bool empty() const noexcept { return count == 0; }
  [[nodiscard]] constexpr bool full() const noexcept { return count == Size; }
  [[nodiscard]] constexpr size_t size() const noexcept { return count; }

  [[nodiscard]] constexpr auto begin() const noexcept { return view().begin(); }
  [[nodiscard]] constexpr auto end() const noexcept { return view().end(); }
  [[nodiscard]] constexpr auto rbegin() const noexcept {
    return view().rbegin();
  }
  [[nodiscard]] constexpr auto rend() const noexcept { return view().rend(); }
  [[nodiscard]] constexpr auto size_hint() const noexcept { return size(); }
  [[nodiscard]] constexpr size_type capacity() const noexcept { return Size; }
  [[nodiscard]] constexpr bool contains(const T& value) const noexcept {
    return std::ranges::find(*this, value) != this->end();
  }

 private:
  std::array<T, Size> buffer{};
  size_t read_pos{0};
  size_t write_pos{0};
  size_t count{0};
  mutable std::mutex mutex_;

  constexpr void advance_read_pos() noexcept {
    read_pos = (read_pos + 1) % Size;
  }

  constexpr void advance_write_pos() noexcept {
    write_pos = (write_pos + 1) % Size;
  }
};

template <typename T, typename... U>
CircularBuffer(T, U...) -> CircularBuffer<T, 1 + sizeof...(U)>;

int main() {
  CircularBuffer buffer{1, 2, 3, 4, 5};

  std::println("Buffer with indices:");
  for (const auto& item : buffer) {
    std::print("{} ", item);
  }
  std::println("");

  auto even_numbers =
      buffer | std::views::filter([](int n) { return n % 2 == 0; });
  std::println("Even numbers: ");
  for (const auto& item : even_numbers) {
    std::print("{} ", item);
  }
  std::println("");

  auto sum = std::ranges::fold_left(buffer, 0, std::plus{});
  auto [min, max] = std::ranges::minmax(buffer);
  std::println("Statistics:");
  std::println("  Sum: {}", sum);
  std::println("  Min: {}", min);
  std::println("  Max: {}", max);
  std::println("");

  buffer.push(6);
  std::println("Reversed buffer: ");
  for (const auto& item : buffer | std::views::reverse) {
    std::print("{} ", item);
  }
  std::println("");

  std::println("Contains 3: {}", buffer.contains(3));
  std::println("Contains 7: {}", buffer.contains(7));

  buffer.emplace(7);
  std::println("Buffer with indices:");
  for (const auto& [idx, item] : buffer | std::views::enumerate) {
    std::println("  [{}]: {}", idx, item);
  }
  std::println("");

  std::println("Popped: ");
  while (const auto item = buffer.pop()) {
    std::print("{} ", *item);
  }
  std::println("");

  return 0;
}
