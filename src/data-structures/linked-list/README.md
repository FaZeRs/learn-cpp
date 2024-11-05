# Linked List Data Structure

Linked Lists are linear data structures where elements are stored in nodes, and each node points to the next node in the sequence. Unlike arrays, linked lists do not store elements in contiguous memory locations.

## Data Structures

### Singly Linked List
- Each node contains data and a reference/pointer to the next node
- The last node points to NULL
- Only forward traversal is possible
```
[Data|Next] → [Data|Next] → [Data|NULL]
```

### Doubly Linked List
- Each node contains data and two references/pointers:
  - One to the next node
  - One to the previous node
- Allows both forward and backward traversal
```
NULL ← [Prev|Data|Next] ↔ [Prev|Data|Next] ↔ [Prev|Data|NULL]
```

### Circular Linked List
- Similar to singly linked list, but the last node points back to the first node
- Forms a circle-like structure
```
[Data|Next] → [Data|Next] → [Data|Next]
       ↑__________________________|
```

## Operations

Each implementation supports the following operations:

- `insert(data)`: Insert a new node
- `delete(data)`: Delete a node
- `search(data)`: Search for a node
- `display()`: Display the list
- `isEmpty()`: Check if list is empty
- `size()`: Get the size of the list

## Example

### Singly Linked List
```cpp
#include <forward_list>
#include <iostream>

int main() {
    // Create a singly linked list
    std::forward_list<int> singly_list;

    // Insert elements
    singly_list.push_front(3);  // Insert at front
    singly_list.push_front(2);
    singly_list.push_front(1);

    // Insert after a position
    auto it = singly_list.begin();
    singly_list.insert_after(it, 4);

    // Display list
    std::cout << "Singly linked list: ";
    for (const auto& element : singly_list) {
        std::cout << element << " ";
    }
    // Output: 1 4 2 3

    return 0;
}
```

### Doubly Linked List
```cpp
#include <list>
#include <iostream>

int main() {
    std::list<int> doubly_list;

    // Insert elements
    doubly_list.push_back(1);
    doubly_list.push_back(2);
    doubly_list.push_back(3);

    // Iterate forward
    std::cout << "Forward traversal: ";
    for (auto it = doubly_list.begin(); it != doubly_list.end(); ++it) {
        std::cout << *it << " ";
    }

    // Iterate backward
    std::cout << "\nBackward traversal: ";
    for (auto it = doubly_list.rbegin(); it != doubly_list.rend(); ++it) {
        std::cout << *it << " ";
    }

    return 0;
}
```

## Time Complexity

| Operation | Singly Linked | Doubly Linked | Circular Linked |
|-----------|---------------|---------------|-----------------|
| Insert (beginning) | O(1) | O(1) | O(1) |
| Insert (end) | O(n) | O(1)* | O(n) |
| Delete (beginning) | O(1) | O(1) | O(1) |
| Delete (end) | O(n) | O(1)* | O(n) |
| Search | O(n) | O(n) | O(n) |
| Access | O(n) | O(n) | O(n) |

\* With tail pointer

## References

- [Linked List on Wikipedia](https://en.wikipedia.org/wiki/Linked_list)
- [GeeksforGeeks - Linked List](https://www.geeksforgeeks.org/linked-list-data-structure/)