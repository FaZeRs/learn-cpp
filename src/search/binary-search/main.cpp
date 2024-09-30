#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <print>
#include <ranges>
#include <utility>

template <typename T>
concept Comparable = std::totally_ordered<T>;

template <std::ranges::random_access_range Range, Comparable T>
constexpr auto binary_search(Range&& range, const T& value) noexcept {
  return std::ranges::binary_search(std::forward<Range>(range), value);
}

template <typename Derived>
struct BinarySearchable {
  [[nodiscard]] constexpr auto contains(const auto& value) const {
    return binary_search(static_cast<const Derived&>(*this), value);
  }
};

template <Comparable T, std::size_t N>
struct Data : BinarySearchable<Data<T, N>> {
  std::array<T, N> arr;

  // cppcheck-suppress noExplicitConstructor
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr Data(std::array<T, N> init_arr) : arr(std::move(init_arr)) {
    std::ranges::sort(arr);
  }
  constexpr Data() : arr{} {}

  [[nodiscard]] constexpr auto begin() const noexcept { return arr.begin(); }
  [[nodiscard]] constexpr auto end() const noexcept { return arr.end(); }
  [[nodiscard]] constexpr auto operator[](std::size_t index) const -> const T& {
    assert(index < N && "Index out of bounds");
    return arr[index];
  }
};

int main() {
  constexpr int target = 5;

  constexpr Data<int, 9> data = {{1, 2, 3, 4, 5, 6, 7, 8, 9}};
  static_assert(std::ranges::is_sorted(data));
  if constexpr (data.contains(target)) {
    std::println("Found {} in the array.", target);
  } else {
    std::println("Did not find {} in the array.", target);
  }

  for (const auto& [index, value] : std::views::enumerate(data)) {
    std::println("data[{}] = {}", index, value);
  }

  return 0;
}
