#include <array>
#include <concepts>
#include <cstdlib>
#include <print>
#include <ranges>
#include <span>
#include <utility>

template <typename T>
concept Comparable = requires(T a, T b) {
  { a < b } -> std::convertible_to<bool>;
  { a > b } -> std::convertible_to<bool>;
};

template <typename Range>
concept ComparableRange = std::ranges::random_access_range<Range> &&
                          Comparable<std::ranges::range_value_t<Range>>;

template <ComparableRange Range>
constexpr void bubble_sort(Range&& range) {
  bubble_sort(std::span{std::forward<Range>(range)});
}

template <Comparable T>
constexpr void bubble_sort(std::span<T> data) noexcept {
  if (data.empty()) return;
  for (std::size_t i = 0; i < data.size() - 1; ++i) {
    bool swapped = false;
    for (std::size_t j = 0; j < data.size() - i - 1; ++j) {
      if (data[j] > data[j + 1]) {
        std::ranges::swap(data[j], data[j + 1]);
        swapped = true;
      }
    }
    if (!swapped) break;  // Early exit if no swaps needed
  }
}

int main() {
  std::array<int, 7> arr = {3, 6, 8, 10, 1, 2, 1};

  auto print_array = [](const auto& container) {
    for (const auto& elem : std::views::all(container)) {
      std::print("{} ", elem);
    }
    std::println("");
  };

  std::println("Original array:");
  print_array(arr);

  bubble_sort<int>(arr);

  std::println("Sorted array:");
  print_array(arr);

  return EXIT_SUCCESS;
}
