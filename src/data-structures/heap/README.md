# Heap Data Structure

A Heap is a specialized tree-based data structure that satisfies the heap property. It's commonly implemented as a complete binary tree, where all levels are fully filled except possibly the last level, which is filled from left to right.

## Types of Heaps

1. **Max Heap**: Parent nodes are always greater than or equal to their children
   - Root node contains the maximum element
2. **Min Heap**: Parent nodes are always less than or equal to their children
   - Root node contains the minimum element

## Properties

- Complete Binary Tree
- Heap Property (either Min or Max)
- Height: O(log n)
- Array Representation: For a node at index i
  - Left Child: 2i + 1
  - Right Child: 2i + 2
  - Parent: (i - 1) / 2

## Time Complexity

| Operation     | Time Complexity |
|--------------|-----------------|
| Insert       | O(log n)        |
| Delete       | O(log n)        |
| Get Min/Max  | O(1)            |
| Heapify      | O(n)            |

## Common Applications

- Priority Queues
- Heap Sort
- Graph Algorithms (Dijkstra's, Prim's)
- Memory Management
- Event-driven Simulation
- Task Scheduling

## Basic Operations

### Insert (Push)
1. Add element at the end
2. Bubble up until heap property is satisfied

### Delete (Pop)
1. Remove root element
2. Replace root with last element
3. Bubble down until heap property is satisfied

## Usage Example

```c++
#include <iostream>
#include <queue>
#include <vector>

int main() {
  // Min heap
  std::priority_queue<int, std::vector<int>, std::greater<int>> min_heap;

  // Insert elements
  min_heap.push(5);
  min_heap.push(3);
  min_heap.push(7);
  min_heap.push(1);

  std::cout << min_heap.top() << std::endl;  // Output: 1
  min_heap.pop();
  std::cout << min_heap.top() << std::endl;  // Output: 3

  // Max heap (default behavior)
  std::priority_queue<int> max_heap;
  max_heap.push(5);
  max_heap.push(3);
  max_heap.push(7);
  max_heap.push(1);

  std::cout << max_heap.top() << std::endl;  // Output: 7

  return 0;
}
```

## References

- [Wikipedia - Heap Data Structure](https://en.wikipedia.org/wiki/Heap_(data_structure))
- [GeeksforGeeks - Binary Heap](https://www.geeksforgeeks.org/binary-heap/)