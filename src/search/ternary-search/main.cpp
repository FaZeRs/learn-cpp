#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <print>
#include <ranges>
#include <utility>

template <typename T>
concept Comparable = std::totally_ordered<T>;

template <Comparable T, std::size_t N>
constexpr bool ternary_search(const std::array<T, N>& arr, const T& value,
                              std::size_t left, std::size_t right) {
  if (left > right) {
    return false;
  }

  std::size_t mid1 = left + (right - left) / 3;
  std::size_t mid2 = right - (right - left) / 3;

  if (arr.at(mid1) == value || arr.at(mid2) == value) {
    return true;
  }

  if (value < arr.at(mid1)) {
    return ternary_search(arr, value, left, mid1 - 1);
  }

  if (value > arr.at(mid2)) {
    return ternary_search(arr, value, mid2 + 1, right);
  }

  return ternary_search(arr, value, mid1 + 1, mid2 - 1);
}

template <typename Derived>
struct Searchable {
  [[nodiscard]] constexpr auto contains(const auto& value) const {
    const auto& self = static_cast<const Derived&>(*this);
    return ternary_search(self.arr, value, 0, self.arr.size() - 1);
  }
};

template <Comparable T, std::size_t N>
struct Data : Searchable<Data<T, N>> {
  std::array<T, N> arr;

  // cppcheck-suppress noExplicitConstructor
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr Data(std::array<T, N> init_arr) : arr(std::move(init_arr)) {}
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
