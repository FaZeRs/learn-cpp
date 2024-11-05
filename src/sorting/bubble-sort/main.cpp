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

template <Comparable T>
constexpr void bubble_sort(std::span<T> data) noexcept {
  if (data.empty()) return;
  for (auto i : std::views::iota(0uz, data.size() - 1)) {
    bool swapped = false;
    for (auto j : std::views::iota(0uz, data.size() - i - 1)) {
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

  std::println("Original array:");
  for (const auto& elem : arr) {
    std::print("{} ", elem);
  }
  std::println("");

  bubble_sort<int>(std::span{arr});

  std::println("Sorted array:");
  for (const auto& elem : arr) {
    std::print("{} ", elem);
  }
  std::println("");

  return EXIT_SUCCESS;
}