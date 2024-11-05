# Array Data Structure

An array is a fundamental data structure that stores elements of the same type in contiguous memory locations. It is one of the most basic and widely used data structures in computer programming.

## Characteristics

- Fixed size (in most implementations)
- Contiguous memory allocation
- Constant-time access to elements using indices
- Same data type for all elements
- Zero-based indexing (in most programming languages)

## Time Complexity

| Operation     | Average Case | Worst Case |
|--------------|--------------|------------|
| Access       | O(1)         | O(1)       |
| Search       | O(n)         | O(n)       |
| Insertion    | O(n)         | O(n)       |
| Deletion     | O(n)         | O(n)       |

## Common Operations

### Basic Operations
- Accessing elements by index
- Inserting elements
- Deleting elements
- Updating elements
- Traversing the array

### Advanced Operations
- Sorting
- Searching
- Reversing
- Rotating
- Merging

## Advantages

- Simple and easy to use
- Fast access to elements (constant time)
- Efficient memory usage for fixed-size data
- Cache-friendly due to memory locality

## Disadvantages

- Fixed size (in static arrays)
- Insertion and deletion operations are expensive
- Memory waste if size is overestimated
- Contiguous memory requirement

## Applications

1. Storing and managing collections of similar items
2. Implementation of other data structures (stacks, queues)
3. Buffer for storing data
4. Lookup tables and matrices
5. Dynamic programming solutions

## Implementation Examples

## Implementation Example

### C++ Example
```c++
#include <iostream>
#include <array>
#include <algorithm>

int main() {
    // Declare and initialize a fixed-size array
    std::array<int, 5> arr = {1, 2, 3, 4, 5};

    // Basic operations
    std::cout << "Array size: " << arr.size() << '\n';
    std::cout << "First element: " << arr.front() << '\n';
    std::cout << "Last element: " << arr.back() << '\n';
    
    // Access elements
    std::cout << "Element at index 2: " << arr[2] << '\n';          // No bounds checking
    std::cout << "Element at index 2: " << arr.at(2) << '\n';       // With bounds checking
    
    // Modify elements
    arr[0] = 10;
    arr.at(4) = 50;
    
    // Iterate through array
    std::cout << "All elements: ";
    for (const auto& element : arr) {
        std::cout << element << " ";
    }
    std::cout << '\n';
    
    // Check if array is empty
    std::cout << "Is array empty? " << (arr.empty() ? "Yes" : "No") << '\n';
    
    // Fill array with a value
    arr.fill(7);
    std::cout << "After filling with 7: ";
    for (const auto& element : arr) {
        std::cout << element << " ";
    }
    std::cout << '\n';
    
    return 0;
}
```

## Related Data Structures

- Dynamic Array
- Matrix
- ArrayList
- Vector

## References

- [GeeksforGeeks - Array Data Structure](https://www.geeksforgeeks.org/array-data-structure/)
- [Wikipedia - Array data structure](https://en.wikipedia.org/wiki/Array_data_structure)