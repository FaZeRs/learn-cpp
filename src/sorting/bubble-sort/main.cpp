#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>

void bubble_sort(std::vector<int>& arr) {
  size_t n = arr.size();
  while (true) {
    bool swapped = false;
    for (size_t i = 1; i < n; ++i) {
      if (arr[i - 1] > arr[i]) {
        std::swap(arr[i - 1], arr[i]);
        swapped = true;
      }
    }
    // Each iteration sorts the next largest element, so we can reduce n
    --n;
    if (!swapped) {
      break;
    }
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

  // Perform bubble sort
  bubble_sort(arr);

  std::cout << "Sorted array: ";
  printArray(arr);

  return EXIT_SUCCESS;
}