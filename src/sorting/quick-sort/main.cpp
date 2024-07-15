#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

template <typename T>
T medianOfThreePivot(std::vector<T>& arr, size_t low, size_t high) {
  // Ensure the high index is greater than the low index
  if (high <= low) throw std::invalid_argument("high must be greater than low");

  size_t mid = low + (high - low) / 2;
  // Sort the three elements at low, mid, and high
  if (arr[mid] < arr[low]) std::swap(arr[mid], arr[low]);
  if (arr[high] < arr[low]) std::swap(arr[high], arr[low]);
  if (arr[high] < arr[mid]) std::swap(arr[high], arr[mid]);

  // Place the median just before the last element as the pivot
  std::swap(arr[mid], arr[high - 1]);

  return arr[high - 1];  // Return the pivot value
}

template <typename T>
size_t partition(std::vector<T>& arr, size_t low, size_t high) {
  if (high <= low + 1) return low;  // Early exit for trivial cases

  T pivot = medianOfThreePivot(arr, low, high);
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

template <typename T>
void quick_sort(std::vector<T>& arr, size_t low, size_t high) {
  if (low < high) {
    // p is partitioning index, arr[pi] is now at right place
    size_t p = partition(arr, low, high);

    // Separately sort elements before partition and after partition
    if (p > 0) quick_sort(arr, low, p - 1);  // Check added to avoid underflow
    quick_sort(arr, p + 1, high);
  }
}

void printArray(const std::vector<int>& arr) {
  for (const auto& elem : arr) {
    std::cout << elem << " ";
  }
  std::cout << "\n";
}

int main() {
  std::vector<int> arr = {3, 6, 8, 10, 1, 2, 1};

  std::cout << "Original array: ";
  printArray(arr);

  quick_sort(arr, 0, arr.size() - 1);

  std::cout << "Sorted array: ";
  printArray(arr);

  return EXIT_SUCCESS;
}