#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>

template <typename T>
class MaxHeap {
 public:
  MaxHeap() = default;

  void insert(const T& value) {
    data.emplace_back(value);
    heapifyUp(data.size() - 1);
  }

  void removeMax() {
    if (data.empty()) return;
    data[0] = data.back();
    data.pop_back();
    if (!data.empty()) heapifyDown(0);
  }

  [[nodiscard]] const T& getMax() const {
    if (data.empty()) throw std::runtime_error("Heap is empty");
    return data[0];
  }

  void print() const {
    for (const auto& value : data) {
      std::cout << value << " ";
    }
    std::cout << "\n";
  }

 private:
  std::vector<T> data;

  void heapifyUp(size_t index) {
    while (index > 0 && data[parent(index)] < data[index]) {
      std::swap(data[parent(index)], data[index]);
      index = parent(index);
    }
  }

  void heapifyDown(size_t index) {
    while (true) {
      size_t leftChild = left(index);
      size_t rightChild = right(index);
      size_t largestChild = index;

      if (leftChild < data.size() && data[leftChild] > data[index]) {
        largestChild = leftChild;
      }
      if (rightChild < data.size() && data[rightChild] > data[largestChild]) {
        largestChild = rightChild;
      }
      if (largestChild == index) {
        break;
      }

      std::swap(data[index], data[largestChild]);
      index = largestChild;
    }
  }

  [[nodiscard]] static size_t parent(size_t index) noexcept {
    return (index - 1) / 2;
  }

  [[nodiscard]] static size_t left(size_t index) noexcept {
    return 2 * index + 1;
  }

  [[nodiscard]] static size_t right(size_t index) noexcept {
    return 2 * index + 2;
  }
};

int main() {
  MaxHeap<int> heap;

  heap.insert(3);
  heap.insert(1);
  heap.insert(15);
  heap.insert(5);
  heap.insert(4);
  heap.insert(45);
  heap.insert(25);

  heap.print();
  heap.removeMax();
  heap.print();
  heap.removeMax();
  heap.print();

  return EXIT_SUCCESS;
}
