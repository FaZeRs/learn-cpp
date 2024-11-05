# Vector

A Vector is a dynamic array data structure that can grow or shrink in size automatically. It provides random access to elements and efficient operations at the end of the collection.

## Key Features

- Dynamic size management
- Random access to elements (O(1))
- Efficient insertion/deletion at the end (amortized O(1))
- Contiguous memory storage
- Automatic resizing when capacity is reached

## Time Complexity

| Operation               | Average Case | Worst Case |
|------------------------|--------------|------------|
| Access                 | O(1)         | O(1)       |
| Insert/Delete at end   | O(1)*        | O(n)       |
| Insert/Delete at middle| O(n)         | O(n)       |
| Search                 | O(n)         | O(n)       |

\* Amortized time complexity

## Space Complexity

- O(n) where n is the number of elements
- Additional space may be allocated for future growth

## Common Use Cases

- Dynamic lists where size is unknown at compile time
- Stack implementations
- Buffer implementations
- When frequent access to elements by index is needed
- Collections that primarily add/remove elements at the end

## Advantages

1. Fast random access to elements
2. Dynamic size management
3. Cache-friendly due to contiguous memory
4. Efficient iteration
5. Simple to use and implement

## Disadvantages

1. Insertion/deletion in the middle is expensive
2. May waste memory due to over-allocation
3. Resizing operations can be costly
4. Fixed contiguous memory requirement

## Implementation Notes

The vector typically maintains:
- A pointer to the underlying array
- Current size (number of elements)
- Current capacity (total available space)

When the capacity is reached, the vector typically:
1. Allocates a new array with larger capacity (often 1.5x or 2x)
2. Copies existing elements to the new array
3. Deallocates the old array
4. Updates the internal pointer

## Implementation Example

```cpp
#include <vector>
#include <iostream>

int main() {
    // Initialize vector
    std::vector<int> vec = {1, 2, 3, 4, 5};

    // Add elements
    vec.push_back(6);                  // Add to end: [1,2,3,4,5,6]
    
    // Access elements
    std::cout << "First element: " << vec[0] << std::endl;         // Using operator[]
    std::cout << "Second element: " << vec.at(1) << std::endl;     // Using at() with bounds checking
    
    // Modify elements
    vec[0] = 10;                       // Direct assignment
    
    // Size operations
    std::cout << "Size: " << vec.size() << std::endl;
    std::cout << "Capacity: " << vec.capacity() << std::endl;
    
    // Insert in the middle
    vec.insert(vec.begin() + 2, 15);   // Insert 15 at index 2
    
    // Remove elements
    vec.pop_back();                    // Remove last element
    vec.erase(vec.begin());            // Remove first element
    
    // Iterate over elements
    for(const auto& element : vec) {
        std::cout << element << " ";
    }
    std::cout << std::endl;
    
    // Clear vector
    vec.clear();                       // Remove all elements
    
    // Check if empty
    if(vec.empty()) {
        std::cout << "Vector is empty" << std::endl;
    }
    
    // Reserve capacity
    vec.reserve(100);                  // Reserve space for 100 elements
    
    // Resize vector
    vec.resize(5, 1);                  // Resize to 5 elements, fill with 1s
    
    return 0;
}
```

## References

- [Dynamic Array](https://en.wikipedia.org/wiki/Dynamic_array)
- [Array List](https://en.wikipedia.org/wiki/Array_list)
