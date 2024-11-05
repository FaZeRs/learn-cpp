#include <concepts>
#include <cstdlib>
#include <print>
#include <ranges>
#include <stdexcept>
#include <utility>

template <typename T>
concept Sortable = std::totally_ordered<T> && std::movable<T>;

template <typename T>
[[nodiscard]] constexpr T medianOfThreePivot(std::span<T> arr) {
  if (arr.size() < 2) {
    throw std::invalid_argument("span size must be at least 2");
  }

  size_t low = 0;
  size_t high = arr.size() - 1;
  size_t mid = high / 2;

  // Sort the three elements at low, mid, and high
  if (arr[mid] < arr[low]) std::swap(arr[mid], arr[low]);
  if (arr[high] < arr[low]) std::swap(arr[high], arr[low]);
  if (arr[high] < arr[mid]) std::swap(arr[high], arr[mid]);

  // Place the median just before the last element as the pivot
  std::swap(arr[mid], arr[high - 1]);

  return arr[high - 1];  // Return the pivot value
}

template <Sortable T>
[[nodiscard]] constexpr size_t partition(std::span<T> arr) {
  if (arr.size() <= 2) return 0;

  T pivot = medianOfThreePivot(arr);
  size_t low = 0;
  size_t high = arr.size() - 1;

  size_t i = low;
  size_t j = high - 1;

  while (true) {
    while (arr[i] < pivot) i++;
    while (pivot < arr[j]) j--;
    if (i >= j) break;
    std::swap(arr[i], arr[j]);
    i++;
    j--;
  }

  // Ensuring pivot is at its correct position
  if (i < high) {
    std::swap(arr[i], arr[high - 1]);
  }

  return i;
}

template <typename Range>
concept SortableRange = std::ranges::random_access_range<Range> &&
                        Sortable<std::ranges::range_value_t<Range>>;

template <SortableRange Range>
constexpr void quick_sort(Range&& range) {
  quick_sort(std::span{std::forward<Range>(range)});
}

template <Sortable T>
constexpr void quick_sort(std::span<T> arr) noexcept {
  if (arr.size() < 2) return;

  size_t p = partition(arr);

  if (p > 0) {
    quick_sort(arr.subspan(0, p));
  }
  quick_sort(arr.subspan(p + 1));
}

int main() {
  std::array<int, 7> arr = {3, 6, 8, 10, 1, 2, 1};

  auto print_array = [](const auto& container) {
    for (const auto& elem : std::views::all(container)) {
      std::print("{} ", elem);
    }
    std::println("");
  };

  std::println("Original array: ");
  print_array(arr);

  quick_sort<int>(arr);

  std::println("Sorted array: ");
  print_array(arr);

  return EXIT_SUCCESS;
}